#pragma once

#include "Vehicle.h"

class TractionControl {
public:
    explicit TractionControl(double desiredSlip = 0.1);

    // Called each physics step (or substep) to adjust drive/brake torque.
    void update(Vehicle& vehicle, double dt);

private:
    double desiredSlip;

    // We'll store some internal parameters for ramping
    double maxBrakeTorque;
    double maxDriveTorque;
    double brakeRampRate; 
    double driveRampRate; 
};
