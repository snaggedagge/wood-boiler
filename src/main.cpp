#include <Wire.h>
#include <max6675.h>
#include <LittleFS.h>

#include "Display.h"
#include "Timer.h"
#include "AirDamper.h"
#include "PIDController.h"
#include "WebserverConfiguration.h"
#include "BurnLogger.h"
#include "Notifier.h"

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

#define NTP_SERVER "at.pool.ntp.org"           
#define TZ "CET-1CEST,M3.5.0/02,M10.5.0/03"   

// PID parameters
double Kp = 0.040;   // Proportional gain
double Ki = 0.001;   // Integral gain // 0.001 worked nicely but was teeny tiny to slow, especially at the end. 0.0012 too aggressive
double Kd = 0.15;   // Derivative gain // 0.15 worked nicely, lets try a little bit more aggressive
PIDController pidController(Kp, Ki, Kd);

Adafruit_SH1106 oled(-1);
Display display(&oled, OLED_WIDTH, OLED_HEIGHT);
MAX6675 waterSensor(SCK_PIN, CS_PIN, SO_PIN_WATER);
MAX6675 exhaustSensor(SCK_PIN, CS_PIN, SO_PIN_EXHAUST);
AirDamper primaryAirDamper(DIRECTION_PIN, STEP_PIN, STEPPER_SLEEP_PIN, 56);
Timer timer;
WebserverConfiguration webserverConfig("4G-Gateway-21E0", "snaggedagge");

// Push notifications via ntfy. Pick a long, hard-to-guess topic and subscribe to it in the ntfy app.
#define NTFY_TOPIC "wood-boiler-x7k2q9"
#define NO_FIRE_CHECK_MINUTES 15     // Not above NO_FIRE_TEMPERATURE after this long => probably no fire
#define NO_FIRE_TEMPERATURE 50
#define HOT_TEMPERATURE 120          // First time reaching this => notify
#define REFILL_TEMPERATURE HOT_TEMPERATURE-10 // Dropping to this after being hot...
#define REFILL_WINDOW_MINUTES 120    // ...within this long since start => remind to add wood
Notifier notifier(NTFY_TOPIC);
bool exceededNoFireTemperature = false;
bool noFireNotified = false;
bool hotReached = false;
bool hotNotified = false;
bool refillNotified = false;

int wantedTemperature = 195;
bool reachedTemperature = false;
unsigned long reachedTemperatureTime = 0;
bool burnLogged = false;

float readSensor(MAX6675& sensor, int lowerLimit, int higherLimit) {
  yield();
  return sensor.readCelsius();
}

void updateTemperatures() {
    BurnLogger::getStats().exhaustTemperature = readSensor(exhaustSensor, 0, 350);
    BurnLogger::getStats().waterTemperature = readSensor(waterSensor, 0, 130);
}

// A flag is only set when ntfy accepted the message, so a failed send (e.g. WiFi down) is retried on the next call.
void checkNotifications(const Stats& stats, int sinceStartedMinutes) {
  int temp = stats.exhaustTemperature;

  if (temp > NO_FIRE_TEMPERATURE) {
    exceededNoFireTemperature = true;
  }
  if (!exceededNoFireTemperature && !noFireNotified && sinceStartedMinutes >= NO_FIRE_CHECK_MINUTES) {
    noFireNotified = notifier.send("Ingen eld?",
      String("Temperaturen har inte stigit över ") + NO_FIRE_TEMPERATURE + " grader på " + NO_FIRE_CHECK_MINUTES + " minuter. Du fick antagligen inte eld.",
      "warning", "high");
  }

  if (temp >= HOT_TEMPERATURE) {
    hotReached = true;
    if (!hotNotified) {
      hotNotified = notifier.send("Pannan är varm", String("Pannan har nått ") + temp + " grader.", "fire");
    }
  }

  if (hotReached && !refillNotified && temp <= REFILL_TEMPERATURE && sinceStartedMinutes < REFILL_WINDOW_MINUTES) {
    refillNotified = notifier.send("Fyll på ved",
      String("Temperaturen har sjunkit till ") + temp + " grader efter att ha varit över " + HOT_TEMPERATURE + ". Dags att fylla på.",
      "wood", "high");
  }
}

void setup() {
  BurnLogger::getStats() = {0, 0, 0, 100, 250, true, 0, wantedTemperature};

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);
  Wire.begin(SDA_PIN, SCL_PIN);
  display.init();
  display.displayLogo();
  delay(2000);
  updateTemperatures();
  primaryAirDamper.init();
  primaryAirDamper.hardResetPosition();
  primaryAirDamper.moveToStep(40);
  webserverConfig.init();
  LittleFS.begin();
  configTime(TZ, NTP_SERVER);
}

void loop() {
  webserverConfig.handleUpdate();  
  unsigned long millisSinceStart = millis();
  if (timer.hasPassedMillis(500, millisSinceStart))
  {
    int sinceStartedMinutes = millisSinceStart / 1000 / 60;
    Stats& stats = BurnLogger::getStats();
    stats.heating = !(sinceStartedMinutes > 60 && stats.exhaustTemperature < stats.lowerExhaustLimit) && stats.waterTemperature < 100;
    stats.burnTimeMinutes = stats.heating ? sinceStartedMinutes : stats.burnTimeMinutes;
    // If position has been updated by webserver
    if (stats.primaryAirDamperPosition != primaryAirDamper._currentPosition)
    {
      primaryAirDamper.moveToStep(stats.primaryAirDamperPosition);
    }

    if (timer.hasPassed(5, millisSinceStart)) // Only read temp every 5 seconds
    {
      webserverConfig.reconnectIfDisconnected(); // Cant be too frequent, has to give it time to connect
      updateTemperatures();
      digitalWrite(RELAY_PIN, stats.heating ? HIGH : LOW);
      checkNotifications(stats, sinceStartedMinutes);
    }

    // Let temperature get high enough before PID takes over, difficult to use same config in starting phase as in burning phase.
    // In case it is hot when starting, let PID take over immediately. Probably reboot or power outage
    if ((!reachedTemperature && stats.exhaustTemperature > 155) || (!reachedTemperature && stats.exhaustTemperature > 100 && sinceStartedMinutes == 0))
    {
      reachedTemperature = true;
      reachedTemperatureTime = millisSinceStart;
      primaryAirDamper.moveToStep(18);
      timer.hasPassed(180, millisSinceStart); // Reset timer
    }

    if (reachedTemperature && stats.heating && timer.hasPassed(180, millisSinceStart)) // Adjust stepper every 3 minutes
    {
      float pidCorrection = pidController.calculateControlSignal(stats.targetExhaustTemperature, stats.exhaustTemperature, sinceStartedMinutes);
      int correction = (int) round(pidCorrection);
      BurnLogger::addEntry(stats.burnTimeMinutes, stats.exhaustTemperature, pidCorrection);
      primaryAirDamper.moveTo(correction);
    } else if (stats.heating) {
      pidController.updateMeasuredValue(stats.targetExhaustTemperature, stats.exhaustTemperature, sinceStartedMinutes);
    }
    if (!stats.heating && primaryAirDamper._currentPosition > 0)
    {
      primaryAirDamper.moveToStep(0);
      primaryAirDamper.shutdown();
      unsigned long effectiveBurnTimeMinutes = (millisSinceStart - reachedTemperatureTime) / 1000 / 60;
      if (!burnLogged && effectiveBurnTimeMinutes > 120) {// Only log if burn time was more than 2 hours{
        BurnLogger::shutdown(effectiveBurnTimeMinutes);
        reachedTemperatureTime = 0;
        burnLogged = true;
      }
    }
    display.display(&stats);
  }
}