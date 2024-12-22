#include "Vehicle.h"
#include <algorithm>
#include <cmath>

Vehicle::Vehicle(double initialSpeed, int numWheels)
    : linearSpeed(initialSpeed)
{
    wheels.resize(numWheels);
    for (auto& w : wheels) {

        double initialOmega = (wheelRadius > 1e-5) 
                                ? (initialSpeed / wheelRadius)
                                : 0.0;
        w.angularVelocity = initialOmega;
        w.brakeTorque     = 0.0;
        w.driveTorque     = 0.0;
        w.rotationAngle   = 0.0;
    }

    wheelRadius  = 0.3;   // 30 cm
    mass         = 1200;  // 1200 kg
    wheelInertia = 1.0;   // 1 kg·m^2 (rough guess)
    muPeak       = 1.0;   // friction coefficient for good tires on dry asphalt
    slipOpt      = 0.1;   // ~10% slip is often near peak traction
}

void Vehicle::update(double dt)
{
    if (dt <= 0.0) return;

    // Sum friction forces from each wheel => netForce => update linearSpeed
    double totalForce = 0.0;

    for (int i = 0; i < (int)wheels.size(); i++) {
        double slip = computeSlipRatio(i);

        // "Exponential" friction model that rises with slip, up to muPeak
        double absSlip = std::fabs(slip);
        double k = 10.0; // shape factor
        double mu = muPeak * (1.0 - std::exp(-k * absSlip));

        // We'll assume equal weight distribution
        double normalForce = (mass * 9.81) / wheels.size();
        double frictionForce = mu * normalForce;

        // Direction: if wheel is going faster than vehicle => friction forward
        // if wheel is going slower => friction backward
        // sign depends on (wheel speed - vehicle speed)
        double wheelLinSpeed = wheels[i].angularVelocity * wheelRadius;
        double diff = wheelLinSpeed - linearSpeed;
        double sign = (diff >= 0.0) ? 1.0 : -1.0;

        frictionForce *= sign;
        totalForce    += frictionForce;
    }

    // Update linear speed
    double accel = totalForce / mass;
    linearSpeed += accel * dt;
    if (linearSpeed < 0.0) {
        linearSpeed = 0.0; // no reversing in this demo
    }

    // Now update each wheel's angular velocity from net torque
    for (auto& w : wheels) {
        double wheelLinSpeed = w.angularVelocity * wheelRadius;
        double diff          = wheelLinSpeed - linearSpeed;

        double absSlip = std::fabs(diff / std::max(linearSpeed, 0.001));
        double k = 10.0;
        double mu = muPeak * (1.0 - std::exp(-k * absSlip));
        double normalForce = (mass * 9.81) / wheels.size();
        double frictionForce = mu * normalForce;

        double sign = (wheelLinSpeed >= linearSpeed) ? 1.0 : -1.0;
        double frictionTorque = frictionForce * wheelRadius * sign;

        double netTorque = w.driveTorque - w.brakeTorque - frictionTorque;
        double alpha     = netTorque / wheelInertia; // T = I·alpha

        w.angularVelocity += alpha * dt;
        if (w.angularVelocity < 0.0) {
            w.angularVelocity = 0.0;
        }

        // Update rotation angle for rendering
        w.rotationAngle += w.angularVelocity * dt;
        // Keep in [0, 2π) if you want to wrap
        if (w.rotationAngle > 2.0 * M_PI) {
            w.rotationAngle = std::fmod(w.rotationAngle, 2.0 * M_PI);
        }
    }
}

void Vehicle::setBrakeTorque(int wheelIndex, double torque)
{
    if (wheelIndex >= 0 && wheelIndex < (int)wheels.size()) {
        wheels[wheelIndex].brakeTorque = std::max(0.0, torque);
    }
}

void Vehicle::setDriveTorque(int wheelIndex, double torque)
{
    if (wheelIndex >= 0 && wheelIndex < (int)wheels.size()) {
        wheels[wheelIndex].driveTorque = std::max(0.0, torque);
    }
}

double Vehicle::computeSlipRatio(int wheelIndex) const
{
    if (wheelIndex < 0 || wheelIndex >= (int)wheels.size()) return 0.0;

    double wheelLinSpeed = wheels[wheelIndex].angularVelocity * wheelRadius;
    double denom = std::max(linearSpeed, 0.001);
    double slip  = (wheelLinSpeed - linearSpeed) / denom;
    return slip;
}
