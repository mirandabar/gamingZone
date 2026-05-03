#pragma once

#include "Ball.h"
#include "Vec2.h"

class CollisionManager {
public:
    bool checkCollision(const Ball& ball1, const Ball& ball2) const;
    void resolveCollision(Ball& ball1, Ball& ball2) const;
    Vec2 checkBoundaryCollision(const Ball& ball, const Vec2& arenaCenter, float arenaRadius, bool &collision) const;
    void resolveBoundaryCollision(Ball& ball, const Vec2& arenaCenter, float arenaRadius) const;
    bool checkBallLineCollision(const Ball& ball, const Line& line) const;
};