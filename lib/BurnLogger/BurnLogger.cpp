#include <LittleFS.h>
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

const char* BurnLogger::getLogFilename()
{
    return "/logs.json";
}

Stats& BurnLogger::getStats()
{
    return stats;
}
