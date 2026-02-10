
#include <stdint.h>
#include <cmath>
#include "PIDController.h"

double PIDController::calculateControlSignal(double desiredValue, double measuredValue, int currentMinute) { 
    updateMeasuredValue(desiredValue, measuredValue, currentMinute);
    double dt = currentMinute - this->lastMinuteAtTarget;
    if (dt < 1)
    {
        dt = 2;
    }

    double error = desiredValue - measuredValue;
    //this->integral += error * dt; 
    double derivative = (error - this->previousError) / dt;
    double scalingFactor = 1.0 / (1.0 + std::fabs(derivative)); // Scales down integral gain if derivative is large
    // Update the integral term with scaling
    this->integral += error * dt * scalingFactor;
    double output = (Kp * error) + (Ki * this->integral * scalingFactor) + (Kd * derivative);
    this->previousError = error;

    #ifdef DEBUG
        std::cout << "Temp "  << measuredValue << " Correction " << round(output) << " (" << output << ")" << " Derivative " << derivative 
        << " kd " << (Kd * derivative) << " ki" << (Ki * this->integral * scalingFactor) << " kp " << (Kp * error) << " "<< scalingFactor<< "\n";
    #endif

    return output;
}

void PIDController::updateMeasuredValue(double desiredValue, double measuredValue, int currentMinute) {
    if (abs(desiredValue - measuredValue) <= 5 || this->lastMinuteAtTarget == 0)
    {
        this->lastMinuteAtTarget = currentMinute;
        this->integral = 0;
    }
}

void PIDController::setGains(double Kp, double Ki, double Kd) {
    this->Kp = Kp;
    this->Ki = Ki;
    this->Kd = Kd;
}