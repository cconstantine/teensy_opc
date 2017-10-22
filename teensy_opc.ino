#include <SPI.h>
#include <Ethernet.h>

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; 

EthernetServer server(7890);

void setup()
{
  pinMode(9, OUTPUT);
  digitalWrite(9, LOW);    // begin reset the WIZ820io
  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);  // de-select WIZ820io
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);   // de-select the SD Card
  digitalWrite(9, HIGH);   // end reset pulse

  SPI.begin();
  Ethernet.begin(mac);
  Serial.print("My IP Address: ");
  Serial.println(Ethernet.localIP());

  server.begin();
}

const unsigned int buf_size = 1024;
uint8_t buf[buf_size];

const int fixup_delay = 0;
void loop()
{
  EthernetClient client = server.available();

  if (client.connected()) {
    unsigned long now = millis();
    unsigned int command_read = 0;
    uint8_t header[4];

    /************* Top of loop delay *********************/
    delay(fixup_delay);
    /*****************************************************/
    if (client.available() < 4) {
      return;
    }
    command_read += client.read(header, 4);

    if (command_read < 4) {
      Serial.print("Failed to read complete header: ");
      Serial.println(command_read);
      
      client.stop();
      return;
    }
    
    uint8_t channel = header[0];
    uint8_t command = header[1];
    uint16_t len = ((uint16_t)header[2] << 8) + header[3];

    Serial.print("header (");Serial.print(command_read);Serial.println("):");
    Serial.println(header[0], HEX);
    Serial.println(header[1], HEX);
    Serial.println(header[2], HEX);
    Serial.println(header[3], HEX);
    Serial.print("channel: ");Serial.println(channel);
    Serial.print("command: ");Serial.println(command);
    Serial.print("length : ");Serial.println(len);
    
    if (channel != 0 || command != 0) {
      client.stop();
      return;
    }

    while(len > 0) {
      unsigned int to_read = buf_size < len ? buf_size : len;
      
      /************** Inner loop delay *********************/
      delay(fixup_delay);
      /*****************************************************/

      int read_bytes = client.read(buf, to_read);
      if (read_bytes > 0) {
        len -= read_bytes;
        command_read += read_bytes;
      }
    }
    Serial.print("first byte   : ");Serial.println(buf[0], HEX);
    Serial.print("command_read : ");Serial.println(command_read);
    Serial.print("available    : ");Serial.println(client.available());
    Serial.print("receive time :");Serial.println(millis() - now);
    Serial.println();
  }
}
