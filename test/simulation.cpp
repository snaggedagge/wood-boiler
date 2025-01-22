#include <iostream>
#include <vector>
#include <cmath>
#include <chrono>
#include <thread>
#include "../lib/PidController/PIDController.h"

// PID parameters
/*
Derivative gain makes the controller react fast to errors, fades out over time
Integral gain make the controller react more the longer the error remains, grows over time
*/
double Kp = 0.018;   // Proportional gain
double Ki = 0.0025;   // Integral gain
double Kd = 0.02;   // Derivative gain
int minutes = 2;

void simulate(PIDController pid, int temp) {
    float pidCorrection = pid.calculateControlSignal(185, temp, minutes);
    int correction = (int) round(pidCorrection);
    std::cout << "Temp "  << temp << " Correction " << correction << " (" << pidCorrection << ")\n";
    minutes += 2;
}


int main() {
    double setpoint = 185;

    PIDController pidController2(Kp, Ki, Kd);
    //pidController2.calculateControlSignal(setpoint, 140);
    //pidController2.calculateControlSignal(setpoint, 150);
    simulate(pidController2, 190 );
    simulate(pidController2, 202 );
    simulate(pidController2, 202 );
    simulate(pidController2, 203 );
    simulate(pidController2, 202 );
    simulate(pidController2, 199 );
    simulate(pidController2, 194 );
/*

    simulate(pidController2, 169);
    simulate(pidController2, 181);
    simulate(pidController2, 193);
    simulate(pidController2, 198);
    simulate(pidController2, 199);
    simulate(pidController2, 199);
    simulate(pidController2, 189);
    simulate(pidController2, 177);
    simulate(pidController2, 174);
    simulate(pidController2, 174);
    simulate(pidController2, 175);
    simulate(pidController2, 180);
    simulate(pidController2, 187);
    simulate(pidController2, 179);
    simulate(pidController2, 179);
    simulate(pidController2, 179);
    simulate(pidController2, 179);
    simulate(pidController2, 179);
*/
}
