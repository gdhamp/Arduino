
#include "Wire.h"
#include "i2cLCD.h"

#define SPEED_8MHZ
#define __LCD__
#define __SERIAL__

// these are checked for in the main program

// initialize the library with the numbers of the interface pins
i2cLCD lcd(0);



float freq;
int intPart;
int fracPart;
char digits[9];

void setup () {

	intPart = 10;
	fracPart = 0;
	digits[4] = '.';
	digits[8] = 0;

#ifdef __SERIAL__
	Serial.begin(115200);       
	Serial.println("Strobe Control");
#endif

#ifdef __LCD__
	// set up the LCD's number of columns and rows: 
	lcd.begin(16,2);
	// Print a message to the LCD.
	lcd.print("Count:       ");
//	lcd.setCursor(0, 1);
#endif

} // end of setup


void loop () {


	uint8_t buttons = lcd.readButtons();

	// adjust counts by counting interval to give frequency in Hz
	float frq = (timerCount * 1000.0) / timerPeriod;

#ifdef __SERIAL__
	Serial.print ("Frequency: ");
	Serial.println ((unsigned long) frq);
#endif

#ifdef __LCD__
	if (updateLCD)
	{
		lcd.setCursor(0, 0);
		digits[0] = (intPart/1000) % 10 + '0';
		digits[1] = (intPart/100) % 10 + '0';
		digits[2] = (intPart/10) % 10 + '0';
		digits[3] = (intPart) % 10 + '0';

		digits[5] = (fracPart/100) % 10 + '0';
		digits[6] = (fracPart/10) % 10 + '0';
		digits[7] = (fracPart) % 10 + '0';

		lcd.print(digits);
	}

	

#endif


#ifdef __SERIAL__
	// let serial stuff finish
	delay(200);
#endif

}

