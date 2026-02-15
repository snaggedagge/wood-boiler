#include <LittleFS.h>
#include <ArduinoJson.h>
#include <time.h>
#include "BurnLogger.h"

Stats BurnLogger::stats;
bool firstLog = true;

void BurnLogger::addEntry(int burnTimeMinutes, int exhaustTemperature, float correction)
{
    fs::File logs = LittleFS.open(getLogFilename(), firstLog ? "w" : "r+");
    if (logs.size() > 0)
        logs.seek(1, SeekEnd);
    logs.printf("%c{\"correction\":%.2f,\"exhaustTemperature\":%d,\"burnTimeMinutes\":%d}]",
        firstLog ? '[' : ',', correction, exhaustTemperature, burnTimeMinutes);
    logs.close();
    firstLog = false;
}

void BurnLogger::shutdown(unsigned long effectiveBurnTimeMinutes)
{
    fs::File info = LittleFS.open(getInfoFilename(), LittleFS.exists(getInfoFilename()) ? "r+" : "w");
    JsonDocument doc;
    if (info.size() > 0)
    {
          deserializeJson(doc, info);
    }
    else {
        time_t now;
        time(&now);
        doc["completeBurnTimeMinutes"] = 0;
        doc["since"] = now;
    }
    doc["completeBurnTimeMinutes"] = effectiveBurnTimeMinutes + doc["completeBurnTimeMinutes"].as<unsigned long>();
    info.seek(0, SeekSet);
    serializeJson(doc, info);
    info.close();
}

const char* BurnLogger::getLogFilename()
{
    return "/logs.json";
}

const char* BurnLogger::getInfoFilename()
{
    return "/info.json";
}

Stats& BurnLogger::getStats()
{
    return stats;
}