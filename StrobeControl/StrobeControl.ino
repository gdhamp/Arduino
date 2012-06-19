
// include the library code:
#include <Wire.h>
#include <Adafruit_MCP23017.h>
#include <Adafruit_RGBLCDShield.h>
#include <PString.h>

#define SPEED_8MHZ
#define __LCD__
#define __SERIAL__

// these are checked for in the main program

// initialize the library with the numbers of the interface pins
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();



char numBuffer[10];
PString numString(numBuffer, sizeof(numBuffer));

float freq;
int updateLCD;
int cursorPos;

void setup () {

        freq = 12.234;
        cursorPos = 3;
        updateLCD = true;

#ifdef __SERIAL__
	Serial.begin(115200);       
	Serial.println("Strobe Control");
#endif

#ifdef __LCD__
	// set up the LCD's number of columns and rows: 
	lcd.begin(16,2);
	// Print a message to the LCD.
        //         0123456789012345
	lcd.print("Freq: ");
	lcd.cursor();
        lcd.setCursor(cursorPos + 6, 0);
#endif

} // end of setup


void loop () {


	uint8_t buttons = lcd.readButtons();


        if (buttons & BUTTON_UP)
        
        {
        }

#ifdef __LCD__
	if (updateLCD)
	{
                numString.print(freq, 3);
                
                
                lcd.setCursor(14 - numString.length(),0);
		lcd.print(numString);
                updateLCD = false;
               
                lcd.setCursor(cursorPos + 6, 0);
#ifdef __SERIAL__
                Serial.println(12 - numString.length());
        	Serial.println(numString);
#endif
	}

	

#endif


#ifdef __SERIAL__
	// let serial stuff finish
	delay(200);
#endif

}

