#pragma once

#include "Renderer.h"
#include <X11/Xlib.h>

class Colours
{
private:
    Renderer& renderer;
    XColor fillStruct(unsigned short r, unsigned short g, unsigned short b);
public:
    Colours(Renderer& renderer) : renderer(renderer) {};
    ~Colours() = default;

    static XColor redColor;
    static XColor blueColor;
    static XColor whiteColor;
    static XColor blackColor;
    static XColor orangeColor;
    static XColor purpleColor;
    static XColor pinkColor;
    static XColor greenColor;
    static XColor brownColor;
    static XColor yellowColor;
    static XColor cyanColor;
    static XColor grayColor;

    // Initializes colors using the renderer's colormap
    static void initialize(Renderer& renderer);
    static XColor setBallColor(int number);
    static std::string getColorName(const XColor& color);
};