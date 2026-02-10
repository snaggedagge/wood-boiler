#include "BurnLogger.h"

std::vector<PidCorrection> BurnLogger::entries;
Stats BurnLogger::stats;

void BurnLogger::addEntry(int burnTimeMinutes, int exhaustTemperature, float correction)
{
    PidCorrection entry;
    entry.burnTimeMinutes = burnTimeMinutes;
    entry.exhaustTemperature = exhaustTemperature;
    entry.correction = correction;

    entries.push_back(entry);
}

const std::vector<PidCorrection>& BurnLogger::getEntries()
{
    return entries;
}

Stats& BurnLogger::getStats()
{
    return stats;
}
