#pragma once

#include "Vehicle.h"
#include <torch/script.h> // Include TorchScript
#include <torch/torch.h>

class TractionControl {
public:
    TractionControl(double desiredSlip, const std::string& modelPath);

    void update(Vehicle& vehicle, double dt);

private:
    double desiredSlip;
    double maxBrakeTorque;
    double maxDriveTorque;
    double brakeRampRate;
    double driveRampRate;

    torch::jit::Module model;
    bool modelLoaded = false;
    c10::Device device;
};
