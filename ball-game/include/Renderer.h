#pragma once
#include <X11/Xlib.h>
#include <string>
#include "Ball.h"
#include "Vec2.h"

class Renderer {
public:
    // Dimensiones: formato portrait teléfono (9:16 aprox)
    static constexpr int WIDTH  = 450;
    static constexpr int HEIGHT = 800;

    Renderer();
    ~Renderer();

    bool init(const std::string& title);
    void clear();                                          // borra el frame
    void drawCircle(Vec2 center, float radius, XColor color);  // círculo hueco
    void drawFilledCircle(Vec2 center, float radius, XColor color);
    void drawBall(const Ball& ball);                       
    void present();                                        // muestra el frame
    bool pollEvents();                                     // false = cerrar

    unsigned long allocColor(int r, int g, int b);

private:
    Display*    m_display;
    Window      m_window;
    GC          m_gc;          // Graphics Context
    Pixmap      m_backBuffer;  // doble buffer para evitar parpadeo

    void          drawCirclePoints(int cx, int cy, int x, int y); // Bresenham
};