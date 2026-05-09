#pragma once

#include "Vec2.h"
#include <SDL2/SDL.h>

class Line
{
private:
    Vec2 start;
    Vec2 end;
    SDL_Color color;
    int id;
    static int nextId;

public:
    Line(Vec2 start, Vec2 end, SDL_Color color);

    // Getters
    Vec2 getStart() const;
    Vec2 getEnd() const;
    SDL_Color getColor() const;
    int getId() const;

    // Setters
    void setStart(Vec2 start);
    void setEnd(Vec2 end);
    void setColor(SDL_Color color);
};