#include <LiquidCrystal.h>
#include <TimerOne.h>

#define TSL_FREQ_PIN 2 // output use digital pin2 for interrupt

int ledPin = 13;
int ledState = LOW;

unsigned short isrPulseCount = 0;
boolean PulseCountValid = false;

unsigned short PulseCount = 0;
unsigned short PulseCountMax = 0;

int PBPin = 8;
unsigned char PBCount = 0;

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(0, 1, 4, 5, 6, 7);

void setup()
{
  // attach interrupt to pin2, send output pin of TSL230R to arduino 2
  // call handler on each rising pulse

  attachInterrupt(0, add_pulse, RISING);
  pinMode(TSL_FREQ_PIN, INPUT);
  pinMode(13, OUTPUT);
  

  // Set timer for 1 second interval and attach the ISR
  Timer1.initialize(100000);
  Timer1.attachInterrupt( timerIsr );

  // set up the LCD's number of columns and rows: 
  lcd.begin(16,2);
  // Print a message to the LCD.
  lcd.print("Count:");
  lcd.setCursor(0, 1);
  lcd.print("Max:");

  // push button to reset max
  pinMode(PBPin, INPUT);
}


void add_pulse()
{
  isrPulseCount++;
  return;
}

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


void loop()
{
  if (PulseCountValid)
  {
    if (ledState == LOW){
      ledState = HIGH;
    } else {
      ledState = LOW;
    }

    //toggle the LED based on our switch above
    digitalWrite(ledPin, ledState);
if (0)
{
    PulseCountValid = false;
    lcd.setCursor(7, 0);
    lcd.print("      ");
    lcd.setCursor(7, 0);
    lcd.print(PulseCount);
    lcd.setCursor(7, 1);
    lcd.print("      ");
    lcd.setCursor(7, 1);
    lcd.print(PulseCountMax);
}
  }
}




