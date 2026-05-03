#include "../include/Line.h"
#include "../include/Logger.h"

static const std::string FILE_NAME = "Line.cpp";

int Line::nextId = 0;

Line::Line(Vec2 start, Vec2 end, XColor color)
    : start(start), end(end), color(color), id(nextId++) {
    Logger::debug(FILE_NAME, "Line::Line", "Creating line with id: " + std::to_string(id) + " from (" + 
                 std::to_string(start.x) + ", " + std::to_string(start.y) + ") to (" + 
                 std::to_string(end.x) + ", " + std::to_string(end.y) + ")");
}

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

int Line::getId() const {
    return id;
}

// Setters
void Line::setStart(Vec2 start) {
    Logger::debug(FILE_NAME, "Line::setStart", "Setting line start to (" + std::to_string(start.x) + ", " + 
                 std::to_string(start.y) + ")");
    this->start = start;
}   

void Line::setEnd(Vec2 end) {
    Logger::debug(FILE_NAME, "Line::setEnd", "Setting line end to (" + std::to_string(end.x) + ", " + 
                 std::to_string(end.y) + ")");
    this->end = end;
}

void Line::setColor(XColor color) {
    Logger::debug(FILE_NAME, "Line::setColor", "Setting line color");
    this->color = color;
}