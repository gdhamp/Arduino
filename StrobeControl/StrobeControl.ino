
// include the library code:
#include <Wire.h>
#include <Adafruit_MCP23017.h>
#include <Adafruit_RGBLCDShield.h>
#include <PString.h>
//#include <TimerOne.h>

#define SPEED_8MHZ
//#define __SERIAL__

// initialize the library 
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

char numBuffer[17];
PString numString(numBuffer, sizeof(numBuffer));

float clockFreq = 16000000.0;
float freq;
boolean freqChanged;
int cursorPos;

bool calcTimerValues(float freq, word* firstTimerVal, int* numTimerIntervals, word* lastTimerVal);

#define DEFAULT_LONGPRESS_LEN    25  // Min nr of loops for a long press
#define DELAY                    20  // Delay per loop in ms


class StrobeTimer
{
public:
	void Init();
	void SetFreq(word firstTimerVal, int numTimerIntervals, word lastTimerVal);
	void Start(void);
	void Stop(void);
	void StrobeISR(void);
	
	
protected:
	unsigned char clockSelectBits;
	char oldSREG;
	word m_FirstTimerVal;
	int  m_NumIntervals;
	word m_LastTimerVal;

	word m_CurFirstTimerVal;
	int  m_CurNumIntervals;
	word m_CurLastTimerVal;

};

StrobeTimer StrobeTimer1;              // preinstatiate

ISR(TIMER1_COMPA_vect)          // interrupt service routine
{
  StrobeTimer1.StrobeISR();
}

void StrobeTimer::Init(void)
{
	// Set COM1A to 0 - No Output on Match
	// WGM(13:10) to 4 - CTC mode
	// CS1(2:0) to 1 - No Prescale)
	TCCR1A = 0;
	TCCR1B = _BV(WGM12);
	clockSelectBits = _BV(CS10);
}

void StrobeTimer::SetFreq(word firstTimerVal, int numTimerIntervals, word lastTimerVal)
{
	// Disable Interrupts
	oldSREG = SREG;
	cli();

	m_FirstTimerVal = firstTimerVal;
	m_NumIntervals = numTimerIntervals;
	m_LastTimerVal = lastTimerVal;

	SREG = oldSREG;
}

void StrobeTimer::Start(void)
{
	// Disable Interrupts
	oldSREG = SREG;
	cli();

	m_CurFirstTimerVal = m_FirstTimerVal;
	m_CurNumIntervals = m_NumIntervals;
	m_CurLastTimerVal = m_LastTimerVal;
	
	// Set the counter value
	TCNT1 = 0;
	OCR1A = m_CurFirstTimerVal;
	SREG = oldSREG;

	// Setup the ISR and the interrupt mask
	TIMSK1 = OCIE1A;

	// Start the timer
	TCCR1B |= clockSelectBits;
}

void StrobeTimer::Stop(void)
{
	// Stop the timer
	TCCR1B &= ~(_BV(CS12) | _BV(CS11) | _BV(CS10));
}

void StrobeTimer::StrobeISR(void)
{
	if (0==m_CurNumIntervals--)
	{
		// Next match causes output to go low
//		TCCR1A |= _BV(COM1A1) | _BV(COM1A0);
		TCCR1A |= _BV(COM1A0);
		OCR1A = m_CurLastTimerVal;
		
		m_CurFirstTimerVal = m_FirstTimerVal;
		m_CurNumIntervals = m_NumIntervals;
		m_CurLastTimerVal = m_LastTimerVal;
	}
	else
	{
		// if still counting intervals disable output pin change
//		TCCR1A &= ~(_BV(COM1A1) | _BV(COM1A0));
		TCCR1A &= ~(_BV(COM1A0));
		OCR1A = m_CurFirstTimerVal;
	}
}

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


void setup () {

	word firstTimerVal;
	int numTimerIntervals;
	word lastTimerVal;

	freq = 12.234;
	cursorPos = 3;
	freqChanged = true;

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

	pinMode(9, OUTPUT);
	calcTimerValues(freq, &firstTimerVal, &numTimerIntervals, &lastTimerVal);
	StrobeTimer1.SetFreq(firstTimerVal, numTimerIntervals, lastTimerVal);
	StrobeTimer1.Start();
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
		freqChanged = true;
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

		freqChanged = true;
	}


	if (freqChanged)
	{
		word firstTimerVal;
		int numTimerIntervals;
		word lastTimerVal;

		calcTimerValues(freq, &firstTimerVal, &numTimerIntervals, &lastTimerVal);
		StrobeTimer1.SetFreq(firstTimerVal, numTimerIntervals, lastTimerVal);
		numString.begin();
		numString.print(firstTimerVal);
		numString.print(" ");
		numString.print(numTimerIntervals);
		numString.print(" ");
		numString.print(lastTimerVal);
		numString.print(" ");
			
		lcd.setCursor(0,1);
		lcd.print(numString);
		
		numString.begin();
		numString.print(freq, 3);
                
		lcd.setCursor(6,0);
		lcd.print("        ");
		lcd.setCursor(14 - numString.length(),0);
		lcd.print(numString);
		freqChanged = false;
               

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

bool calcTimerValues(float freq, word* firstTimerVal, int* numTimerIntervals, word* lastTimerVal)
{
	word timerVal = 65535;
	word minTimerVal = 32768;
	long numClocks = long (clockFreq / freq + 0.5);
	int numIntervals = int(numClocks/timerVal);
	word lastVal = numClocks % timerVal;

	if (numIntervals != 0)
	{
		if (lastVal < minTimerVal)
		{
			word delta = minTimerVal - lastVal;
			int decrement = delta / numIntervals + 1;
			timerVal -= decrement;
			numIntervals = int (numClocks /timerVal);
			lastVal = numClocks % timerVal;
		}
	}

	*firstTimerVal = timerVal;
	*numTimerIntervals = numIntervals;
	*lastTimerVal = lastVal;
	return true;
}


