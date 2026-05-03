#include "../include/Renderer.h"
#include "../include/GameManager.h"
#include <chrono>
#include <thread>
#include <vector>

int main() {
    Renderer renderer;
    GameManager gameManager(renderer);
    
    if (!renderer.init("Ball Game")) {
        return 1;
    }
    
    gameManager.initializeGame();

    // Sleep 1 seconds to show initial state
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    while (true) {
        gameManager.updateGame();
        gameManager.renderGame();
        
        // ~60 FPS
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    return 0;
}