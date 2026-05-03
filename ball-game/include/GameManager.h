#pragma once

#include "Renderer.h"
#include "Ball.h"
#include "CollisionManager.h"
#include <vector>

class GameManager {
public:
    GameManager(Renderer& renderer);

    void initializeGame();
    void updateGame();
    void renderGame();

private:
    Renderer& m_renderer;
    std::vector<Ball> m_balls;
    CollisionManager m_collisionManager;

    Vec2 m_arenaCenter;
    float m_arenaRadius;

    void initRandom();
    void generateRandomVelocity(Vec2& velocity);
    void generateRandomPosition(Vec2& position);

    void detectArenaCollisions(Ball& ball);
    void detectBallCollisions(Ball& ball1, Ball& ball2);
    void detectCrossingLine(Ball& ball1, Ball& ball2);
    void eraseLines(Ball& ball, Ball& ballLinesToDelete);
};