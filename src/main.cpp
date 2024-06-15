#include <Wire.h>
#include <max6675.h>
#include <string> 

#include "Display.h"
#include "Timer.h"

#define OLED_WIDTH 128
#define OLED_HEIGHT 64

// Built in led (Uses Adafruit_SSD1306)
//#define SDA_PIN 14
//#define SCL_PIN 12

#define SDA_PIN 4
#define SCL_PIN 0
#define RELAY_PIN 2

#define SO_PIN_WATER 16 // D0
#define SO_PIN_EXHAUST 5 // D1
#define SCK_PIN 15 // D8
#define CS_PIN 13 // D7

Adafruit_SH1106 oled(-1);
Display display(&oled, OLED_WIDTH, OLED_HEIGHT);
MAX6675 waterSensor(SCK_PIN, CS_PIN, SO_PIN_WATER);
MAX6675 exhaustSensor(SCK_PIN, CS_PIN, SO_PIN_EXHAUST);

int exhaustTemp;
int waterTemp;
int upperLimit = 250;
int lowerLimit = 100;

Timer timer;

void setup() {
  pinMode(RELAY_PIN, OUTPUT);
  Wire.begin(SDA_PIN, SCL_PIN);
  display.init();
  display.displayLogo();
  delay(3000);
  digitalWrite(RELAY_PIN, LOW);
  exhaustTemp = exhaustSensor.readCelsius();
  waterTemp = waterSensor.readCelsius();
}



void loop() {
  unsigned long millisSinceStart = millis();
  int sinceStartedMinutes = millisSinceStart / 1000 / 60;
  boolean shouldHeat = !(sinceStartedMinutes > 30 && exhaustTemp < lowerLimit);
  display.display(exhaustTemp, waterTemp, lowerLimit, upperLimit, shouldHeat, sinceStartedMinutes);
  if (timer.hasPassed(5, millisSinceStart)) // Only read temp every 5 seconds
  {
    exhaustTemp = exhaustSensor.readCelsius();
    waterTemp = waterSensor.readCelsius();
    digitalWrite(RELAY_PIN, shouldHeat ? HIGH : LOW);
  }
  delay(500);
}
