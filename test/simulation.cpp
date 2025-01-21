#include <iostream>
#include <vector>
#include <cmath>
#include <chrono>
#include <thread>
#include "../lib/PidController/PIDController.h"

// PID parameters
double Kp = 0.07;   // Proportional gain
double Ki = 0.0;   // Integral gain
double Kd = 0.0;   // Derivative gain

void simulate(PIDController pid, int temp) {
    float pidCorrection = pid.calculateControlSignal(185, temp, 2);
    int correction = (int) round(pidCorrection);
    std::cout << "Temp "  << temp << " Correction " << correction << " (" << pidCorrection << ")\n";
}


int main() {
    double setpoint = 185;

    PIDController pidController2(Kp, Ki, Kd);
    //pidController2.calculateControlSignal(setpoint, 140);
    //pidController2.calculateControlSignal(setpoint, 150);
    simulate(pidController2, 148);
    simulate(pidController2, 155);
    simulate(pidController2, 200);
    simulate(pidController2, 195);
    simulate(pidController2, 190);
    simulate(pidController2, 191);
    simulate(pidController2, 191);
    simulate(pidController2, 195);
    simulate(pidController2, 191);
    
}
