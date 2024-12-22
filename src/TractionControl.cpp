#include "TractionControl.h"
#include <algorithm>
#include <cmath>

TractionControl::TractionControl(double desiredSlip_)
    : desiredSlip(desiredSlip_)
{
    maxBrakeTorque = 200.0;   // N·m
    maxDriveTorque = 150.0;   // N·m
    brakeRampRate  = 500.0;   // N·m per second
    driveRampRate  = 300.0;   // N·m per second
}

void TractionControl::update(Vehicle& vehicle, double dt)
{
    const auto& wheels = vehicle.getWheels();
    int n = (int)wheels.size();

    for (int i = 0; i < n; i++) {
        double slip = vehicle.computeSlipRatio(i);
        auto& w     = wheels[i]; // read‐only reference for current torque

        // We'll do a P-like control:
        // slipError = slip - desiredSlip
        // If slip > desired => add brake or reduce drive
        // If slip < desired => reduce brake, add drive
        double slipError = slip - desiredSlip;

        // Current torque
        double currentBrake = w.brakeTorque;
        double currentDrive = w.driveTorque;

        if (slipError > 0.0) {
            // Too much slip => ramp up brake, ramp down drive
            double inc = brakeRampRate * slipError * dt;  
            double dec = driveRampRate * slipError * dt;  

            double newBrake = std::min(maxBrakeTorque, currentBrake + inc);
            double newDrive = std::max(0.0, currentDrive - dec);

            vehicle.setBrakeTorque(i, newBrake);
            vehicle.setDriveTorque(i, newDrive);
        }
        else {
            // slip <= desired => reduce brake, ramp up drive
            double slipMag = -slipError; // how far below desired
            double dec = brakeRampRate * slipMag * dt;
            double inc = driveRampRate  * slipMag * dt;

            double newBrake = std::max(0.0, currentBrake - dec);
            double newDrive = std::min(maxDriveTorque, currentDrive + inc);

            vehicle.setBrakeTorque(i, newBrake);
            vehicle.setDriveTorque(i, newDrive);
        }
    }
}
