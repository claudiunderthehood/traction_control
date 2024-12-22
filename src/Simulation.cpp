#include "Simulation.h"
#include <thread>
#include <chrono>

Simulation::Simulation(std::shared_ptr<Vehicle> vehicle,
                       std::shared_ptr<TractionControl> tc,
                       std::shared_ptr<Visualizer> vis)
    : vehicle(std::move(vehicle)),
    tractionControl(std::move(tc)),
    visualizer(std::move(vis))
{}

void Simulation::run()
{
    using clock = std::chrono::steady_clock;

    const double physicsDt = 0.01; // 10 ms
    double accumulator = 0.0;

    auto prevTime = clock::now();

    while (visualizer->isRunning()) {
        // 1) Measure elapsed time in seconds
        auto currentTime = clock::now();
        double frameTime = std::chrono::duration<double>(currentTime - prevTime).count();
        prevTime = currentTime;

        // 2) Accumulate time
        accumulator += frameTime;

        // 3) While we have enough time accumulated for a physics step
        while (accumulator >= physicsDt) {
            // A) Update traction control => sets torque
            tractionControl->update(*vehicle, physicsDt);

            // B) Advance vehicle physics
            vehicle->update(physicsDt);

            accumulator -= physicsDt;
        }

        // 4) Render once per loop
        visualizer->render(*vehicle);

        // 5) Sleep a bit to limit CPU usage (e.g. ~20-30 fps render)
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }
}