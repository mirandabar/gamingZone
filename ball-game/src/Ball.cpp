#include "../include/Ball.h"
#include "../include/Vec2.h"

int Ball::nextId = 0; 

Ball::Ball(XColor color, float radius, Vec2 position)
    : id(nextId++)
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

Vec2 Ball::getVelocity() const
{
    return this->velocity;
}

int Ball::getId() const {
    return id;
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

// Método para mover la bola
void Ball::move()
{
    this->position = this->position + this->velocity;
    updateLines();
}

void Ball::setVelocity(Vec2 velocity)
{
    this->velocity = velocity;
}

void Ball::addLine(const Line& line) {
    lines.push_back(line);
}

const std::vector<Line>& Ball::getLines() const {
    return lines;
}

void Ball::updateLines() {
    for (auto& line : lines) {
        line.setStart(this->position);
    }
}