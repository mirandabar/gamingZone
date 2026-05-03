#pragma once

#include "Vec2.h"
#include <X11/Xutil.h>

class Line
{
private:
    Vec2 start;
    Vec2 end;
    XColor color;

public:
    Line(Vec2 start, Vec2 end, XColor color);

    // Getters
    Vec2 getStart() const;
    Vec2 getEnd() const;
    XColor getColor() const;

    // Setters
    void setStart(Vec2 start);
    void setEnd(Vec2 end);
    void setColor(XColor color);
};