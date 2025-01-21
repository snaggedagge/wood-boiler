#include <Wire.h>
#include <max6675.h>


#include "Display.h"
#include "Timer.h"
#include "AirDamper.h"
#include "PIDController.h"
#include "WebserverConfiguration.h"
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

Stats stats = {0, 0, 0, 100, 250, true, 0};

// PID parameters
double Kp = 0.07;   // Proportional gain
double Ki = 0.0;   // Integral gain
double Kd = 0.0;   // Derivative gain
PIDController pidController(Kp, Ki, Kd);
LogManager logManager;

Adafruit_SH1106 oled(-1);
Display display(&oled, OLED_WIDTH, OLED_HEIGHT);
MAX6675 waterSensor(SCK_PIN, CS_PIN, SO_PIN_WATER);
MAX6675 exhaustSensor(SCK_PIN, CS_PIN, SO_PIN_EXHAUST);
AirDamper primaryAirDamper(DIRECTION_PIN, STEP_PIN, STEPPER_SLEEP_PIN, 90, logManager);
Timer timer;
WebserverConfiguration config("4G-Gateway-21E0", "snaggedagge", logManager, stats);

int wantedTemperature = 185;
bool reachedTemperature = false;

void updateTemperatures() {
    int exhaustTemperature = exhaustSensor.readCelsius();
    int waterTemperature = waterSensor.readCelsius();
    if (exhaustTemperature < 350 && exhaustTemperature > 0)
    {
      stats.exhaustTemperature = exhaustTemperature;
    }
    if (waterTemperature < 130 && waterTemperature > 0)
    {
      stats.waterTemperature = waterTemperature;
    }
}

void setup() {
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);
  Wire.begin(SDA_PIN, SCL_PIN);
  display.init();
  display.displayLogo();
  delay(3000);
  updateTemperatures();
  primaryAirDamper.init();
  primaryAirDamper.hardResetPosition();
  primaryAirDamper.open(20);
  config.init();
}

void loop() {
  config.handleUpdate();  
  unsigned long millisSinceStart = millis();
  int sinceStartedMinutes = millisSinceStart / 1000 / 60;
  stats.heating = !(sinceStartedMinutes > 60 && stats.exhaustTemperature < stats.lowerExhaustLimit) && stats.waterTemperature < 100;
  stats.burnTimeMinutes = stats.heating ? sinceStartedMinutes : stats.burnTimeMinutes;

  // Let temperature get high enough before PID takes over, difficult to use same config in starting phase as in burning phase.
  // using different gains could also work
  if (!reachedTemperature && stats.exhaustTemperature > 160)
  {
    reachedTemperature = true;
    primaryAirDamper.moveToPercentage(13);
    timer.hasPassed(120, millisSinceStart); // Reset timer
  }

  if (timer.hasPassed(5, millisSinceStart)) // Only read temp every 5 seconds
  {
    updateTemperatures();
    stats.primaryAirDamperPosition = primaryAirDamper._currentPosition;
    digitalWrite(RELAY_PIN, stats.heating ? HIGH : LOW);
  }
  display.display(stats);

  if (!stats.heating && primaryAirDamper._currentPosition > 0)
  {
    logManager.addLog(F("Turning off heating, closing damper"));
    primaryAirDamper.moveToPercentage(0);
  }

  if (reachedTemperature && stats.heating && timer.hasPassed(120, millisSinceStart)) // Adjust stepper every 2 minutes
  {
    float pidCorrection = pidController.calculateControlSignal(wantedTemperature, stats.exhaustTemperature, sinceStartedMinutes);
    int correction = (int) round(pidCorrection);
    logManager.addLog("PID " + String(correction) + "(" + String(pidCorrection) + ") " + String(stats.exhaustTemperature));
    primaryAirDamper.moveTo(correction);
  }
  delay(500);
}