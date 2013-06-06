#include <Ethernet.h>
#include <EthernetUdp.h>
#include <SPI.h>    

#include <OSCMessage.h>
#include <DHT.h>
#include <Barometer.h>

EthernetUDP Udp;

// the Arduino's IP
IPAddress ip(192, 168, 1, 101);
// destination IP
IPAddress outIp(192, 168, 1, 44);
const unsigned int outPort = 7110;

// you can find this written on the board of some Arduino Ethernets or shields
byte mac[] =
{  
	0x90, 0xA2, 0xDA, 0x00, 0x30, 0x89
};

// Temperature & Humidity Sensor
// AM2302 Seonsor on Grove board SEN51035P
// type is DHT22
#define DHTPIN 14
DHT dht(DHTPIN, DHT22);

// Barometer Sensor
// Bosch BMP085 Sensor on Grove Board SEN05291
Barometer myBarometer;

void setup()
{
	Ethernet.begin(mac,ip);
	Udp.begin(8888);

	dht.begin();
	myBarometer.init();
}


void loop()
{
	float temp;
	float humidity;
	float pressure;
	OSCMessage msg("/analog/0");
  
	Udp.beginPacket(outIp, outPort);
	msg.add((int32_t)analogRead(0));
	msg.send(Udp);
	msg.empty();
	Udp.endPacket();

	Udp.beginPacket(outIp, outPort);
	msg.setAddress("/weather/temp0");
	temp = dht.readTemperature();
	msg.add(temp);
	msg.send(Udp);
	msg.empty();
	Udp.endPacket();

	Udp.beginPacket(outIp, outPort);
	msg.setAddress("/weather/humidity");
	humidity = dht.readHumidity();
	msg.add(humidity);
	msg.send(Udp);
	msg.empty();
	Udp.endPacket();

	//Get the temperature, bmp085ReadUT MUST be called first
	temp = myBarometer.bmp085GetTemperature(myBarometer.bmp085ReadUT());
	//Get the temperature
	pressure = myBarometer.bmp085GetPressure(myBarometer.bmp085ReadUP());

	Udp.beginPacket(outIp, outPort);
	msg.setAddress("/weather/pressure");
	msg.add(pressure);
	msg.send(Udp);
	msg.empty();
	Udp.endPacket();

	Udp.beginPacket(outIp, outPort);
	msg.setAddress("/weather/temp1");
	msg.add(temp);
	msg.send(Udp);
	msg.empty();
	Udp.endPacket();

//  delay(20);
}
