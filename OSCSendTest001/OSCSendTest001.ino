#include <OSCMessage.h>

/*
    Make an OSC message and send it over UDP
    
    Adrian Freed
 */
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <SPI.h>    
#include <OSCMessage.h>

EthernetUDP Udp;

//the Arduino's IP
IPAddress ip(192, 168, 1, 101);
//destination IP
IPAddress outIp(192, 168, 1, 44);
const unsigned int outPort = 7110;

// you can find this written on the board of some Arduino Ethernets or shields
byte mac[] =
{  
//  0x90, 0xA2, 0xDA, 0x0D, 0x3B, 0xD1
  0x90, 0xA2, 0xDA, 0x00, 0x30, 0x80
};

void setup()
{
  Ethernet.begin(mac,ip);
    Udp.begin(8888);
}


void loop()
{
  //the message wants an OSC address as first argument
  OSCMessage msg("/analog/0");
  msg.add((int32_t)analogRead(0));
  
  Udp.beginPacket(outIp, outPort);
    msg.send(Udp); // send the bytes to the SLIP stream
  Udp.endPacket(); // mark the end of the OSC Packet
  msg.empty(); // free space occupied by message

//  delay(20);
}
