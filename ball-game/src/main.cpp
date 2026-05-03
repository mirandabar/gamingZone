#include "../include/Renderer.h"
#include "../include/GameManager.h"
#include "../include/Logger.h"
#include <chrono>
#include <thread>
#include <vector>

static const std::string FILE_NAME = "main.cpp";

int main() {
    Logger::info(FILE_NAME, "main", "========== STARTING BALL GAME ==========");
    
    try {
        Renderer renderer;
        GameManager gameManager(renderer);
        
        Logger::info(FILE_NAME, "main", "Initializing Renderer");
        if (!renderer.init("Ball Game")) {
            Logger::critical(FILE_NAME, "main", "FAILED: Could not initialize Renderer");
            return 1;
        }
        
        Logger::info(FILE_NAME, "main", "Initializing Game");
        gameManager.initializeGame();
        gameManager.renderGame(); // Render initial state

        // Sleep 1 seconds to show initial state
        Logger::debug(FILE_NAME, "main", "Sleeping for 1 second to show initial state");
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        Logger::info(FILE_NAME, "main", "Starting game loop");
        while (true) {
            gameManager.updateGame();
            gameManager.renderGame();
            
            // ~60 FPS
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }
        
        Logger::info(FILE_NAME, "main", "========== GAME ENDED NORMALLY ==========");
        return 0;
    } catch (const std::exception& e) {
        Logger::critical(FILE_NAME, "main", "EXCEPTION CAUGHT: " + std::string(e.what()));
        return 1;
    }
}