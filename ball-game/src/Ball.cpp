#include "../include/Ball.h"
#include "../include/Vec2.h"
#include "../include/Logger.h"

static const std::string FILE_NAME = "Ball.cpp";

int Ball::nextId = 0; 

Ball::Ball(XColor color, float radius, Vec2 position)
    : id(nextId++)
{
    Logger::debug(FILE_NAME, "Ball::Ball", "Creating new Ball with id: " + std::to_string(id));
    setColor(color);
    setRadius(radius);
    setPosition(position);
    Logger::info(FILE_NAME, "Ball::Ball", "Ball created successfully with id: " + std::to_string(id) + 
                 " at position (" + std::to_string(position.x) + ", " + std::to_string(position.y) + ")");
}

Ball::~Ball()
{
    Logger::debug(FILE_NAME, "Ball::~Ball", "Destroying Ball with id: " + std::to_string(id));
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
    Logger::debug(FILE_NAME, "Ball::move", "Moving ball " + std::to_string(id) + 
                 " from (" + std::to_string(position.x) + ", " + std::to_string(position.y) + ")");
    this->position = this->position + this->velocity;
    updateLines();
    Logger::debug(FILE_NAME, "Ball::move", "Ball " + std::to_string(id) + 
                 " moved to (" + std::to_string(position.x) + ", " + std::to_string(position.y) + ")");
}

void Ball::setVelocity(Vec2 velocity)
{
    this->velocity = velocity;
}

void Ball::addLine(const Line& line) {
    Logger::info(FILE_NAME, "Ball::addLine", "Adding line to ball " + std::to_string(id) + 
                 ". Total lines now: " + std::to_string(lines.size() + 1));
    lines.push_back(line);
}

void Ball::eraseLine(int lineId) {
    for (std::size_t i = 0; i < lines.size(); ++i) {
        if (lines[i].getId() == lineId) {
            Logger::info(FILE_NAME, "Ball::eraseLine", "Erasing line with id " + std::to_string(lineId) + 
                         " from ball " + std::to_string(id));
            lines.erase(lines.begin() + i);
            return;
        }
    }
}

std::vector<Line>& Ball::getLines() {
    return lines;
}

void Ball::updateLines() {
    for (auto& line : lines) {
        line.setStart(this->position);
    }
}