#include "../include/Renderer.h"
#include "../include/GameManager.h"
#include "../include/Logger.h"
#include "../include/Colours.h"
#include "../include/Audio.h"
#include "../include/VideoRecorder.h"
#include <chrono>
#include <thread>
#include <vector>
#include <ctime>
#include <iomanip>
#include <sstream>

static const std::string FILE_NAME = "main.cpp";

std::string generateVideoFilename() {
    auto now = std::time(nullptr);
    auto tm = *std::localtime(&now);
    std::ostringstream oss;
    oss << "/root/gitRepos/gamingZone/ball-game/video/ball-game-"
        << std::put_time(&tm, "%Y%m%d-%H%M%S") << ".mp4";
    return oss.str();
}

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
        
        // Initialize video recording
        std::string videoFilename = generateVideoFilename();
        VideoRecorder videoRecorder(videoFilename, Renderer::WIDTH, Renderer::HEIGHT, 60);
        if (!videoRecorder.initialize()) {
            Logger::warning(FILE_NAME, "main", "Failed to initialize video recording, continuing without recording");
        }
        
        Logger::info(FILE_NAME, "main", "Initializing Game");
        gameManager.initializeGame();
        gameManager.renderGame(); // Render initial state
        
        // Capture initial frame
        if (videoRecorder.isInitialized()) {
            int frameSize = 0;
            uint8_t* frameData = renderer.captureFrame(frameSize);
            if (frameData) {
                videoRecorder.captureFrame(frameData, frameSize);
                renderer.releaseFrameBuffer(frameData);
            }
        }

        gameManager.renderGame();
        // Sleep 1 seconds to show initial state
        Logger::debug(FILE_NAME, "main", "Sleeping for 1 second to show initial state");
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        Logger::info(FILE_NAME, "main", "Starting game loop");
        while (gameManager.onlyOneBallLeft() == false) {
            gameManager.updateGame();
            gameManager.renderGame();
            
            // Capture frame for video
            if (videoRecorder.isInitialized()) {
                int frameSize = 0;
                uint8_t* frameData = renderer.captureFrame(frameSize);
                if (frameData) {
                    videoRecorder.captureFrame(frameData, frameSize);
                    renderer.releaseFrameBuffer(frameData);
                }
            }
            
            // ~60 FPS
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }
        gameManager.renderGame();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        Audio::playSound("/root/gitRepos/gamingZone/ball-game/audio/winnerSound.wav");
        
        XColor winningColor = gameManager.getWinningBallColor();
        renderer.setScreenColor(winningColor);
        
        renderer.showMessage("!!! WINNER !!!");
        
        // Capture final frames
        if (videoRecorder.isInitialized()) {
            for (int i = 0; i < 120; i++) { // 2 seconds at 60 FPS
                int frameSize = 0;
                uint8_t* frameData = renderer.captureFrame(frameSize);
                if (frameData) {
                    videoRecorder.captureFrame(frameData, frameSize);
                    renderer.releaseFrameBuffer(frameData);
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(16));
            }
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        
        // Finalize video recording
        if (videoRecorder.isInitialized()) {
            videoRecorder.finalize();
            Logger::info(FILE_NAME, "main", "Video saved to: " + videoFilename);
        }
        
        Audio::shutdown();
        
        Logger::info(FILE_NAME, "main", "========== GAME ENDED NORMALLY ==========");
        return 0;
    } catch (const std::exception& e) {
        Logger::critical(FILE_NAME, "main", "EXCEPTION CAUGHT: " + std::string(e.what()));
        return 1;
    }
}