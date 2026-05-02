#pragma once

#include "Vec2.h"
#include <X11/Xutil.h>

class Ball
{
private:
    XColor color;
    float radius;
    Vec2 position;
public:
    Ball(XColor color, float radius, Vec2 position);
    ~Ball();

    // Getters
    XColor getColor() const;
    float getRadius() const;
    Vec2 getPosition() const;

    // Setters
    void setColor(XColor color);
    void setRadius(float radius); 
    void setPosition(Vec2 position);
};

