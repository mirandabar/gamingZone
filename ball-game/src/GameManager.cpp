#include "../include/GameManager.h"
#include "../include/Colours.h"
#include "../include/Config.h"
#include "../include/Vec2.h"
#include "../include/Line.h"

GameManager::GameManager(Renderer& renderer)
    : m_renderer(renderer),
    m_arenaCenter(ARENA_CENTER_X, ARENA_CENTER_Y),
    m_arenaRadius(ARENA_RADIUS) {}

void GameManager::initRandom() {
    srand(static_cast<unsigned int>(time(nullptr)));
}

void GameManager::generateRandomVelocity(Vec2& velocity) {
    // Genera una velocidad aleatoria entre -5 y 5 para x e y
    velocity.x = (static_cast<float>(rand()) / RAND_MAX) * 10.0f - 5.0f;
    velocity.y = (static_cast<float>(rand()) / RAND_MAX) * 10.0f - 5.0f;
}

void GameManager::generateRandomPosition(Vec2& position) {
    // Genera una posición aleatoria dentro del área de juego
    float angle = static_cast<float>(rand()) / RAND_MAX * 2.0f * M_PI;
    float radius = static_cast<float>(rand()) / RAND_MAX * (m_arenaRadius - BALL_RADIUS);
    position.x = m_arenaCenter.x + radius * cos(angle);
    position.y = m_arenaCenter.y + radius * sin(angle);
}

void GameManager::initializeGame() {
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

    renderGame();
}

void GameManager::updateGame() {
    for (auto& ball : m_balls) {
        ball.move();

        Vec2 pointColision;
        bool collision = false;

        pointColision = m_collisionManager.checkBoundaryCollision(ball, m_arenaCenter, m_arenaRadius, collision);

        if (collision) {
            Line lineConection = Line(ball.getPosition(), pointColision, ball.getColor());
            ball.addLine(lineConection);
            m_renderer.drawLine(lineConection.getStart(), lineConection.getEnd(), lineConection.getColor());
            m_collisionManager.resolveBoundaryCollision(ball, m_arenaCenter);
        }
    }

    for (size_t i = 0; i < m_balls.size(); ++i) {
        for (size_t j = i + 1; j < m_balls.size(); ++j) {
            if (m_collisionManager.checkCollision(m_balls[i], m_balls[j])) {
                m_collisionManager.resolveCollision(m_balls[i], m_balls[j]);
            }
        }
    }
}

void GameManager::renderGame() {
    m_renderer.clear();
    m_renderer.drawCircle(m_arenaCenter, m_arenaRadius, Colours::whiteColor);

    for (const auto& ball : m_balls) {
        m_renderer.drawBall(ball);
        for (const auto& line : ball.getLines()) {
            m_renderer.drawLine(line.getStart(), line.getEnd(), line.getColor());
        }
    }

    m_renderer.present();
}