
#include "Wire.h"
#include "i2cLCD.h"

#define SPEED_8MHZ
//#define __LCD__
//#define __SERIAL__

// these are checked for in the main program
volatile unsigned long timerCount;
volatile boolean counterReady;
volatile unsigned long RedMaxCount = 0;
volatile unsigned long IRMaxCount = 0;

// internal to counting routine
unsigned long overflowCount;
unsigned int timerTicks;
unsigned int timerPeriod;

// initialize the library with the numbers of the interface pins
i2cLCD lcd(0);

#define LED_RED 0
#define LED_IR  1
unsigned int theLED;

// Pin # of the pushbutton
int PBPin = 8;
boolean buttonIsPushed = false;

void startCounting (unsigned int ms) 
{

	counterReady = false;		// time not up yet
	timerPeriod = ms;			// how many 1 mS counts to do
	timerTicks = 0;				// reset interrupt counter
	overflowCount = 0;			// no overflows yet

	if (theLED == LED_RED)
	{
		digitalWrite(2, LOW);
		digitalWrite(3, HIGH);
	}
	else
	{
		digitalWrite(3, LOW);
		digitalWrite(2, HIGH);
	}

	// reset Timer 1 and Timer 2
	TCCR1A = 0;
	TCCR1B = 0;
	TCCR2A = 0;
	TCCR2B = 0;

	// Timer 1 - counts events on pin D5
	TIFR1  = 0xff;					// Clear timer1 interupts
	TIMSK1 = _BV (TOIE1);		// interrupt on Timer 1 overflow

	// Timer 2 - gives us our 1 mS counting interval
	// 16 MHz clock (62.5 nS per tick) - prescaled by 128
	// counter increments every 8 uS. 
	// So we count 125 of them, giving exactly 1000 uS (1 mS)
	TCCR2A = _BV (WGM21);		// CTC mode
	OCR2A  = 124;				// count up to 125  (zero relative!!!!)

	// Timer 2 - interrupt on match (ie. every 1 mS)
	TIMSK2 = _BV (OCIE2A);		// enable Timer2 Interrupt

	TCNT1 = 0;					// Both counters to zero
	TCNT2 = 0;

	// Reset prescalers
	GTCCR = _BV (PSRASY);		// reset prescaler now

	// start Timer 2
#ifdef SPEED_8MHZ
	TCCR2B = _BV (CS22);				// prescaler of 64
#else
	TCCR2B = _BV (CS20) | _BV (CS22);	// prescaler of 128
#endif

	// start Timer 1
	// External clock source on T1 pin (D5). Clock on rising edge.
	TCCR1B = _BV (CS10) | _BV (CS11) | _BV (CS12);

}

ISR (TIMER1_OVF_vect)
{
	++overflowCount;			// count number of Counter1 overflows
}


//******************************************************************
//  Timer2 Interrupt Service is invoked by hardware Timer 2 every 1ms = 1000 Hz
//  16Mhz / 128 / 125 = 1000 Hz

ISR (TIMER2_COMPA_vect) 
{
	// grab counter value before it changes any more
	unsigned int timer1CounterValue;
	timer1CounterValue = TCNT1;			// see datasheet, page 117 (accessing 16-bit registers)

	// Reset the max if the button has been pressed
	if (digitalRead(PBPin) == LOW)
	{
		// is the first time we've sensed the button being pushed
		if (!buttonIsPushed)
		{
			RedMaxCount = 0;
			IRMaxCount = 0;
		}
		buttonIsPushed = true;
	}
	else
		buttonIsPushed = false;

	// see if we have reached timing period
	if (++timerTicks < timerPeriod) 
		return;							// not yet


	// if just missed an overflow
	if (TIFR1 & TOV1)
		overflowCount++;

	// end of gate time, measurement ready

	TCCR1A = 0;		// stop timer 1
	TCCR1B = 0;

	TCCR2A = 0;		// stop timer 2
	TCCR2B = 0;

	TIMSK1 = 0;		// disable Timer1 Interrupt
	TIMSK2 = 0;		// disable Timer2 Interrupt

	// calculate total count
	timerCount = (overflowCount << 16) + timer1CounterValue;	// each overflow is 65536 more

	if (theLED == LED_RED)
	{
		if (timerCount > RedMaxCount)
			RedMaxCount = timerCount;
	}
	else
	{
		if (timerCount > IRMaxCount)
			IRMaxCount = timerCount;
	}

	counterReady = true;										// set global flag for end count period
}


void setup () {


#ifdef __SERIAL__
	Serial.begin(115200);       
	Serial.println("Frequency Counter");
#endif

#ifdef __LCD__
	// set up the LCD's number of columns and rows: 
	lcd.begin(16,2);
	// Print a message to the LCD.
	lcd.print("Count:       ");
	lcd.setCursor(0, 1);
//			   01234567890123456
	lcd.print("R       I        ");
#endif

	pinMode(2, OUTPUT);
	digitalWrite(2, LOW);
	pinMode(3, OUTPUT);
	digitalWrite(3, LOW);

	theLED = LED_RED;

	RedMaxCount = 0;
	IRMaxCount = 0;
} // end of setup


void loop () {

	// stop Timer 0 interrupts from throwing the count out
	byte oldTCCR0A = TCCR0A;
	byte oldTCCR0B = TCCR0B;
	TCCR0A = 0;				// stop timer 0
	TCCR0B = 0;

	Serial.println ((unsigned int) theLED);
	startCounting (250);		// how many mS to count for

	
	while (!counterReady) 
		{}

	// adjust counts by counting interval to give frequency in Hz
	float frq = (timerCount * 1000.0) / timerPeriod;

#ifdef __SERIAL__
	Serial.print ("Frequency: ");
	Serial.println ((unsigned long) frq);
#endif

#ifdef __LCD__
	lcd.setCursor(7, 0);
	lcd.print("      ");
	lcd.setCursor(7, 0);
	lcd.print(timerCount);
	lcd.setCursor(2, 1);
	lcd.print("      ");
	lcd.setCursor(2, 1);
	lcd.print(RedMaxCount);
	lcd.setCursor(10, 1);
	lcd.print("      ");
	lcd.setCursor(10, 1);
	lcd.print(IRMaxCount);
#endif

	// restart timer 0
	TCCR0A = oldTCCR0A;
	TCCR0B = oldTCCR0B;
  
	if (theLED == LED_RED)
		theLED = LED_IR;
	else
		theLED = LED_RED;

#ifdef __SERIAL__
	// let serial stuff finish
	delay(200);
#endif

}

