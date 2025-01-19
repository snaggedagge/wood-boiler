#ifndef PID_CONTROLLER_H
#define PID_CONTROLLER_H

#include <stdint.h>

class PIDController {
private:
    double Kp, Ki, Kd;
    double previousError, integral;

public:
    PIDController(double p, double i, double d) : Kp(p), Ki(i), Kd(d), previousError(0), integral(0) {}
    
    double calculateControlSignal(double desiredValue, double measuredValue) {
        double error = desiredValue - measuredValue;
        integral += error;
        double derivative = error - previousError;
        double output = Kp * error + Ki * integral + Kd * derivative;
        previousError = error;
        return output;
    }
};

#endif