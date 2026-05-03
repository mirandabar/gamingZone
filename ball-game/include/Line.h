#pragma once

#include "Vec2.h"
#include <X11/Xutil.h>

class Line
{
private:
    Vec2 start;
    Vec2 end;
    XColor color;
    int id;
    static int nextId;

public:
    Line(Vec2 start, Vec2 end, XColor color);

    // Getters
    Vec2 getStart() const;
    Vec2 getEnd() const;
    XColor getColor() const;
    int getId() const;

    // Setters
    void setStart(Vec2 start);
    void setEnd(Vec2 end);
    void setColor(XColor color);
};