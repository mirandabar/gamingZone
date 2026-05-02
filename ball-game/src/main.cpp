#include "../include/Renderer.h"
#include "../include/Vec2.h"
#include "../include/Ball.h"
#include "../include/Colours.h"
#include <chrono>
#include <thread>

int main() {
    Renderer renderer;
    
    if (!renderer.init("Ball Game")) {
        return 1;
    }
    
    // Initialize colors
    Colours::initialize(renderer);
    
    Colours colours(renderer);
    
    // Arena: círculo centrado, radio generoso para portrait
    Vec2  arenaCenter = { Renderer::WIDTH  / 2.0f,
                          Renderer::HEIGHT / 2.0f };
    float arenaRadius = Renderer::WIDTH * 0.44f; // ~198px de margen lateral

    // Dos bolas en lados opuestos del centro
    Vec2 ballRed  = { arenaCenter.x - arenaRadius * 0.5f, arenaCenter.y };
    Vec2 ballBlue = { arenaCenter.x + arenaRadius * 0.5f, arenaCenter.y };
    float ballRadius = 18.0f;

    bool running = true;

    while (running) {
        running = renderer.pollEvents();

        // --- Render ---
        renderer.clear();
        renderer.drawCircle(arenaCenter, arenaRadius, Colours::whiteColor);
        renderer.drawFilledCircle(ballRed,  ballRadius, Colours::redColor);
        renderer.drawFilledCircle(ballBlue, ballRadius, Colours::blueColor);
        renderer.present();

        // ~60 FPS
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    return 0;
}