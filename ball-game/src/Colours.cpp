#include "../include/Colours.h"
#include "../include/Renderer.h"
#include "../include/Logger.h"

static const std::string FILE_NAME = "Colours.cpp";

SDL_Color Colours::fillStruct(unsigned short r, unsigned short g, unsigned short b) {
    Logger::debug(FILE_NAME, "Colours::fillStruct", "Creating color RGB(" + std::to_string(r) + ", " + 
                 std::to_string(g) + ", " + std::to_string(b) + ")");
    return SDL_Color{static_cast<Uint8>(r), static_cast<Uint8>(g), static_cast<Uint8>(b), 255};
}

void Colours::initialize(Renderer& renderer) {
    Logger::info(FILE_NAME, "Colours::initialize", "Initializing colors");
    redColor = renderer.allocColor(220, 50, 50);
    blueColor = renderer.allocColor(50, 100, 220);
    whiteColor = renderer.allocColor(255, 255, 255);
    blackColor = renderer.allocColor(0, 0, 0);
    orangeColor = renderer.allocColor(255, 165, 0);
    purpleColor = renderer.allocColor(128, 0, 128);
    pinkColor = renderer.allocColor(255, 192, 203);
    greenColor = renderer.allocColor(0, 255, 0);
    brownColor = renderer.allocColor(165, 42, 42);
    yellowColor = renderer.allocColor(255, 255, 0);
    cyanColor = renderer.allocColor(0, 255, 255);
    grayColor = renderer.allocColor(128, 128, 128);
    Logger::info(FILE_NAME, "Colours::initialize", "Colors initialized successfully");
}

SDL_Color Colours::setBallColor(int number) {
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

SDL_Color Colours::redColor;
SDL_Color Colours::blueColor;
SDL_Color Colours::whiteColor;
SDL_Color Colours::blackColor;
SDL_Color Colours::orangeColor;
SDL_Color Colours::purpleColor;
SDL_Color Colours::pinkColor;
SDL_Color Colours::greenColor;
SDL_Color Colours::brownColor;
SDL_Color Colours::yellowColor;
SDL_Color Colours::cyanColor;
SDL_Color Colours::grayColor;

namespace {
bool sameColor(const SDL_Color& a, const SDL_Color& b) {
    return a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a;
}
}

std::string Colours::getColorName(const SDL_Color& color) {
    if (sameColor(color, redColor)) return "ROJA";
    if (sameColor(color, blueColor)) return "AZUL";
    if (sameColor(color, orangeColor)) return "NARANJA";
    if (sameColor(color, purpleColor)) return "PURPURA";
    if (sameColor(color, pinkColor)) return "ROSA";
    if (sameColor(color, greenColor)) return "VERDE";
    if (sameColor(color, brownColor)) return "MARRON";
    if (sameColor(color, yellowColor)) return "AMARILLA";
    if (sameColor(color, cyanColor)) return "CIAN";
    if (sameColor(color, grayColor)) return "GRIS";
    return "DESCONOCIDA";
}