#include "../include/Colours.h"
#include "../include/Renderer.h"
#include "../include/Logger.h"

static const std::string FILE_NAME = "Colours.cpp";

XColor Colours::fillStruct(unsigned short r, unsigned short g, unsigned short b) {
    Logger::debug(FILE_NAME, "Colours::fillStruct", "Creating color RGB(" + std::to_string(r) + ", " + 
                 std::to_string(g) + ", " + std::to_string(b) + ")");
    XColor color;
    color.red   = r * 257; // X11 usa rango 0-65535
    color.green = g * 257;
    color.blue  = b * 257;
    color.flags = DoRed | DoGreen | DoBlue;
    color.pixel = renderer.allocColor(r, g, b);
    return color;
}

void Colours::initialize(Renderer& renderer) {
    Logger::info(FILE_NAME, "Colours::initialize", "Initializing colors");
    redColor.pixel = renderer.allocColor(220, 50, 50);
    blueColor.pixel = renderer.allocColor(50, 100, 220);
    whiteColor.pixel = renderer.allocColor(255, 255, 255);
    blackColor.pixel = renderer.allocColor(0, 0, 0);
    Logger::info(FILE_NAME, "Colours::initialize", "Colors initialized successfully");
}

XColor Colours::redColor;
XColor Colours::blueColor;
XColor Colours::whiteColor;
XColor Colours::blackColor;