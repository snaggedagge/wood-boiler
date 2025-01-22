#ifndef AIR_DAMPER_H
#define AIR_DAMPER_H

#include <stdint.h>
#include "LogManager.h"

class AirDamper {
private:
    uint8_t _direction_pin;
    uint8_t _step_pin;
    uint8_t _sleep_pin;
    int _stepRange;
    LogManager& logManager;

    void makeStep(bool open, int numberOfSteps);
public:
    int _currentPosition;

    AirDamper(uint8_t direction_pin, uint8_t step_pin, uint8_t sleep_pin, int stepRange, LogManager& lm) : _direction_pin(direction_pin), 
    _step_pin(step_pin), _sleep_pin(sleep_pin), _stepRange(stepRange), logManager(lm), _currentPosition(0)  { }

    void init();
    void moveToPercentage(int percentage);
    void moveToStep(int targetStep);
    void moveTo(int relativePosition);
    void open(int steps);
    void close(int steps);

    /** Performs a hard reset of position. Will always close damper fully, to make sure position zero is actually fully closed */
    void hardResetPosition();
};

#endif