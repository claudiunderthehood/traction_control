#include <SDL2/SDL.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <memory>
#include <random> // For randomness
#include "Vehicle.h"
#include "TractionControl.h"
#include "Simulation.h"
#include "Visualizer.h"

void generateData(const std::string& outputFile, int numEntries) {
    std::ofstream dataFile(outputFile);

    dataFile << "wheel_index,slip_ratio,angular_velocity,linear_speed,"
             << "current_brake_torque,current_drive_torque,"
             << "desired_brake_torque,desired_drive_torque\n";

    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_real_distribution<double> frictionDist(0.5, 1.0); // Random road friction
    std::uniform_real_distribution<double> speedDist(5.0, 25.0);   // Random initial speed
    std::uniform_real_distribution<double> slipDist(0.05, 0.15);   // Random desired slip ratio
    std::uniform_int_distribution<int> stepsDist(500, 1500);       // Random number of steps

    int numWheels = 4; // Default to 4 wheels
    int entriesGenerated = 0;

    while (entriesGenerated < numEntries) {
        // Randomized parameters for each simulation
        double mu = frictionDist(rng);           // Random road friction
        double speed = speedDist(rng);           // Random initial speed
        double desiredSlip = slipDist(rng);      // Random desired slip ratio
        int steps = stepsDist(rng);              // Random number of simulation steps

        auto vehicle = std::make_shared<Vehicle>(speed, numWheels);
        auto tc = std::make_shared<TractionControl>(desiredSlip);

        double physicsDt = 0.01; // Fixed time step for consistency

        for (int step = 0; step < steps && entriesGenerated < numEntries; ++step) {
            tc->update(*vehicle, physicsDt); // Update vehicle state

            // Log data
            const auto& wheels = vehicle->getWheels();
            for (size_t i = 0; i < wheels.size(); ++i) {
                double slip = vehicle->computeSlipRatio(i);
                const auto& wheel = wheels[i];

                // Compute desired torques based on the current state
                double desiredBrakeTorque = 0.0;
                double desiredDriveTorque = 0.0;

                if (slip > desiredSlip) {
                    // Too much slip => ramp up brake, reduce drive
                    desiredBrakeTorque = std::min(200.0, wheel.brakeTorque + (500.0 * (slip - desiredSlip) * physicsDt));
                    desiredDriveTorque = std::max(0.0, wheel.driveTorque - (300.0 * (slip - desiredSlip) * physicsDt));
                } else {
                    // Too little slip => reduce brake, ramp up drive
                    double slipDiff = desiredSlip - slip;
                    desiredBrakeTorque = std::max(0.0, wheel.brakeTorque - (500.0 * slipDiff * physicsDt));
                    desiredDriveTorque = std::min(150.0, wheel.driveTorque + (300.0 * slipDiff * physicsDt));
                }

                // Write data to CSV
                dataFile << i << ","
                         << slip << ","
                         << wheel.angularVelocity << ","
                         << vehicle->getLinearSpeed() << ","
                         << wheel.brakeTorque << ","
                         << wheel.driveTorque << ","
                         << desiredBrakeTorque << "," 
                         << desiredDriveTorque << "\n";
                entriesGenerated++;
                if (entriesGenerated >= numEntries) break;
            }

            // Update vehicle physics
            vehicle->update(physicsDt);
        }
    }

    std::cout << "Data generation complete. Total entries: " << entriesGenerated << std::endl;
    dataFile.close();
}

int main(int argc, char* argv[]) {
    const std::string outputFile = "simulation_data.csv";
    const int numEntries = 1000;

    generateData(outputFile, numEntries);
    return 0;
}