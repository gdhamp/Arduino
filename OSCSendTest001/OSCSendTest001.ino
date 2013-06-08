#include <Ethernet.h>
#include <EthernetUdp.h>
#include <SPI.h>    

#include <OSCMessage.h>
#include <DHT.h>
#include <Barometer.h>
#include <Twitter.h>
 
#define TWITTER

int IsTime(unsigned long *timeMark, unsigned long timeInterval);

EthernetUDP Udp;

// the Arduino's IP
IPAddress ip(192, 168, 1, 101);
// destination IP
IPAddress outIp(192, 168, 1, 44);
const unsigned int outPort = 7110;

// you can find this written on the board of some Arduino Ethernets or shields
byte mac[] = {  0x90, 0xA2, 0xDA, 0x00, 0x30, 0x89 };
byte gateway[] = { 192, 168, 1, 1 };
byte subnet[] = { 255, 255, 255, 0 };

#ifdef TWITTER
#include "TwitterToken.h"
unsigned long twitterTimeMark = 0;
unsigned long twitterTimeInterval = 120;
Twitter twitter(TwitterToken);
#endif

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
	Ethernet.begin(mac, ip, gateway, subnet);
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

#ifdef TWITTER
	if (IsTime(&twitterTimeMark, twitterTimeInterval))
	{
		if (twitter.post("Test Tweet"))
		{
			int status = twitter.wait();
			if (status == 200)
			{
				Serial.println("OK");
			}
			else
			{
				Serial.print("failed: code ");
				Serial.print(status);
			}
		}
		twitterTimeMark = 0;
	}
#endif

//  delay(20);
}



#define TIMECTL_MAXTICKS  4294967295UL
#define TIMECTL_INIT      0

int IsTime(unsigned long *timeMark, unsigned long timeInterval)
{
	unsigned long timeCurrent;
	unsigned long timeElapsed;
	int result=false;
  
	timeCurrent=millis();

	// Rollover?
	if(timeCurrent<*timeMark)
	{
		//elapsed=all the ticks to overflow + all the ticks since overflow
		timeElapsed=(TIMECTL_MAXTICKS-*timeMark)+timeCurrent;
	}
	else	
	{
		timeElapsed=timeCurrent-*timeMark;  
	}

	if(timeElapsed>=timeInterval)
	{
		*timeMark=timeCurrent;
		result=true;
	}
	return(result);  
}
