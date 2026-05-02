#pragma once

struct Vec2 {
    float x, y;

    Vec2(float x = 0, float y = 0) : x(x), y(y) {}

    Vec2 operator+(const Vec2& o) const { return {x + o.x, y + o.y}; }
    Vec2 operator-(const Vec2& o) const { return {x - o.x, y - o.y}; }
    Vec2 operator*(float t)       const { return {x * t,   y * t};   }

    float dot(const Vec2& o)  const { return x * o.x + y * o.y; }
    float cross(const Vec2& o) const { return x * o.y - y * o.x; }
    float length()            const;
    Vec2  normalize()         const;
    Vec2  reflect(const Vec2& normal) const;
};