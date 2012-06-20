
// include the library code:
#include <Wire.h>
#include <Adafruit_MCP23017.h>
#include <Adafruit_RGBLCDShield.h>
#include <PString.h>

#define SPEED_8MHZ
//#define __SERIAL__


#define DEFAULT_LONGPRESS_LEN    25  // Min nr of loops for a long press
#define DELAY                    20  // Delay per loop in ms

//////////////////////////////////////////////////////////////////////////////

enum { EV_NONE=0, EV_SHORTPRESS, EV_LONGPRESS };

//////////////////////////////////////////////////////////////////////////////
// Class definition

class ButtonHandler {
public:
	// Constructor
    ButtonHandler(uint8_t buttonMask, int longpress_len=DEFAULT_LONGPRESS_LEN);

	// Initialization done after construction, to permit static instances
	void init();

	// Handler, to be called in the loop()
	int handle(uint8_t buttonState);

protected:
	boolean was_pressed;     // previous state
	int pressed_counter;     // press running duration
	const uint8_t buttonMask;           // mask of the button
	const int longpress_len; // longpress duration
};

ButtonHandler::ButtonHandler(uint8_t mask, int lp)
: buttonMask(mask), longpress_len(lp)
{
}

void ButtonHandler::init()
{
	was_pressed = false;
	pressed_counter = 0;
}

int ButtonHandler::handle(uint8_t buttonState)
{
	int event;
	int now_pressed = buttonState & buttonMask;

	if (!now_pressed && was_pressed) {
		// handle release event
		if (pressed_counter < longpress_len)
			event = EV_SHORTPRESS;
		else
			event = EV_LONGPRESS;
	}
	else
		event = EV_NONE;

	// update press running duration
	if (now_pressed)
		++pressed_counter;
	else
		pressed_counter = 0;

	// remember state, and we're done
	was_pressed = now_pressed;
	return event;
}

//////////////////////////////////////////////////////////////////////////////

// Instanciate button objects
ButtonHandler buttonUp(BUTTON_UP);
ButtonHandler buttonDown(BUTTON_DOWN, DEFAULT_LONGPRESS_LEN*2);
ButtonHandler buttonLeft(BUTTON_LEFT);
ButtonHandler buttonRight(BUTTON_RIGHT);


// initialize the library 
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

	// init buttons
	buttonUp.init();
	buttonDown.init();
	buttonLeft.init();
	buttonRight.init();

	// set up the LCD's number of columns and rows: 
	lcd.begin(16,2);
	// Print a message to the LCD.
	//         0123456789012345
	lcd.print("Freq: ");
	lcd.cursor();
	lcd.setCursor(cursorPos + 6, 0);

} // end of setup

float blah[8] = { 1000.0, 100.0, 10.0, 1.0, 0.0, 0.1, 0.01, 0.001 };

void loop () {

	// handle button
	uint8_t buttonState = lcd.readButtons();
	int upEvent = buttonUp.handle(buttonState);
	int downEvent = buttonDown.handle(buttonState);
	int leftEvent = buttonLeft.handle(buttonState);
	int rightEvent = buttonRight.handle(buttonState);

	if (leftEvent == EV_SHORTPRESS)
	{
		if (cursorPos > 0)
			cursorPos--;
	}

	if (rightEvent == EV_SHORTPRESS)
	{
		if (cursorPos < 7)
			cursorPos++;
	}

	
	if (upEvent == EV_SHORTPRESS)
	{
		freq += blah[cursorPos];	
		if (freq > 2000.0)
			freq = 2000.0;
		updateLCD = true;
	}
		
		

	if (downEvent == EV_SHORTPRESS)
	{
		float dec = blah[cursorPos];
		if (freq <  2.0 * dec)
		{
			if (cursorPos < 7)
				cursorPos++;
		}
		freq -= dec;	
		if (freq < 0.0)
			freq = 0.1;

		updateLCD = true;
	}


	if (updateLCD)
	{
		numString.begin();
		numString.print(freq, 3);
                
		lcd.setCursor(6,0);
		lcd.print("        ");
		lcd.setCursor(14 - numString.length(),0);
		lcd.print(numString);
		updateLCD = false;
               

#ifdef __SERIAL__
		Serial.println(12 - numString.length());
		Serial.println(numString);
#endif
	}

	lcd.setCursor(cursorPos + 6, 0);
	



#ifdef __SERIAL__
	// let serial stuff finish
	delay(200);
#endif

  delay(DELAY);
}



