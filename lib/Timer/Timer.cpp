
#include "Timer.h"

bool Timer::hasPassed(int s, unsigned long millisSinceStart) {
  return hasPassedMillis(s * 1000, millisSinceStart);
}

bool Timer::hasPassedMillis(unsigned long ms, unsigned long millisSinceStart) {
  if ((timers[ms] + ms) >= millisSinceStart)
  {
    return false;
  }
  timers[ms] = millisSinceStart;
  return true;
}