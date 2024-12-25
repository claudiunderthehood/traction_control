#include "Visualizer.h"
#include <iostream>
#include <cmath>
#include <algorithm>

Visualizer::Visualizer()
    : window(nullptr), renderer(nullptr)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL Initialization Error: " << SDL_GetError() << std::endl;
        exit(EXIT_FAILURE);
    }

    window = SDL_CreateWindow("Traction Control Simulation",
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              800, 600,
                              SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Window Creation Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer Creation Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        exit(EXIT_FAILURE);
    }
}

Visualizer::~Visualizer()
{
    if (renderer) {
        SDL_DestroyRenderer(renderer);
    }
    if (window) {
        SDL_DestroyWindow(window);
    }
    SDL_Quit();
}

bool Visualizer::isRunning() const
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            std::cout << "Quit event detected. Exiting simulation." << std::endl;
            return false;
        }
    }
    return true;
}

void Visualizer::render(const Vehicle& vehicle)
{
    // Clear to black
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    drawCarAndWheels(vehicle);
    drawBarGraphs(vehicle);

    SDL_RenderPresent(renderer);
}

void Visualizer::drawCarAndWheels(const Vehicle& vehicle)
{
    // Draw a simple top‐down rectangle for the car, with 4 wheels at corners
    // For brevity, we’ll assume 4 wheels. If you have more, you can adapt.

    if (vehicle.getWheels().empty()) {
        std::cerr << "Error: Vehicle has no wheels to render!" << std::endl;
        return;
    }

    const int carX = 400;  // center of screen
    const int carY = 300;
    const int carW = 120;  // car body width
    const int carH = 200;  // car body height

    // Draw car body (white)
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_Rect carRect = {carX - carW/2, carY - carH/2, carW, carH};
    SDL_RenderFillRect(renderer, &carRect);

    const auto& wheels = vehicle.getWheels();
    if (wheels.size() < 4) return; // skip if not exactly 4 wheels

    // We'll place wheels at corners of that rectangle
    // front-left, front-right, rear-left, rear-right
    // Then rotate them according to w.rotationAngle.

    // For demonstration, let's define offsets from the center:
    struct WheelPos {
        int dx, dy; // offsets from car center
    };
    WheelPos wheelOffsets[4] = {
        {-carW/2 - 10, -carH/2 + 20}, // front-left
        { carW/2 + 10, -carH/2 + 20}, // front-right
        {-carW/2 - 10,  carH/2 - 20}, // rear-left
        { carW/2 + 10,  carH/2 - 20}, // rear-right
    };

    // Wheel dimension
    const int wWidth = 12;
    const int wHeight = 30;

    // For each wheel, we'll rotate a small rectangle in place
    for (int i = 0; i < 4; i++) {
        double angleRad = wheels[i].rotationAngle;

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

        // Compute wheel center
        int wxCenter = carX + wheelOffsets[i].dx;
        int wyCenter = carY + wheelOffsets[i].dy;

        SDL_Rect wheelRect = {
            wxCenter - wWidth/2,
            wyCenter - wHeight/2,
            wWidth,
            wHeight
        };
        SDL_RenderFillRect(renderer, &wheelRect);

    }
}

void Visualizer::drawBarGraphs(const Vehicle& vehicle)
{
    // --- Green bar (vehicle linear speed) ---
    double linSpeed = vehicle.getLinearSpeed();
    int greenHeight = static_cast<int>(linSpeed * 5.0);
    if (greenHeight > 500) greenHeight = 500;

    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_Rect speedRect = { 50, 550 - greenHeight, 30, greenHeight };
    SDL_RenderFillRect(renderer, &speedRect);

    // --- Blue bars (each wheel speed) ---
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    int xOffset = 100;
    for (int i = 0; i < (int)vehicle.getWheels().size(); i++) {
        double wSpeed = vehicle.getWheels()[i].angularVelocity * vehicle.wheelRadius;
        int barHeight = static_cast<int>(wSpeed * 5.0);
        if (barHeight < 0)   barHeight = 0;
        if (barHeight > 500) barHeight = 500;

        SDL_Rect wheelRect = { xOffset, 550 - barHeight, 20, barHeight };
        SDL_RenderFillRect(renderer, &wheelRect);

        xOffset += 30;
    }

    // --- Red bars (slip ratio) - moved to the right side, e.g. ~x=700 ---
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

    int slipX = 700;  
    for (int i = 0; i < (int)vehicle.getWheels().size(); i++) {
        double slip = vehicle.computeSlipRatio(i);
        double slipPixels = 100.0 * std::fabs(slip);
        if (slipPixels > 200) slipPixels = 200;

        int barHeight = static_cast<int>(slipPixels);
        SDL_Rect slipRect = { slipX, 550 - barHeight, 20, barHeight };
        SDL_RenderFillRect(renderer, &slipRect);

        slipX += 30;
    }
}