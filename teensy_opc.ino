#include <SPI.h>
#include <Ethernet.h>
#include <EthernetBonjour.h>

#define USE_OCTOWS2811
#include<OctoWS2811.h>
#include <FastLED.h>

#include "TeensyMAC.h"


#define NUM_LEDS_PER_STRIP 256
#define NUM_STRIPS 8
#define BRIGHTNESS 64
const double desired_fps = 60.0;

CRGB leds[NUM_STRIPS * NUM_LEDS_PER_STRIP];

EthernetServer server(7890);

const double frame_millis = 1000.0f / desired_fps;
void setup()
{
  memset((uint8_t*)&leds, 0, sizeof(leds));

  LEDS.addLeds<OCTOWS2811, RGB>(leds, NUM_LEDS_PER_STRIP);
  LEDS.setBrightness(BRIGHTNESS);
  LEDS.setDither( 0 );
  LEDS.show();

  Serial.begin(9600);
  

  pinMode(9, OUTPUT);
  digitalWrite(9, LOW);    // begin reset the WIZ820io
  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);  // de-select WIZ820io
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);   // de-select the SD Card
  digitalWrite(9, HIGH);   // end reset pulse
  
  uint64_t mac64;
  uint8_t mac[6];
  
  mac64 = teensyMAC();
  mac[0] = mac64 >> 40;
  mac[1] = mac64 >> 32;
  mac[2] = mac64 >> 24;
  mac[3] = mac64 >> 16;
  mac[4] = mac64 >> 8;
  mac[5] = mac64;

  SPI.begin();

  Ethernet.begin(mac);
  String hostname = String("teensy-opc-") + String(teensySerial());
  EthernetBonjour.begin(hostname.c_str());
  Serial.println("Welcome to teensy_opc!");

  Serial.printf("Serial:     %u\n", teensySerial());
  Serial.printf("Hostname:   %s.local\n", hostname.c_str());
  Serial.printf("MAC:        0x%012llX\n", teensyMAC());
  //Serial.print("frame_millis: ");Serial.println(frame_millis);
  if (IPAddress(0, 0, 0, 0) != Ethernet.localIP()) {
    Serial.print("IP Address: ");
    Serial.println(Ethernet.localIP());  
  }

  server.begin();
}


void loop()
{
  static unsigned long lastFrame = millis();
  IPAddress prev_ip = Ethernet.localIP();
  Ethernet.maintain();
  if (prev_ip != Ethernet.localIP()) {
    Serial.print("IP Address: ");
    Serial.println(Ethernet.localIP());
  }
  EthernetBonjour.run();

  if (millis() - lastFrame > 1000) {
    Serial.print("No new frames in ");Serial.print(millis() - lastFrame);Serial.println("ms.  Clearing LEDs.");
    memset((uint8_t*)&leds, 0, sizeof(leds));
  }

  EthernetClient client = server.available();

  if (client.connected() && client.available() >= 4) {
    unsigned long start = millis();
    uint8_t header[4];

    if (client.read(header, 4) < 4) {
      Serial.println("Failed to read all 4 header bytes");
      client.stop();
      return;
    }
    
    uint8_t channel = header[0];
    uint8_t command = header[1];
    uint16_t len = ((uint16_t)header[2] << 8) + header[3];
    
    //Serial.print("channel: ");Serial.println(channel);
    //Serial.print("command: ");Serial.println(command);
    //Serial.print("length : ");Serial.println(len);
    
    if (channel != 0 || command != 0 || len == 0) {
      Serial.println("Invalid length, command or channel");
      client.stop();
      return;
    }

    if (len > sizeof(leds)) {
      Serial.printf("Length of %d is bigger than led buffer of %d\n", len, sizeof(leds));
      client.stop();
      return;
    }
    
    uint8_t* buf = (uint8_t*)&leds;
    while (len > 0 ) {
      if (frame_millis <  millis() - start) {
        Serial.println("OPC Message read timed out");
        client.stop();
        return;
      }

      int read_bytes;
      if ((read_bytes = client.available()) > 0) {

        if (read_bytes > len)  read_bytes = len;
        
        int bytes_read = client.read(buf, read_bytes);
        len -= bytes_read;
        buf += bytes_read;
      }
    }
    
    LEDS.show();
    lastFrame = millis();
    long duration = lastFrame - start;
    if(duration > 16) {
      Serial.print("duration:");Serial.print(duration);Serial.println("ms");
      Serial.println();
    }

  } else {
    LEDS.show();
  }
}
