#ifndef STATS_H
#define STATS_H

typedef struct {
    int burnTimeMinutes;
    int exhaustTemperature;
    int waterTemperature;
    int lowerExhaustLimit;
    int upperExhaustLimit;
    bool heating;
    int primaryAirDamperPosition;
} Stats;

#endif