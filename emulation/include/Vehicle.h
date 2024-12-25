#pragma once

#define _USE_MATH_DEFINES
#include <vector>
#include <cmath>

class Vehicle {
public:
    struct Wheel {
        double angularVelocity;  // rad/s (rotation speed)
        double brakeTorque;      // N·m (applied by brake)
        double driveTorque;      // N·m (applied by engine)
        double rotationAngle;    // for rendering (accumulated rotation in radians)
    };

    Vehicle(double initialSpeed, int numWheels);

    void update(double dt);

    // Accessors
    double getLinearSpeed() const { return linearSpeed; }
    const std::vector<Wheel>& getWheels() const { return wheels; }

    // Set torque for traction/braking
    void setBrakeTorque(int wheelIndex, double torque);
    void setDriveTorque(int wheelIndex, double torque);
    void setFriction(double friction);

    // Slip ratio for a single wheel
    double computeSlipRatio(int wheelIndex) const;

    double wheelRadius;   // wheel radius (meters)
    double mass;          // total vehicle mass (kg)
    double wheelInertia;  // moment of inertia per wheel (kg·m^2)
    double muPeak;        // maximum friction coefficient
    double slipOpt;       // slip ratio near which friction peaks

private:
    double linearSpeed;     // m/s, forward speed of the vehicle
    std::vector<Wheel> wheels;
};
