/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.
 */

#define TRIAC_A 2
#define NUM_TRIACS 8
#define DEALY 50
unsigned int g_value = 0;

void setup()
{                
  pinMode(10, OUTPUT);       
  for (int i=0; i<NUM_TRIACS; i++)
  {
    pinMode(TRIAC_A + i, OUTPUT);
  }
}

void writeTriacs(unsigned int value)
{
  for (int i=0; i<NUM_TRIACS; i++)
  {
    if (value & 1)
    {
      digitalWrite(TRIAC_A + i, LOW);
    }
    else
    {
      digitalWrite(TRIAC_A + i, HIGH);
    }
    value = value >> 1;
  }
}

void loop() {
  writeTriacs(g_value);
  g_value++;
  digitalWrite(10, HIGH);   // set the LED on
  delay(DELAY);              // wait for a second
  writeTriacs(g_value);
  g_value++;
  digitalWrite(10, LOW);    // set the LED off
  delay(DELAY);              // wait for a second
}
