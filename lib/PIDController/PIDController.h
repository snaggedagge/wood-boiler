#ifndef PID_CONTROLLER_H
#define PID_CONTROLLER_H

#include <stdint.h>

class PIDController {
private:
    double Kp, Ki, Kd;
    double previousError, integral;
    int lastMinuteAtTarget;

public:
    PIDController(double p, double i, double d) : Kp(p), Ki(i), Kd(d), previousError(0), integral(0), lastMinuteAtTarget(0) {}
    
    double calculateControlSignal(double desiredValue, double measuredValue, int currentMinute) { 
        if (abs(desiredValue - measuredValue) <= 5)
        {
            lastMinuteAtTarget = currentMinute;
        }
        double dt = currentMinute - lastMinuteAtTarget;
        if (dt < 1)
        {
            dt = 2;
        }

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