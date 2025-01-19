#include <Wire.h>
#include <max6675.h>


#include "Display.h"
#include "Timer.h"
#include "AirDamper.h"
#include "PIDController.h"
#include "Configuration.h"
#include "LogManager.h"

#define OLED_WIDTH 128
#define OLED_HEIGHT 64

// Built in led (Uses Adafruit_SSD1306) ESP8266 OLED
//#define SDA_PIN 14
//#define SCL_PIN 12

#define SDA_PIN 4
#define SCL_PIN 0
#define RELAY_PIN 2

#define SO_PIN_WATER 16 // D0
#define SO_PIN_EXHAUST 5 // D1
#define SCK_PIN 15 // D8
#define CS_PIN 13 // D7


#define DIRECTION_PIN 3 // RX
#define STEP_PIN 1 // TX
#define STEPPER_SLEEP_PIN 14 // D5

LogManager logManager;

Adafruit_SH1106 oled(-1);
Display display(&oled, OLED_WIDTH, OLED_HEIGHT);
MAX6675 waterSensor(SCK_PIN, CS_PIN, SO_PIN_WATER);
MAX6675 exhaustSensor(SCK_PIN, CS_PIN, SO_PIN_EXHAUST);
AirDamper primaryAirDamper(DIRECTION_PIN, STEP_PIN, STEPPER_SLEEP_PIN, 90, logManager);

int exhaustTemp;
int waterTemp;
int upperLimit = 250;
int lowerLimit = 100;
int wantedTemperature = 185;
int lastOpenPosition = 0;

Timer timer;

// PID parameters
double Kp = 0.10;   // Proportional gain
double Ki = 0.02;   // Integral gain
double Kd = 0.0;   // Derivative gain
PIDController pidController(Kp, Ki, Kd);

const char* ssid = "4G-Gateway-21E0";
const char* password = "snaggedagge";

Configuration config(ssid, password, logManager);


void setup() {
  pinMode(RELAY_PIN, OUTPUT);
  Wire.begin(SDA_PIN, SCL_PIN);
  display.init();
  display.displayLogo();
  delay(3000);
  digitalWrite(RELAY_PIN, LOW);
  exhaustTemp = exhaustSensor.readCelsius();
  waterTemp = waterSensor.readCelsius();


  logManager.addLog("Booting");
  
  config.connectToWiFi();
  config.setUpOverTheAirProgramming();

  primaryAirDamper.init();
  // MF sounds horrific when closing it fully without knowing exakt position for sure. 
  // Either construct a softer stopper, or dont include in code...
  //primaryAirDamper.hardResetPosition();
  primaryAirDamper.open(25);
}

void loop() {
  config.handleUpdate();  
  unsigned long millisSinceStart = millis();
  int sinceStartedMinutes = millisSinceStart / 1000 / 60;
  boolean shouldHeat = !(sinceStartedMinutes > 60 && exhaustTemp < lowerLimit);
  if (waterTemp >= 100)
  {
    shouldHeat = false;
  }

  display.display(exhaustTemp, waterTemp, lowerLimit, upperLimit, shouldHeat, sinceStartedMinutes);
  if (timer.hasPassed(5, millisSinceStart)) // Only read temp every 5 seconds
  {
    exhaustTemp = exhaustSensor.readCelsius();
    waterTemp = waterSensor.readCelsius();
    digitalWrite(RELAY_PIN, shouldHeat ? HIGH : LOW);
  }

  if (!shouldHeat && primaryAirDamper._currentPosition > 0)
  {
    logManager.addLog("Turning off heating, closing damper");
    lastOpenPosition = primaryAirDamper._currentPosition;
    primaryAirDamper.moveToPercentage(0);
  }
  else if(shouldHeat && primaryAirDamper._currentPosition == 0) {
    // As a safety mechanism to begin with. This could happen if temperature sensor gives faulty readings.
    // Maybe add failsafe to readings?
    logManager.addLog("Reopening damper, should heat");
    primaryAirDamper.open(lastOpenPosition);
  }

  if (exhaustTemp > 140 && shouldHeat && timer.hasPassed(300, millisSinceStart)) // Adjust stepper every five minutes
  {
    float pidCorrection = pidController.calculateControlSignal(wantedTemperature, exhaustTemp);
    int correction = (int) round(pidCorrection);
    logManager.addLog("PID Correction: " + String(correction) + " (" + String(pidCorrection) + ") for temperature " + String(exhaustTemp));
    primaryAirDamper.moveTo(correction);
  }
  delay(500);
}