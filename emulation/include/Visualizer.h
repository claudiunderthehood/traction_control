#pragma once

#include <SDL2/SDL.h>
#include "Vehicle.h"

class Visualizer {
public:
    Visualizer();
    ~Visualizer();

    bool isRunning() const;
    void render(const Vehicle& vehicle);

private:
    SDL_Window* window;
    SDL_Renderer* renderer;

    void drawCarAndWheels(const Vehicle& vehicle);

    void drawBarGraphs(const Vehicle& vehicle);
};
