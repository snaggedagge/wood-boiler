#ifndef TIMER_H
#define TIMER_H

#include <map>

class Timer {
public:
    bool hasPassed(int seconds, unsigned long millisSinceStart);
private:
    std::map<int, unsigned long> timers;
};

#endif