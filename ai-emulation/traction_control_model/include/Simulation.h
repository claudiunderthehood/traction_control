#pragma once

#include <memory>
#include "Vehicle.h"
#include "TractionControl.h"
#include "Visualizer.h"

class Simulation {
public:
    Simulation(std::shared_ptr<Vehicle> vehicle,
               std::shared_ptr<TractionControl> tc,
               std::shared_ptr<Visualizer> vis);

    void run();

private:
    std::shared_ptr<Vehicle> vehicle;
    std::shared_ptr<TractionControl> tractionControl;
    std::shared_ptr<Visualizer> visualizer;
};
