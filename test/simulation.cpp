#include <iostream>
#include <vector>
#include <cmath>
#include <chrono>
#include <thread>
#include "../lib/PidController/PIDController.h"

// PID parameters
double Kp = 0.10;   // Proportional gain
double Ki = 0.02;   // Integral gain
double Kd = 0.0;   // Derivative gain


void simulate(PIDController pid, int temp) {
    float pidCorrection = pid.calculateControlSignal(185, temp);
    int correction = (int) round(pidCorrection);
    std::cout << "Temp "  << temp << " Correction " << correction << " (" << pidCorrection << ")\n";
}


int main() {
    double setpoint = 185;

    PIDController pidController2(Kp, Ki, Kd);
    //pidController2.calculateControlSignal(setpoint, 140);
    //pidController2.calculateControlSignal(setpoint, 150);
    simulate(pidController2, 160);
    simulate(pidController2, 200);
    simulate(pidController2, 195);
    simulate(pidController2, 190);
    simulate(pidController2, 185);
    simulate(pidController2, 185);
    simulate(pidController2, 185);
    simulate(pidController2, 180);
}
