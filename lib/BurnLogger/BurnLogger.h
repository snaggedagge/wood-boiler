#ifndef BURN_LOG_H
#define BURN_LOG_H

#include <Arduino.h>
#include <vector>

struct Stats {
    int burnTimeMinutes;
    int exhaustTemperature;
    int waterTemperature;
    int lowerExhaustLimit;
    int upperExhaustLimit;
    bool heating;
    int primaryAirDamperPosition;
    int targetExhaustTemperature;
};

class BurnLogger
{
public:
    static void addEntry(int burnTimeMinutes, int exhaustTemperature, float correction);
    static void shutdown(unsigned long effectiveBurnTimeMinutes);
    static Stats& getStats();
    static const char* getLogFilename();
    static const char* getInfoFilename();
    static const char* getStatisticsFilename();
private:
    static Stats stats; 
};

#endif