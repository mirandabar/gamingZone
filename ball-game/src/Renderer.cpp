#include "../include/Renderer.h"
#include "../include/Colours.h"
#include "../include/Logger.h"

#include <stdexcept>
#include <cmath>
#include <cstdlib>
#include <vector>
#include <iostream>

static const std::string FILE_NAME = "Renderer.cpp";

Renderer::Renderer()
    : m_window(nullptr),
      m_renderer(nullptr),
      m_backBuffer(nullptr),
      m_font(nullptr),
      m_fontPath(""),
      m_fontSize(32),
      m_videoInitialized(false) {
    Logger::debug(FILE_NAME, "Renderer::Renderer", "Renderer constructor - initializing");
}

Renderer::~Renderer() {
    Logger::debug(FILE_NAME, "Renderer::~Renderer", "Renderer destructor - cleaning up resources");

    if (m_font) {
        TTF_CloseFont(m_font);
        m_font = nullptr;
    }

    if (m_backBuffer) {
        SDL_DestroyTexture(m_backBuffer);
        m_backBuffer = nullptr;
    }

    if (m_renderer) {
        SDL_DestroyRenderer(m_renderer);
        m_renderer = nullptr;
    }

    if (m_window) {
        SDL_DestroyWindow(m_window);
        m_window = nullptr;
    }

    if (TTF_WasInit()) {
        TTF_Quit();
    }

    if (m_videoInitialized) {
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
    }

    Logger::info(FILE_NAME, "Renderer::~Renderer", "Renderer resources cleaned up successfully");
}

bool Renderer::init(const std::string& title) {
    Logger::info(FILE_NAME, "Renderer::init", "Initializing Renderer with title: " + title);

    if (SDL_WasInit(0) == 0) {
        if (SDL_Init(SDL_INIT_VIDEO) != 0) {
            Logger::critical(FILE_NAME, "Renderer::init", "FAILED: SDL_Init - " + std::string(SDL_GetError()));
            return false;
        }
    } else if (SDL_WasInit(SDL_INIT_VIDEO) == 0) {
        if (SDL_InitSubSystem(SDL_INIT_VIDEO) != 0) {
            Logger::critical(FILE_NAME, "Renderer::init", "FAILED: SDL_InitSubSystem - " + std::string(SDL_GetError()));
            return false;
        }
    }

    if (SDL_WasInit(SDL_INIT_VIDEO) == 0) {
        Logger::critical(FILE_NAME, "Renderer::init", "FAILED: SDL video subsystem not available");
        return false;
    }

    m_videoInitialized = true;

    if (TTF_WasInit() == 0 && TTF_Init() != 0) {
        Logger::critical(FILE_NAME, "Renderer::init", "FAILED: TTF_Init - " + std::string(TTF_GetError()));
        return false;
    }

    m_window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    if (!m_window) {
        Logger::critical(FILE_NAME, "Renderer::init", "FAILED: SDL_CreateWindow - " + std::string(SDL_GetError()));
        return false;
    }

    m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!m_renderer) {
        Logger::critical(FILE_NAME, "Renderer::init", "FAILED: SDL_CreateRenderer - " + std::string(SDL_GetError()));
        return false;
    }

    m_backBuffer = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, WIDTH, HEIGHT);
    if (!m_backBuffer) {
        Logger::critical(FILE_NAME, "Renderer::init", "FAILED: SDL_CreateTexture - " + std::string(SDL_GetError()));
        return false;
    }

    SDL_SetRenderTarget(m_renderer, m_backBuffer);
    SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);

    m_fontPath = "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf";
    if (!loadFont()) {
        Logger::warning(FILE_NAME, "Renderer::init", "Default font not loaded; text rendering disabled");
    }

    Logger::info(FILE_NAME, "Renderer::init", "Renderer initialized successfully - Window size: " + 
                std::to_string(WIDTH) + "x" + std::to_string(HEIGHT));
    return true;
}

SDL_Color Renderer::allocColor(int r, int g, int b) {
    Logger::debug(FILE_NAME, "Renderer::allocColor", "Allocating color RGB(" + std::to_string(r) + ", " + 
                 std::to_string(g) + ", " + std::to_string(b) + ")");
    SDL_Color color{static_cast<Uint8>(r), static_cast<Uint8>(g), static_cast<Uint8>(b), 255};
    return color;
}

void Renderer::clear() {
    SDL_SetRenderTarget(m_renderer, m_backBuffer);
    SDL_SetRenderDrawColor(m_renderer, Colours::blackColor.r, Colours::blackColor.g, Colours::blackColor.b, Colours::blackColor.a);
    SDL_RenderClear(m_renderer);
}

// Algoritmo de Bresenham para círculo
// Dibuja los 8 octantes simétricamente -> muy eficiente, sin trigonometria
void Renderer::drawCirclePoints(int cx, int cy, int px, int py) {
    SDL_RenderDrawPoint(m_renderer, cx + px, cy + py);
    SDL_RenderDrawPoint(m_renderer, cx - px, cy + py);
    SDL_RenderDrawPoint(m_renderer, cx + px, cy - py);
    SDL_RenderDrawPoint(m_renderer, cx - px, cy - py);
    SDL_RenderDrawPoint(m_renderer, cx + py, cy + px);
    SDL_RenderDrawPoint(m_renderer, cx - py, cy + px);
    SDL_RenderDrawPoint(m_renderer, cx + py, cy - px);
    SDL_RenderDrawPoint(m_renderer, cx - py, cy - px);
}

void Renderer::drawCircle(Vec2 center, float radius, SDL_Color color) {
    SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);

    int cx = static_cast<int>(center.x);
    int cy = static_cast<int>(center.y);
    int r  = static_cast<int>(radius);
    int x  = 0, y = r;
    int d  = 3 - 2 * r; // decisor de Bresenham

    while (x <= y) {
        drawCirclePoints(cx, cy, x, y);
        if (d < 0) {
            d += 4 * x + 6;
        } else {
            d += 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
}

void Renderer::drawLine(Vec2 start, Vec2 end, SDL_Color color) {
    SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawLine(m_renderer, static_cast<int>(start.x), static_cast<int>(start.y),
                       static_cast<int>(end.x), static_cast<int>(end.y));
}

void Renderer::drawFilledCircle(Vec2 center, float radius, SDL_Color color) {
    SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);

    int cx = static_cast<int>(center.x);
    int cy = static_cast<int>(center.y);
    int r = static_cast<int>(radius);

    for (int dy = -r; dy <= r; ++dy) {
        int dx = static_cast<int>(std::sqrt((r * r) - (dy * dy)));
        SDL_RenderDrawLine(m_renderer, cx - dx, cy + dy, cx + dx, cy + dy);
    }
}

void Renderer::drawBall(const Ball& ball) {
    Logger::debug(FILE_NAME, "Renderer::drawBall", "Drawing ball id=" + std::to_string(ball.getId()) + 
                 " at position (" + std::to_string(ball.getPosition().x) + ", " + 
                 std::to_string(ball.getPosition().y) + ")");
    drawFilledCircle(ball.getPosition(), ball.getRadius(), ball.getColor());
    ballsMap.emplace(ball.getId(), ball);
}

void Renderer::eraseBall(const Ball& ball) {
    if (ballsMap.find(ball.getId()) != ballsMap.end()) {
        drawFilledCircle(ball.getPosition(), ball.getRadius(), Colours::blackColor);
    }
}

void Renderer::updateBalls() {
    for (auto& [id, ball] : ballsMap) {
        eraseBall(ball); // Borrar la bola antes de moverla
        ball.move();
        drawBall(ball);
    }
}

void Renderer::present() {
    SDL_SetRenderTarget(m_renderer, nullptr);
    SDL_RenderCopy(m_renderer, m_backBuffer, nullptr, nullptr);
    SDL_RenderPresent(m_renderer);
    SDL_SetRenderTarget(m_renderer, m_backBuffer);
}

bool Renderer::pollEvents() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            Logger::info(FILE_NAME, "Renderer::pollEvents", "Window close detected - exiting application");
            return false;
        }
        if (e.type == SDL_KEYDOWN) {
            Logger::info(FILE_NAME, "Renderer::pollEvents", "Key press detected - exiting application");
            return false; // cualquier tecla = salir
        }
    }
    return true;
}

void Renderer::showMessage(const std::string& message) {
    Logger::debug(FILE_NAME, "Renderer::showMessage", "Displaying message: " + message);

    if (!m_font) {
        Logger::warning(FILE_NAME, "Renderer::showMessage", "Font not loaded - cannot render text");
        return;
    }

    SDL_SetRenderTarget(m_renderer, m_backBuffer);

    std::vector<std::string> lines;
    size_t pos = 0;
    while (pos < message.length()) {
        size_t newlinePos = message.find('\n', pos);
        if (newlinePos == std::string::npos) {
            newlinePos = message.length();
        }
        lines.emplace_back(message.substr(pos, newlinePos - pos));
        pos = newlinePos + 1;
    }

    if (lines.empty()) {
        return;
    }

    int lineSkip = TTF_FontLineSkip(m_font);
    int totalHeight = lineSkip * static_cast<int>(lines.size());
    int startY = (HEIGHT - totalHeight) / 2;

    SDL_Color textColor = Colours::whiteColor;

    for (size_t i = 0; i < lines.size(); ++i) {
        SDL_Surface* surface = TTF_RenderUTF8_Blended(m_font, lines[i].c_str(), textColor);
        if (!surface) {
            Logger::warning(FILE_NAME, "Renderer::showMessage", "Failed to render text surface");
            continue;
        }

        SDL_Texture* texture = SDL_CreateTextureFromSurface(m_renderer, surface);
        if (!texture) {
            Logger::warning(FILE_NAME, "Renderer::showMessage", "Failed to create text texture");
            SDL_FreeSurface(surface);
            continue;
        }

        SDL_Rect dst{};
        dst.w = surface->w;
        dst.h = surface->h;
        dst.x = (WIDTH - dst.w) / 2;
        dst.y = startY + static_cast<int>(i) * lineSkip;

        SDL_FreeSurface(surface);
        SDL_RenderCopy(m_renderer, texture, nullptr, &dst);
        SDL_DestroyTexture(texture);
    }

    present();
}

void Renderer::setFontSize(int size) {
    if (size <= 0) {
        Logger::warning(FILE_NAME, "Renderer::setFontSize", "Invalid font size requested");
        return;
    }

    m_fontSize = size;
    if (!loadFont()) {
        Logger::warning(FILE_NAME, "Renderer::setFontSize", "Failed to load font at new size");
    }
}

void Renderer::setFont(const std::string& fontName) {
    m_fontPath = fontName;
    if (!loadFont()) {
        Logger::warning(FILE_NAME, "Renderer::setFont", "Failed to load font: " + fontName);
    }
}

void Renderer::setScreenColor(SDL_Color color) {
    SDL_SetRenderTarget(m_renderer, m_backBuffer);
    SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);
    SDL_RenderClear(m_renderer);
    present();
}

uint8_t* Renderer::captureFrame(int& outSize) {
    Logger::debug(FILE_NAME, "Renderer::captureFrame", "Capturing frame from backbuffer");

    int stride = ((WIDTH * 4 + 31) / 32) * 32; // Align to 32 bytes
    outSize = stride * HEIGHT;

    uint8_t* buffer = nullptr;
    if (posix_memalign((void**)&buffer, 32, outSize) != 0) {
        Logger::error(FILE_NAME, "Renderer::captureFrame", "Failed to allocate aligned memory");
        outSize = 0;
        return nullptr;
    }

    SDL_SetRenderTarget(m_renderer, m_backBuffer);
    if (SDL_RenderReadPixels(m_renderer, nullptr, SDL_PIXELFORMAT_RGBA8888, buffer, stride) != 0) {
        Logger::error(FILE_NAME, "Renderer::captureFrame", "Failed to capture image: " + std::string(SDL_GetError()));
        free(buffer);
        outSize = 0;
        return nullptr;
    }

    return buffer;
}

void Renderer::releaseFrameBuffer(uint8_t* buffer) {
    if (buffer) {
        free(buffer);  // Use free() for posix_memalign allocated memory
    }
}

bool Renderer::loadFont() {
    if (m_font) {
        TTF_CloseFont(m_font);
        m_font = nullptr;
    }

    if (m_fontPath.empty() || m_fontSize <= 0) {
        Logger::warning(FILE_NAME, "Renderer::loadFont", "Font path or size not set");
        return false;
    }

    m_font = TTF_OpenFont(m_fontPath.c_str(), m_fontSize);
    if (!m_font) {
        Logger::warning(FILE_NAME, "Renderer::loadFont", "Failed to load font: " + m_fontPath + " - " + std::string(TTF_GetError()));
        return false;
    }

    return true;
}