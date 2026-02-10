#include <iostream>
#include <cmath>
#define DEBUG 1
#include "../lib/PidController/PIDController.h"

// PID parameters
/*
Derivative gain makes the controller react fast to errors, fades out over time
Integral gain make the controller react more the longer the error remains, grows over time
*/

/*

Was kind of decent, but ki felt to aggressive
double Kp = 0.018;   // Proportional gain
double Ki = 0.0025;   // Integral gain
double Kd = 0.035;   // Derivative gain

*/

double Kp = 0.018;   // Proportional gain
double Ki = 0.001;   // Integral gain // 0.001 worked nicely but was teeny tiny to slow, especially at the end. 0.0012 too aggressive
double Kd = 0.20;   // Derivative gain // 0.15 worked nicely, lets try a little bit more aggressive
int minutes = 52;
PIDController pidController2(Kp, Ki, Kd);

void simulate(PIDController& pid, int temp) {
    float pidCorrection = pid.calculateControlSignal(185, temp, minutes);
    int correction = (int) round(pidCorrection);

    minutes += 3;
}


int main() {
    double setpoint = 185;

    simulate(pidController2, 155);
    simulate(pidController2, 151);
    simulate(pidController2, 142);
    simulate(pidController2, 140);
    simulate(pidController2, 148);
    simulate(pidController2, 156);
    simulate(pidController2, 158);
    simulate(pidController2, 163);
    simulate(pidController2, 171);
    simulate(pidController2, 176);
    simulate(pidController2, 181);
    simulate(pidController2, 179);
    simulate(pidController2, 177);
    simulate(pidController2, 176);
    simulate(pidController2, 177);
    simulate(pidController2, 176);
    simulate(pidController2, 183);
    simulate(pidController2, 194);
    simulate(pidController2, 200);
    simulate(pidController2, 202);
    simulate(pidController2, 204);
    simulate(pidController2, 201);
    simulate(pidController2, 215);
    simulate(pidController2, 226);
    simulate(pidController2, 223);
    simulate(pidController2, 219);
    simulate(pidController2, 217);
    simulate(pidController2, 216);
    simulate(pidController2, 212);
    simulate(pidController2, 209);
    simulate(pidController2, 207);
    simulate(pidController2, 200);
    simulate(pidController2, 190);
    simulate(pidController2, 183);
    simulate(pidController2, 178);
    simulate(pidController2, 179);
    simulate(pidController2, 181);
    return 0;
}
