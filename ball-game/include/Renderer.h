#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <cstdint>
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
    void drawCircle(Vec2 center, float radius, SDL_Color color);  
    void drawLine(Vec2 start, Vec2 end, SDL_Color color);
    void drawFilledCircle(Vec2 center, float radius, SDL_Color color); 
    void drawBall(const Ball& ball);                       
    void present();                                        
    bool pollEvents();                                     

    SDL_Color allocColor(int r, int g, int b);

    void updateBalls(); 
    void eraseBall(const Ball& ball);

    void showMessage(const std::string& message);

    void setFontSize(int size);
    void setFont(const std::string& fontName);

    void setScreenColor(SDL_Color color);

    // Video capture
    uint8_t* captureFrame(int& outSize);
    void releaseFrameBuffer(uint8_t* buffer);

private:
    SDL_Window*   m_window;
    SDL_Renderer* m_renderer;
    SDL_Texture*  m_backBuffer;
    TTF_Font*     m_font;
    std::string   m_fontPath;
    int           m_fontSize;
    bool          m_videoInitialized;
    std::unordered_map<int, Ball> ballsMap; // Mapa de bolas por ID

    bool          loadFont();
    void          drawCirclePoints(int cx, int cy, int x, int y); // Bresenham
};