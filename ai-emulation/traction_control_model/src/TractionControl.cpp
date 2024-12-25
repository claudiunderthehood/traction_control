#include "TractionControl.h"
#include <algorithm>
#include <cmath>
#include <iostream>

TractionControl::TractionControl(double desiredSlip_, const std::string& modelPath)
    : desiredSlip(desiredSlip_),
      device(torch::cuda::is_available() ? torch::kCUDA : torch::kCPU)
{
    maxBrakeTorque = 200.0;   // N·m
    maxDriveTorque = 150.0;   // N·m
    brakeRampRate  = 500.0;   // N·m per second
    driveRampRate  = 300.0;   // N·m per second

    if (!modelPath.empty()) {
        try {
            model = torch::jit::load(modelPath);
            model.to(device);
            modelLoaded = true;

            if (device == torch::kCUDA) {
                std::cout << "CUDA is available. Using GPU." << std::endl;
            } else {
                std::cout << "CUDA is not available. Using CPU." << std::endl;
            }

            std::cout << "Model loaded successfully from: " << modelPath << std::endl;
        } catch (const c10::Error& e) {
            std::cerr << "Error loading model: " << e.what() << std::endl;
        }
    }
}

void TractionControl::update(Vehicle& vehicle, double dt)
{
    const auto& wheels = vehicle.getWheels();
    int n = static_cast<int>(wheels.size());

    for (int i = 0; i < n; i++) {
        double slip = vehicle.computeSlipRatio(i);
        auto& w = wheels[i];

        if (modelLoaded) {
            try {
                // Prepare input tensor
                auto input = torch::tensor(
                    {slip, w.angularVelocity, vehicle.getLinearSpeed(),
                     w.brakeTorque, w.driveTorque, 0.0, 0.0, 0.0},
                    torch::kFloat
                ).to(device).unsqueeze(0);

                std::cout << "Input Tensor (Wheel " << i << "): " << input << std::endl;

                auto output = model.forward({input});
                
                if (output.isTuple()) {
                    auto tupleOutput = output.toTuple();
                    double predictedDriveTorque = tupleOutput->elements()[0].toTensor().item<double>();
                    double predictedBrakeTorque = tupleOutput->elements()[1].toTensor().item<double>();

                    // Apply predicted torques
                    vehicle.setBrakeTorque(i, std::clamp(predictedBrakeTorque, 0.0, maxBrakeTorque));
                    vehicle.setDriveTorque(i, std::clamp(predictedDriveTorque, 0.0, maxDriveTorque));
                } else if (output.isTensor()) {
                    auto tensorOutput = output.toTensor();
                    if (tensorOutput.size(1) >= 2) { // Ensure at least two values in output tensor
                        double predictedDriveTorque = tensorOutput[0][0].item<double>();
                        double predictedBrakeTorque = tensorOutput[0][1].item<double>();

                        // Apply predicted torques
                        vehicle.setBrakeTorque(i, std::clamp(predictedBrakeTorque, 0.0, maxBrakeTorque));
                        vehicle.setDriveTorque(i, std::clamp(predictedDriveTorque, 0.0, maxDriveTorque));
                    } else {
                        std::cerr << "Unexpected tensor shape in model output." << std::endl;
                    }
                } else {
                    std::cerr << "Unexpected model output type." << std::endl;
                }
            } catch (const c10::Error& e) {
                std::cerr << "Model inference error: " << e.what() << std::endl;
            }
        } else {
            // Fallback: Default behavior
            double slipError = slip - desiredSlip;

            double currentBrake = w.brakeTorque;
            double currentDrive = w.driveTorque;

            if (slipError > 0.0) {
                double inc = brakeRampRate * slipError * dt;  
                double dec = driveRampRate * slipError * dt;  

                double newBrake = std::min(maxBrakeTorque, currentBrake + inc);
                double newDrive = std::max(0.0, currentDrive - dec);

                vehicle.setBrakeTorque(i, newBrake);
                vehicle.setDriveTorque(i, newDrive);
            } else {
                double slipMag = -slipError;
                double dec = brakeRampRate * slipMag * dt;
                double inc = driveRampRate * slipMag * dt;

                double newBrake = std::max(0.0, currentBrake - dec);
                double newDrive = std::min(maxDriveTorque, currentDrive + inc);

                vehicle.setBrakeTorque(i, newBrake);
                vehicle.setDriveTorque(i, newDrive);
            }
        }
    }
}