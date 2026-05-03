#include "../include/GameManager.h"
#include "../include/Colours.h"
#include "../include/Config.h"

GameManager::GameManager(Renderer& renderer)
    : m_renderer(renderer),
    m_arenaCenter(ARENA_CENTER_X, ARENA_CENTER_Y),
    m_arenaRadius(ARENA_RADIUS) {}

void GameManager::initializeGame() {
    // Initialize colors
    Colours::initialize(m_renderer);

    // Create balls
    Vec2 initPositionRed = { m_arenaCenter.x - m_arenaRadius * 0.5f, m_arenaCenter.y };
    Vec2 initPositionBlue = { m_arenaCenter.x + m_arenaRadius * 0.5f, m_arenaCenter.y };

    float ballRadius = BALL_RADIUS;
    Vec2 initialVelocity = { INITIAL_VELOCITY_X, INITIAL_VELOCITY_Y };

    Ball redBall(Colours::redColor, ballRadius, initPositionRed);
    Ball blueBall(Colours::blueColor, ballRadius, initPositionBlue);

    redBall.setVelocity(initialVelocity);
    blueBall.setVelocity(initialVelocity * -1.0f);

    m_balls.push_back(redBall);
    m_balls.push_back(blueBall);

    renderGame();
}

void GameManager::updateGame() {
    for (auto& ball : m_balls) {
        ball.move();
        if (m_collisionManager.checkBoundaryCollision(ball, m_arenaCenter, m_arenaRadius)) {
            m_collisionManager.resolveBoundaryCollision(ball, m_arenaCenter);
        }
    }

    //for (size_t i = 0; i < m_balls.size(); ++i) {
    //    for (size_t j = i + 1; j < m_balls.size(); ++j) {
    //        if (m_collisionManager.checkCollision(m_balls[i], m_balls[j])) {
    //            m_collisionManager.resolveCollision(m_balls[i], m_balls[j]);
    //        }
    //    }
    //}
}

void GameManager::renderGame() {
    m_renderer.clear();
    m_renderer.drawCircle(m_arenaCenter, m_arenaRadius, Colours::whiteColor);

    for (const auto& ball : m_balls) {
        m_renderer.drawBall(ball);
    }

    m_renderer.present();
}