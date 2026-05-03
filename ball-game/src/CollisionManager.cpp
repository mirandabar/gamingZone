#include "../include/CollisionManager.h"
#include <cmath>

bool CollisionManager::checkCollision(const Ball& ball1, const Ball& ball2) const {
    float distance = (ball1.getPosition() - ball2.getPosition()).length();
    return distance < (ball1.getRadius() + ball2.getRadius());
}

void CollisionManager::resolveCollision(Ball& ball1, Ball& ball2) const {
    Vec2 normal = (ball2.getPosition() - ball1.getPosition()).normalize();
    Vec2 relativeVelocity = ball1.getVelocity() - ball2.getVelocity();

    float velocityAlongNormal = relativeVelocity.dot(normal);
    if (velocityAlongNormal > 0) return;

    float restitution = 1.0f; // Perfectly elastic collision
    float impulse = -(1 + restitution) * velocityAlongNormal;
    impulse /= 1 / ball1.getRadius() + 1 / ball2.getRadius();

    Vec2 impulseVector = normal * impulse;
    ball1.setVelocity(ball1.getVelocity() - impulseVector / ball1.getRadius());
    ball2.setVelocity(ball2.getVelocity() + impulseVector / ball2.getRadius());
}

bool CollisionManager::checkBoundaryCollision(const Ball& ball, const Vec2& arenaCenter, float arenaRadius) const {
    float distance = (ball.getPosition() - arenaCenter).length();
    return distance + ball.getRadius() > arenaRadius;
}

void CollisionManager::resolveBoundaryCollision(Ball& ball, const Vec2& arenaCenter) const {
    Vec2 normal = (ball.getPosition() - arenaCenter).normalize();
    ball.setVelocity(ball.getVelocity().reflect(normal));
}