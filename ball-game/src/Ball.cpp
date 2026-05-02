#include "../include/Ball.h"


Ball::Ball(XColor color, float radius, Vec2 position)
{
    setColor(color);
    setRadius(radius);
    setPosition(position);
}

Ball::~Ball()
{
}

// ----------
// Getters
// ----------

XColor Ball::getColor() const
{
    return this->color;
}

float Ball::getRadius() const
{
    return this->radius;
}

Vec2 Ball::getPosition() const
{
    return this->position;
}

// ----------
// Setters 
// ----------

void Ball::setColor(XColor color)
{
    this->color = color;
}

void Ball::setRadius(float radius)
{
    this->radius = radius;
}

void Ball::setPosition(Vec2 position)
{
    this->position = position;
}

