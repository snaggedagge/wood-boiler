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
        // TODO: dT does not work properly since it is currently always uses the accumulated minutes since system startup.
        // Does not matter since integral and derivative parts are disabled, but in case we want to take them into proper use, 
        // reset minutes every time system is in target interval
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