#ifndef TIMER_H
#define TIMER_H

#include <map>

class Timer {
public:
    bool hasPassed(int s, unsigned long millisSinceStart);
    bool hasPassedMillis(unsigned long ms, unsigned long millisSinceStart);
private:
    std::map<unsigned long, unsigned long> timers;
};

#endif