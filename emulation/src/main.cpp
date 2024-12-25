#include <SDL2/SDL.h>
#include <memory>
#include "Vehicle.h"
#include "TractionControl.h"
#include "Visualizer.h"
#include "Simulation.h"

int main(int argc, char* argv[])
{
    auto vehicle = std::make_shared<Vehicle>(5.0, 4);
    auto tc = std::make_shared<TractionControl>(0.1);
    auto vis = std::make_shared<Visualizer>();

    Simulation sim(vehicle, tc, vis);
    sim.run();

    return 0;
}
