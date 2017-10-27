# Teensy OPC

The purpose of this project is to have an easy to build and deploy LED controller that speaks the Open Pixel Control protocol.

## Hardware Parts List
You will need to solder the following things together:
 - Teensy 3.1/3.2 ( https://www.pjrc.com/store/teensy32.html )
 - Wiznet wiz850io ( http://www.shopwiznet.com/wiz850io )
 - WIZ820io & Micro SD Card Adaptor ( https://www.pjrc.com/store/wiz820_sd_adaptor.html )
 - OctoWS2811 Adaptor ( https://www.pjrc.com/store/octo28_adaptor.html )
 - Headers and Pins ( https://www.pjrc.com/store/socket_14x1.html / https://www.pjrc.com/store/header_14x1_d.html ) 
 
## Software

To run this arduino sketch you will need:
 - Arduino IDE ( https://www.arduino.cc/en/Main/Software )
 - The FastLED Library (Version 3.1 or later)
 - EthernetBonjour ( https://github.com/TrippyLighting/EthernetBonjour )
 
 
## Operations

On boot, this sketch attempts to get an IP address via DHCP, and broadcasts a zeroconf/bonjour hostname that is like `teensy-opc-SERIAL.local`.  The hostname and IP address will be printed to the serial console.  Ethernet setup only happens on boot, so you must have the ethernet cable plugged in when you apply power.

To configure the LEDs you expect to talk to you currently need to modify the sketch.  There are some constants at the top that help define your LED setup.


## TODO
 - Allow ethernet configuration at any time after power-on, not just boot
 - Use the SD card to configure things like number and type of LEDs, and override the default hostname
