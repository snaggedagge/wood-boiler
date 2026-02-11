
#include "AirDamper.h"
#include <Arduino.h>

void AirDamper::init() {
  pinMode(_step_pin, OUTPUT); 
  pinMode(_direction_pin, OUTPUT);
  pinMode(_sleep_pin, OUTPUT);
  digitalWrite(_sleep_pin, LOW);
}

void AirDamper::shutdown() {
  digitalWrite(_sleep_pin, HIGH);
}

void AirDamper::hardResetPosition() {
  _currentPosition = _stepRange;
  makeStep(false, _stepRange);
  _currentPosition = 0;
}

void AirDamper::moveToStep(int targetStep) {
  moveTo(targetStep - _currentPosition);
}

void AirDamper::moveToPercentage(int percentage) {
  float targetStep = percentage == 0 ? 0 : ((float) percentage / 100) * _stepRange;
  moveTo(targetStep - _currentPosition);
}

void AirDamper::moveTo(int relativePosition) {
  int targetStep = _currentPosition + relativePosition;
  if (targetStep < _currentPosition)
  {
    close(_currentPosition - targetStep);
  }
  else if (targetStep > _currentPosition)
  {
    open(targetStep - _currentPosition);
  }
}

void AirDamper::close(int steps) {
  makeStep(false, steps);
}

void AirDamper::open(int steps) {
  makeStep(true, steps);
}

void AirDamper::makeStep(bool open, int numberOfSteps) {
  int newPosition = open ? _currentPosition + numberOfSteps : _currentPosition - numberOfSteps;
  int constrainedPosition = constrain(newPosition, 0, _stepRange);
  if (constrainedPosition != newPosition) {
    moveToStep(constrainedPosition);
    return;
  }

  _currentPosition = newPosition;
  digitalWrite(_direction_pin, open ? LOW : HIGH); //Changes the rotation direction
  //digitalWrite(_sleep_pin, LOW);

  delay(20);
  for(int x = 0; x < numberOfSteps; x++) {
    digitalWrite(_step_pin, HIGH);
    delay(30);
    digitalWrite(_step_pin, LOW);
    delay(30);
  }
  //delay(500);
  //digitalWrite(_sleep_pin, HIGH);
}
