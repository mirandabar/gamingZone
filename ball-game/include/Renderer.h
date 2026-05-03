#pragma once
#include <X11/Xlib.h>
#include <string>
#include "Ball.h"
#include "Vec2.h"
#include <vector>
#include <unordered_map>

class Renderer {
public:
    // Dimensiones: formato portrait teléfono (9:16 aprox)
    static constexpr int WIDTH  = 450;
    static constexpr int HEIGHT = 800;

    Renderer();
    ~Renderer();
    
    // Rule of Five - forbid copying system resources, but allow move
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer(Renderer&&) = default;
    Renderer& operator=(Renderer&&) = default;

    bool init(const std::string& title);
    void clear();                                         
    void drawCircle(Vec2 center, float radius, XColor color);  
    void drawLine(Vec2 start, Vec2 end, XColor color);
    void drawFilledCircle(Vec2 center, float radius, XColor color); 
    void drawBall(const Ball& ball);                       
    void present();                                        
    bool pollEvents();                                     

    unsigned long allocColor(int r, int g, int b);

    void updateBalls(); 
    void eraseBall(const Ball& ball);

private:
    Display*    m_display;
    Window      m_window;
    GC          m_gc;          // Graphics Context
    Pixmap      m_backBuffer;  // doble buffer para evitar parpadeo
    std::unordered_map<int, Ball> ballsMap; // Mapa de bolas por ID

    void          drawCirclePoints(int cx, int cy, int x, int y); // Bresenham
};