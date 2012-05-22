#include "Wire.h"
#include "i2cLCD.h"
#include <TimerOne.h>

#define __MYISR__
#define __LCD__


// output use digital pin2 for interrupt
#define TSL_FREQ_PIN 2

int ledPin = 13;
int ledState = LOW;

unsigned short isrPulseCount = 0;
boolean PulseCountValid = false;

unsigned short PulseCount = 0;
unsigned short PulseCountMax = 0;

int PBPin = 8;
unsigned char PBCount = 0;

// initialize the library with the numbers of the interface pins
i2cLCD lcd(0);

void timerIsr()
{
	if (digitalRead(PBPin) == LOW)
	{
		PBCount++;
		if (PBCount == 1)
		{
			PulseCountMax = 0;
		}
	}
	else
	{
		PBCount = 0;
	}
	PulseCount = isrPulseCount;
	if (PulseCount > PulseCountMax)
		PulseCountMax = PulseCount;

	isrPulseCount = 0;
	PulseCountValid = true;
}

#ifdef __MYISR__

ISR(INT0_vect, ISR_NAKED)
{
	uint8_t s;
	asm (
			"push __tmp_reg__" "\n\t"
			"in __tmp_reg__,__SREG__" "\n\t"
			"push r26" "\n\t"
			"push r27" "\n\t"
			::);

	asm (
			"adiw %0,1" "\n\t"
			: "=x" (isrPulseCount)
			: "0" (isrPulseCount));
	asm (
			"pop r27" "\n\t"
			"pop r26" "\n\t"
			"out __SREG__,__tmp_reg__" "\n\t"
			"pop __tmp_reg__" "\n\t"
			"reti" "\n\t"
			::);
}


ISR(INT1_vect)
{
}

void myattachInterrupt(uint8_t interruptNum, int mode) {
#define EXTERNAL_NUM_INTERRUPTS 2
  if(interruptNum < EXTERNAL_NUM_INTERRUPTS) {
    
    // Configure the interrupt mode (trigger on low input, any change, rising
    // edge, or falling edge).  The mode constants were chosen to correspond
    // to the configuration bits in the hardware register, so we simply shift
    // the mode into place.
      
    // Enable the interrupt.
      
    switch (interruptNum) {
#if defined(EICRA) && defined(EICRB) && defined(EIMSK)
    case 2:
      EICRA = (EICRA & ~((1 << ISC00) | (1 << ISC01))) | (mode << ISC00);
      EIMSK |= (1 << INT0);
      break;
    case 3:
      EICRA = (EICRA & ~((1 << ISC10) | (1 << ISC11))) | (mode << ISC10);
      EIMSK |= (1 << INT1);
      break;
    case 4:
      EICRA = (EICRA & ~((1 << ISC20) | (1 << ISC21))) | (mode << ISC20);
      EIMSK |= (1 << INT2);
      break;
    case 5:
      EICRA = (EICRA & ~((1 << ISC30) | (1 << ISC31))) | (mode << ISC30);
      EIMSK |= (1 << INT3);
      break;
    case 0:
      EICRB = (EICRB & ~((1 << ISC40) | (1 << ISC41))) | (mode << ISC40);
      EIMSK |= (1 << INT4);
      break;
    case 1:
      EICRB = (EICRB & ~((1 << ISC50) | (1 << ISC51))) | (mode << ISC50);
      EIMSK |= (1 << INT5);
      break;
    case 6:
      EICRB = (EICRB & ~((1 << ISC60) | (1 << ISC61))) | (mode << ISC60);
      EIMSK |= (1 << INT6);
      break;
    case 7:
      EICRB = (EICRB & ~((1 << ISC70) | (1 << ISC71))) | (mode << ISC70);
      EIMSK |= (1 << INT7);
      break;
#else
    case 0:
    #if defined(EICRA) && defined(ISC00) && defined(EIMSK)
      EICRA = (EICRA & ~((1 << ISC00) | (1 << ISC01))) | (mode << ISC00);
      EIMSK |= (1 << INT0);
    #elif defined(MCUCR) && defined(ISC00) && defined(GICR)
      MCUCR = (MCUCR & ~((1 << ISC00) | (1 << ISC01))) | (mode << ISC00);
      GICR |= (1 << INT0);
    #elif defined(MCUCR) && defined(ISC00) && defined(GIMSK)
      MCUCR = (MCUCR & ~((1 << ISC00) | (1 << ISC01))) | (mode << ISC00);
      GIMSK |= (1 << INT0);
    #else
      #error attachInterrupt not finished for this CPU (case 0)
    #endif
      break;

    case 1:
    #if defined(EICRA) && defined(ISC10) && defined(ISC11) && defined(EIMSK)
      EICRA = (EICRA & ~((1 << ISC10) | (1 << ISC11))) | (mode << ISC10);
      EIMSK |= (1 << INT1);
    #elif defined(MCUCR) && defined(ISC10) && defined(ISC11) && defined(GICR)
      MCUCR = (MCUCR & ~((1 << ISC10) | (1 << ISC11))) | (mode << ISC10);
      GICR |= (1 << INT1);
    #elif defined(MCUCR) && defined(ISC10) && defined(GIMSK) && defined(GIMSK)
      MCUCR = (MCUCR & ~((1 << ISC10) | (1 << ISC11))) | (mode << ISC10);
      GIMSK |= (1 << INT1);
    #else
      #warning attachInterrupt may need some more work for this cpu (case 1)
    #endif
      break;
#endif
    }
  }
}

#else
void add_pulse()
{
	isrPulseCount++;
	return;
}

#endif


void setup()
{
	// attach interrupt to pin2, send output pin of TSL230R to arduino 2
	// call handler on each rising pulse

#ifdef __MYISR__
	myattachInterrupt(0, RISING);
#else
	attachInterrupt(0, add_pulse, RISING);
#endif

	pinMode(TSL_FREQ_PIN, INPUT);
	pinMode(13, OUTPUT);
  

	// Set timer for 1 second interval and attach the ISR
	Timer1.initialize(100000);
	Timer1.attachInterrupt( timerIsr );

#ifdef __LCD__
	// set up the LCD's number of columns and rows: 
	lcd.begin(16,2);
	// Print a message to the LCD.
	lcd.print("Count:");
	lcd.setCursor(0, 1);
	lcd.print("Max:");
#endif

	// push button to reset max
	pinMode(PBPin, INPUT);
}


void loop()
{
	if (PulseCountValid)
	{
		if (ledState == LOW)
			ledState = HIGH;
		else
			ledState = LOW;

		//toggle the LED based on our switch above
		digitalWrite(ledPin, ledState);
		PulseCountValid = false;
#ifdef __LCD__
		lcd.setCursor(7, 0);
		lcd.print("      ");
		lcd.setCursor(7, 0);
		lcd.print(PulseCount);
		lcd.setCursor(7, 1);
		lcd.print("      ");
		lcd.setCursor(7, 1);
		lcd.print(PulseCountMax);
#endif
	}
}




