#include "../include/Renderer.h"
#include "../include/Vec2.h"
#include "../include/Ball.h"
#include "../include/Colours.h"
#include <chrono>
#include <thread>
#include <vector>

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
    Vec2 initPositionRed  = { arenaCenter.x - arenaRadius * 0.5f, arenaCenter.y };
    Vec2 initPositionBlue = { arenaCenter.x + arenaRadius * 0.5f, arenaCenter.y };
    
    float ballRadius = 18.0f;
    Vec2 initialVelocity = { 1.0f, 0.0f };
    
    // Ball objects
    Ball redBall(Colours::redColor, ballRadius, initPositionRed);
    Ball blueBall(Colours::blueColor, ballRadius, initPositionBlue);

    redBall.setVelocity(initialVelocity);
    blueBall.setVelocity(initialVelocity * -1.0f);

    // Init draw
    renderer.clear();
    renderer.drawCircle(arenaCenter, arenaRadius, Colours::whiteColor);
    renderer.drawBall(redBall);
    renderer.drawBall(blueBall);
    renderer.present();

    // Sleep 1 seconds to show initial state
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    while (true) {
        //renderer.pollEvents();
        renderer.updateBalls();
        renderer.present();
        // ~60 FPS
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    return 0;
}