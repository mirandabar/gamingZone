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
    orangeColor.pixel = renderer.allocColor(255, 165, 0);
    purpleColor.pixel = renderer.allocColor(128, 0, 128);
    pinkColor.pixel = renderer.allocColor(255, 192, 203);
    greenColor.pixel = renderer.allocColor(0, 255, 0);
    brownColor.pixel = renderer.allocColor(165, 42, 42);
    yellowColor.pixel = renderer.allocColor(255, 255, 0);
    cyanColor.pixel = renderer.allocColor(0, 255, 255);
    grayColor.pixel = renderer.allocColor(128, 128, 128);
    Logger::info(FILE_NAME, "Colours::initialize", "Colors initialized successfully");
}

XColor Colours::setBallColor(int number) {
    switch (number) {
        case 0: return redColor;
        case 1: return blueColor;
        case 2: return orangeColor;
        case 3: return purpleColor;
        case 4: return pinkColor;
        case 5: return greenColor;
        case 6: return brownColor;
        case 7: return yellowColor;
        case 8: return cyanColor;
        case 9: return grayColor;
        default:
            Logger::warning(FILE_NAME, "Colours::setBallColor", "Creating a random color");
            //return Colours::fillStruct(rand() % 256, rand() % 256, rand() % 256);
            return whiteColor; // fallback
    }
}

XColor Colours::redColor;
XColor Colours::blueColor;
XColor Colours::whiteColor;
XColor Colours::blackColor;
XColor Colours::orangeColor;
XColor Colours::purpleColor;
XColor Colours::pinkColor;
XColor Colours::greenColor;
XColor Colours::brownColor;
XColor Colours::yellowColor;
XColor Colours::cyanColor;
XColor Colours::grayColor;