#include <iostream>
#include <cmath>

#include "../lib/PidController/PIDController.h"
#define DEBUG 1
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

double Kp = 0.06;   // Proportional gain
double Ki = 0.001;   // Integral gain // 0.001 worked nicely but was teeny tiny to slow, especially at the end. 0.0012 too aggressive
double Kd = 0.15;   // Derivative gain // 0.15 worked nicely, lets try a little bit more aggressive
int minutes = 32;
PIDController pidController2(Kp, Ki, Kd);

void simulate(PIDController& pid, int temp) {


    float pidCorrection = pid.calculateControlSignal(195, temp, minutes);
    int correction = (int) round(pidCorrection);
            minutes += 3;
}



int main() {
    std::cout << "Hello Geek\n";
    pidController2.updateMeasuredValue(195, 130, 2);
    simulate(pidController2, 159);
    simulate(pidController2, 158);
    simulate(pidController2, 148);
    simulate(pidController2, 198);
    simulate(pidController2, 199);
    simulate(pidController2, 184);
    simulate(pidController2, 174);
    simulate(pidController2, 162);
    simulate(pidController2, 150);
    simulate(pidController2, 141);
    return 0;
}
