#include "../include/GameManager.h"
#include "../include/Colours.h"
#include "../include/Config.h"
#include "../include/Vec2.h"
#include "../include/Line.h"
#include "../include/Logger.h"

static const std::string FILE_NAME = "GameManager.cpp";

GameManager::GameManager(Renderer& renderer)
    : m_renderer(renderer),
    m_arenaCenter(ARENA_CENTER_X, ARENA_CENTER_Y),
    m_arenaRadius(ARENA_RADIUS) {
    Logger::debug(FILE_NAME, "GameManager::GameManager", "GameManager constructor - Initializing with arena center (" + 
                 std::to_string(ARENA_CENTER_X) + ", " + std::to_string(ARENA_CENTER_Y) + ") and radius " + 
                 std::to_string(ARENA_RADIUS));
}

void GameManager::initRandom() {
    srand(static_cast<unsigned int>(time(nullptr)));
    Logger::debug(FILE_NAME, "GameManager::initRandom", "Random seed initialized");
}

void GameManager::generateRandomVelocity(Vec2& velocity) {
    // Genera una velocidad aleatoria entre -5 y 5 para x e y
    velocity.x = (static_cast<float>(rand()) / RAND_MAX) * 10.0f - 5.0f;
    velocity.y = (static_cast<float>(rand()) / RAND_MAX) * 10.0f - 5.0f;
    Logger::debug(FILE_NAME, "GameManager::generateRandomVelocity", "Generated velocity: (" + 
                 std::to_string(velocity.x) + ", " + std::to_string(velocity.y) + ")");
}

void GameManager::generateRandomPosition(Vec2& position) {
    // Genera una posición aleatoria dentro del área de juego
    float angle = static_cast<float>(rand()) / RAND_MAX * 2.0f * M_PI;
    float radius = static_cast<float>(rand()) / RAND_MAX * (m_arenaRadius - BALL_RADIUS);
    position.x = m_arenaCenter.x + radius * cos(angle);
    position.y = m_arenaCenter.y + radius * sin(angle);
    Logger::debug(FILE_NAME, "GameManager::generateRandomPosition", "Generated position: (" + 
                 std::to_string(position.x) + ", " + std::to_string(position.y) + ")");
}

void GameManager::initializeGame() {
    Logger::info(FILE_NAME, "GameManager::initializeGame", "Initializing game...");
    
    // Initialize colors
    Colours::initialize(m_renderer);

    // Initialize random seed
    initRandom();

    Vec2 initPositionRed, initPositionBlue;
    generateRandomPosition(initPositionRed);
    generateRandomPosition(initPositionBlue);

    float ballRadius = BALL_RADIUS;
    Vec2 initialVelocity;

    generateRandomVelocity(initialVelocity);

    Ball redBall(Colours::redColor, ballRadius, initPositionRed);
    Ball blueBall(Colours::blueColor, ballRadius, initPositionBlue);

    redBall.setVelocity(initialVelocity);
    blueBall.setVelocity(initialVelocity * -1.0f);

    m_balls.push_back(redBall);
    m_balls.push_back(blueBall);
    
    Logger::info(FILE_NAME, "GameManager::initializeGame", "Game initialized successfully with " + 
                std::to_string(m_balls.size()) + " balls");

    renderGame();
}

void GameManager::detectArenaCollisions(Ball& ball) {
    Logger::debug(FILE_NAME, "GameManager::detectArenaCollisions", "Checking arena collisions for ball " + 
                 std::to_string(ball.getId()) + " at position (" + std::to_string(ball.getPosition().x) + 
                 ", " + std::to_string(ball.getPosition().y) + ")");

    Vec2 pointColision;
    bool collision = false;

    pointColision = m_collisionManager.checkBoundaryCollision(ball, m_arenaCenter, m_arenaRadius, collision);

    if (collision) {
        Logger::info(FILE_NAME, "GameManager::detectArenaCollisions", "Boundary collision detected for ball " + std::to_string(ball.getId()));
        Line lineConection = Line(ball.getPosition(), pointColision, ball.getColor());
        ball.addLine(lineConection);
        m_renderer.drawLine(lineConection.getStart(), lineConection.getEnd(), lineConection.getColor());
        m_collisionManager.resolveBoundaryCollision(ball, m_arenaCenter);
    }
}

void GameManager::detectBallCollisions(Ball& ball1, Ball& ball2) {
    Logger::debug(FILE_NAME, "GameManager::detectBallCollisions", "Checking for ball collisions between ball " + std::to_string(ball1.getId()) + " and ball " + std::to_string(ball2.getId()));
    if (m_collisionManager.checkCollision(ball1, ball2)) {
        Logger::info(FILE_NAME, "GameManager::detectBallCollisions", "Ball collision detected between ball " + 
                    std::to_string(ball1.getId()) + " and ball " + std::to_string(ball2.getId()));
        m_collisionManager.resolveCollision(ball1, ball2);
    }
}

void GameManager::eraseLines(Ball& ball, Ball& ballLinesToDelete) {
    Logger::debug(FILE_NAME, "GameManager::eraseLines", "Checking for line collisions to erase for ball " + 
                 std::to_string(ball.getId()) + " against lines of ball " + std::to_string(ballLinesToDelete.getId()));
    for (auto& line : ballLinesToDelete.getLines()) {
        if (m_collisionManager.checkBallLineCollision(ball, line)) {
            ballLinesToDelete.eraseLine(line.getId());
            Logger::info(FILE_NAME, "GameManager::eraseLines", "Erased line with id " + std::to_string(line.getId()) + 
                         " from ball " + std::to_string(ballLinesToDelete.getId()));
        }
    }
}

void GameManager::detectCrossingLine(Ball& ball1, Ball& ball2) {
    Logger::debug(FILE_NAME, "GameManager::detectCrossingLine", "Checking for line crossing between ball " + 
                 std::to_string(ball1.getId()) + " and ball " + std::to_string(ball2.getId()));
    
    eraseLines(ball1, ball2);
    eraseLines(ball2, ball1);
}

void GameManager::updateGame() {
    Logger::debug(FILE_NAME, "GameManager::updateGame", "Updating game - processing " + std::to_string(m_balls.size()) + " balls");
    
    for (auto& ball : m_balls) {
        ball.move();
        detectArenaCollisions(ball);
    }

    for (size_t i = 0; i < m_balls.size(); ++i) {
        for (size_t j = i + 1; j < m_balls.size(); ++j) {
            detectBallCollisions(m_balls[i], m_balls[j]);
            detectCrossingLine(m_balls[i],m_balls[j]);
        }
    }
}

void GameManager::renderGame() {
    Logger::debug(FILE_NAME, "GameManager::renderGame", "Rendering game - drawing " + std::to_string(m_balls.size()) + " balls");
    m_renderer.clear();
    m_renderer.drawCircle(m_arenaCenter, m_arenaRadius, Colours::whiteColor);

    for (auto& ball : m_balls) {
        m_renderer.drawBall(ball);
        for (auto& line : ball.getLines()) {
            m_renderer.drawLine(line.getStart(), line.getEnd(), line.getColor());
        }
    }

    m_renderer.present();
}