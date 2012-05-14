

// include the library code:
#include <Wire.h>
#include <Adafruit_MCP23017.h>
#include <Adafruit_RGBLCDShield.h>


Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

#define SHUTTER_PIN 13

void setup() {
  lcd.begin(16, 2);

  lcd.print("Camera Control");
  lcd.setBacklight(1);
  
  digitalWrite(SHUTTER_PIN, LOW);
  pinMode(SHUTTER_PIN, OUTPUT);
}

uint8_t i=0;
void loop() {

  uint8_t buttons = lcd.readButtons();

  if (buttons)
  {
    if (buttons & BUTTON_DOWN)
    {
      lcd.setCursor(0, 1);
      lcd.print("Go");

      digitalWrite(SHUTTER_PIN, HIGH);
      delay(2000);
      digitalWrite(SHUTTER_PIN, LOW);
      delay(300);
      digitalWrite(SHUTTER_PIN, HIGH);
      delay(1000);
      digitalWrite(SHUTTER_PIN, LOW);

      lcd.setCursor(0, 1);
      lcd.print("  ");
    }
  }
}
