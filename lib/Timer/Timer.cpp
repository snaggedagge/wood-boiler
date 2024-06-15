
#include "Timer.h"

bool Timer::hasPassed(int seconds, unsigned long millisSinceStart) {
  if (timers[seconds] == millisSinceStart || (timers[seconds] + (seconds * 1000)) > millisSinceStart)
  {
    return false;
  }
  timers[seconds] = millisSinceStart;
  return true;
}