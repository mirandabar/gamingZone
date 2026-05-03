#include "../include/CollisionManager.h"
#include <cmath>

bool CollisionManager::checkCollision(const Ball& ball1, const Ball& ball2) const {
    float distance = (ball1.getPosition() - ball2.getPosition()).length();
    return distance < (ball1.getRadius() + ball2.getRadius());
}

void CollisionManager::resolveCollision(Ball& ball1, Ball& ball2) const {
    Vec2 deltaPosition = ball2.getPosition() - ball1.getPosition();
    float distance = deltaPosition.length();
    float r = ball1.getRadius() + ball2.getRadius();

    if (distance > r) {
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
}

bool CollisionManager::checkBoundaryCollision(const Ball& ball, const Vec2& arenaCenter, float arenaRadius) const {
    float distance = (ball.getPosition() - arenaCenter).length();
    return distance + ball.getRadius() > arenaRadius;
}

void CollisionManager::resolveBoundaryCollision(Ball& ball, const Vec2& arenaCenter) const {
    Vec2 normal = (ball.getPosition() - arenaCenter).normalize();
    ball.setVelocity(ball.getVelocity().reflect(normal));
}