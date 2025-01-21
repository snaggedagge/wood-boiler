#ifndef PID_CONTROLLER_H
#define PID_CONTROLLER_H

#include <stdint.h>

class PIDController {
private:
    double Kp, Ki, Kd;
    double previousError, integral;

public:
    PIDController(double p, double i, double d) : Kp(p), Ki(i), Kd(d), previousError(0), integral(0) {}
    
    double calculateControlSignal(double desiredValue, double measuredValue, double dt) {
        double error = desiredValue - measuredValue;
        integral += error * dt; 
        double derivative = (error - previousError) / dt;
        double output = (Kp * error) + (Ki * integral) + (Kd * derivative);
        previousError = error;
        return output;
    }

    void setGains(double Kp, double Ki, double Kd) {
        this->Kp = Kp;
        this->Ki = Ki;
        this->Kd = Kd;
    }
};

#endif