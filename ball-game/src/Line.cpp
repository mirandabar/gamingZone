#include "../include/Line.h"

Line::Line(Vec2 start, Vec2 end, XColor color)
    : start(start), end(end), color(color) {}

// Getters
Vec2 Line::getStart() const {
    return start;
}

Vec2 Line::getEnd() const {
    return end;
}

XColor Line::getColor() const {
    return color;
}

// Setters
void Line::setStart(Vec2 start) {
    this->start = start;
}   

void Line::setEnd(Vec2 end) {
    this->end = end;
}

void Line::setColor(XColor color) {
    this->color = color;
}