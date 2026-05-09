#pragma once

#include "Vec2.h"
#include <SDL2/SDL.h>
#include <vector>
#include "Line.h"

class Ball
{
private:
    SDL_Color color;
    float radius;
    Vec2 position;
    Vec2 velocity;
    int id; 
    static int nextId;
    std::vector<Line> lines; // Líneas asociadas a la bola

public:
    Ball(SDL_Color color, float radius, Vec2 position);
    ~Ball();
    
    // Rule of Five
    Ball(const Ball&) = default;
    Ball& operator=(const Ball&) = default;
    Ball(Ball&&) = default;
    Ball& operator=(Ball&&) = default;

    // Getters
    SDL_Color getColor() const;
    float getRadius() const;
    Vec2 getPosition() const;
    Vec2 getVelocity() const;
    int getId() const;

    // Setters
    void setColor(SDL_Color color);
    void setRadius(float radius); 
    void setPosition(Vec2 position);
    void setVelocity(Vec2 velocity);
    void move();

    void addLine(const Line& line); 
    void eraseLine(int lineId);
    std::vector<Line>& getLines(); 
    void updateLines();
};

