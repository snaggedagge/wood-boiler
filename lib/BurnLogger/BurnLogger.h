#ifndef BURN_LOG_H
#define BURN_LOG_H

#include <Arduino.h>
#include <vector>
struct PidCorrection {
    int burnTimeMinutes;
    int exhaustTemperature;
    float correction;
};

struct Stats {
    int burnTimeMinutes;
    int exhaustTemperature;
    int waterTemperature;
    int lowerExhaustLimit;
    int upperExhaustLimit;
    bool heating;
    int primaryAirDamperPosition;
};

class BurnLogger
{
public:
    static void addEntry(int burnTimeMinutes, int exhaustTemperature, float correction);

    static const std::vector<PidCorrection>& getEntries();
    static Stats& getStats();

private:
    static std::vector<PidCorrection> entries;
    static Stats stats; 
};

#endif