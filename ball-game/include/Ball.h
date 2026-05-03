#pragma once

#include "Vec2.h"
#include <X11/Xutil.h>
#include <vector>
#include "Line.h"

class Ball
{
private:
    XColor color;
    float radius;
    Vec2 position;
    Vec2 velocity;
    int id; 
    static int nextId;
    std::vector<Line> lines; // Líneas asociadas a la bola

public:
    Ball(XColor color, float radius, Vec2 position);
    ~Ball();

    // Getters
    XColor getColor() const;
    float getRadius() const;
    Vec2 getPosition() const;
    Vec2 getVelocity() const;
    int getId() const;

    // Setters
    void setColor(XColor color);
    void setRadius(float radius); 
    void setPosition(Vec2 position);
    void setVelocity(Vec2 velocity);
    void move();

    void addLine(const Line& line); 
    void eraseLine(int lineId);
    std::vector<Line>& getLines(); 
    void updateLines();
};

