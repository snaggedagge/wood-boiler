#ifndef PID_CONTROLLER_H
#define PID_CONTROLLER_H

class PIDController {
private:
    double Kp, Ki, Kd;
    double previousError, integral;
    int lastMinuteAtTarget;

public:
    PIDController(double p, double i, double d) : Kp(p), Ki(i), Kd(d), previousError(0), integral(0), lastMinuteAtTarget(0) {}
    
    double calculateControlSignal(double desiredValue, double measuredValue, int currentMinute);
    void updateMeasuredValue(double desiredValue, double measuredValue, int currentMinute);
    void setGains(double Kp, double Ki, double Kd);
};

#endif