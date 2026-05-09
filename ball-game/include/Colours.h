#pragma once

#include "Renderer.h"
#include <SDL2/SDL.h>
#include <string>

class Colours
{
private:
    Renderer& renderer;
    SDL_Color fillStruct(unsigned short r, unsigned short g, unsigned short b);
public:
    Colours(Renderer& renderer) : renderer(renderer) {};
    ~Colours() = default;

    static SDL_Color redColor;
    static SDL_Color blueColor;
    static SDL_Color whiteColor;
    static SDL_Color blackColor;
    static SDL_Color orangeColor;
    static SDL_Color purpleColor;
    static SDL_Color pinkColor;
    static SDL_Color greenColor;
    static SDL_Color brownColor;
    static SDL_Color yellowColor;
    static SDL_Color cyanColor;
    static SDL_Color grayColor;

    // Initializes colors using the renderer's colormap
    static void initialize(Renderer& renderer);
    static SDL_Color setBallColor(int number);
    static std::string getColorName(const SDL_Color& color);
};