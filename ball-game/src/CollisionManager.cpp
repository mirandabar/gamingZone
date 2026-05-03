#include "../include/CollisionManager.h"
#include "../include/Logger.h"
#include <cmath>

static const std::string FILE_NAME = "CollisionManager.cpp";

bool CollisionManager::checkCollision(const Ball& ball1, const Ball& ball2) const {
    float distance = (ball1.getPosition() - ball2.getPosition()).length();
    bool collided = distance < (ball1.getRadius() + ball2.getRadius());
    if (collided) {
        Logger::debug(FILE_NAME, "CollisionManager::checkCollision", "Collision detected between ball " + 
                     std::to_string(ball1.getId()) + " and ball " + std::to_string(ball2.getId()) + 
                     " at distance: " + std::to_string(distance));
    }
    return collided;
}

void CollisionManager::resolveCollision(Ball& ball1, Ball& ball2) const {
    Logger::debug(FILE_NAME, "CollisionManager::resolveCollision", "Resolving collision between ball " + 
                 std::to_string(ball1.getId()) + " and ball " + std::to_string(ball2.getId()));
    
    Vec2 deltaPosition = ball2.getPosition() - ball1.getPosition();
    float distance = deltaPosition.length();
    float r = ball1.getRadius() + ball2.getRadius();

    if (distance > r) {
        Logger::debug(FILE_NAME, "CollisionManager::resolveCollision", "Distance > r, no collision to resolve");
        return;
    }

    Vec2 normal = deltaPosition.normalize();
    Vec2 t = {-normal.y, normal.x}; // Tangential vector

    float v1n = ball1.getVelocity().dot(normal);
    float v1t = ball1.getVelocity().dot(t);
    float v2n = ball2.getVelocity().dot(normal);
    float v2t = ball2.getVelocity().dot(t);

    // Interchange normal velocities (elastic collision)
    Vec2 v1 = t * v1t + normal * v2n;
    Vec2 v2 = t * v2t + normal * v1n;

    // Update velocities of the balls
    ball1.setVelocity(v1);
    ball2.setVelocity(v2);

    // Correct positions to prevent sticking
    float overlap = r - distance;
    Vec2 correction = normal * (overlap / 2.0f);

    ball1.setPosition(ball1.getPosition() - correction);
    ball2.setPosition(ball2.getPosition() + correction);
    
    Logger::info(FILE_NAME, "CollisionManager::resolveCollision", "Collision resolved - velocities exchanged");
}

Vec2 CollisionManager::checkBoundaryCollision(const Ball& ball, const Vec2& arenaCenter, float arenaRadius, bool &collision) const {
    float distance = (ball.getPosition() - arenaCenter).length();

    if (distance + ball.getRadius() > arenaRadius) {
        collision = true;
        Vec2 normal = (ball.getPosition() - arenaCenter).normalize();
        Vec2 contactPoint = arenaCenter + normal * arenaRadius;
        Logger::debug(FILE_NAME, "CollisionManager::checkBoundaryCollision", "Boundary collision detected for ball " + 
                     std::to_string(ball.getId()));
        return contactPoint;
    }

    collision = false;
    return Vec2(0, 0);
}

void CollisionManager::resolveBoundaryCollision(Ball& ball, const Vec2& arenaCenter) const {
    Logger::debug(FILE_NAME, "CollisionManager::resolveBoundaryCollision", "Resolving boundary collision for ball " + 
                 std::to_string(ball.getId()));
    Vec2 normal = (ball.getPosition() - arenaCenter).normalize();
    ball.setVelocity(ball.getVelocity().reflect(normal));
    Logger::info(FILE_NAME, "CollisionManager::resolveBoundaryCollision", "Boundary collision resolved - ball " + 
                std::to_string(ball.getId()) + " velocity reflected");
}