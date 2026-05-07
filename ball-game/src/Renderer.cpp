#include "../include/Renderer.h"
#include "../include/Colours.h"
#include "../include/Logger.h"

#include <X11/Xutil.h>
#include <stdexcept>
#include <cmath>
#include <cstdlib>
#include <vector>
#include <iostream>

static const std::string FILE_NAME = "Renderer.cpp";

Renderer::Renderer()
    : m_display(nullptr), m_window(0), m_gc(0), m_backBuffer(0) {
    Logger::debug(FILE_NAME, "Renderer::Renderer", "Renderer constructor - initializing");
}

Renderer::~Renderer() {
    Logger::debug(FILE_NAME, "Renderer::~Renderer", "Renderer destructor - cleaning up resources");
    if (m_display) {
        if (m_backBuffer) XFreePixmap(m_display, m_backBuffer);
        if (m_gc)         XFreeGC(m_display, m_gc);
        XDestroyWindow(m_display, m_window);
        XCloseDisplay(m_display);
        Logger::info(FILE_NAME, "Renderer::~Renderer", "Renderer resources cleaned up successfully");
    }
}

bool Renderer::init(const std::string& title) {
    Logger::info(FILE_NAME, "Renderer::init", "Initializing Renderer with title: " + title);
    
    m_display = XOpenDisplay(nullptr);
    if (!m_display) {
        Logger::critical(FILE_NAME, "Renderer::init", "FAILED: Could not open X Display");
        return false;
    }

    int screen = DefaultScreen(m_display);

    // Ventana con fondo negro
    m_window = XCreateSimpleWindow(
        m_display, RootWindow(m_display, screen),
        0, 0, WIDTH, HEIGHT, 0,
        BlackPixel(m_display, screen),
        BlackPixel(m_display, screen)
    );

    XStoreName(m_display, m_window, title.c_str());

    // Solo escuchamos estos eventos
    XSelectInput(m_display, m_window, ExposureMask | KeyPressMask | StructureNotifyMask);
    XMapWindow(m_display, m_window);

    // Graphics Context
    m_gc = XCreateGC(m_display, m_window, 0, nullptr);

    // Back buffer (doble buffer manual)
    m_backBuffer = XCreatePixmap(m_display, m_window, WIDTH, HEIGHT,
                                 DefaultDepth(m_display, screen));

    // Set a beautiful font and large size
    setFont("-*-lucida-bright-*-r-*-*-500-*-*-*-*-*-iso8859-1");

    XFlush(m_display);
    Logger::info(FILE_NAME, "Renderer::init", "Renderer initialized successfully - Window size: " + 
                std::to_string(WIDTH) + "x" + std::to_string(HEIGHT));
    return true;
}

unsigned long Renderer::allocColor(int r, int g, int b) {
    Logger::debug(FILE_NAME, "Renderer::allocColor", "Allocating color RGB(" + std::to_string(r) + ", " + 
                 std::to_string(g) + ", " + std::to_string(b) + ")");
    XColor color;
    color.red   = r * 257; // X11 usa rango 0-65535
    color.green = g * 257;
    color.blue  = b * 257;
    color.flags = DoRed | DoGreen | DoBlue;
    XAllocColor(m_display, DefaultColormap(m_display, DefaultScreen(m_display)), &color);
    return color.pixel;
}

void Renderer::clear() {
    XSetForeground(m_display, m_gc, Colours::blackColor.pixel);
    XFillRectangle(m_display, m_backBuffer, m_gc, 0, 0, WIDTH, HEIGHT);
}

// Algoritmo de Bresenham para círculo
// Dibuja los 8 octantes simétricamente → muy eficiente, sin trigonometría
void Renderer::drawCirclePoints(int cx, int cy, int px, int py) {
    XDrawPoint(m_display, m_backBuffer, m_gc, cx + px, cy + py);
    XDrawPoint(m_display, m_backBuffer, m_gc, cx - px, cy + py);
    XDrawPoint(m_display, m_backBuffer, m_gc, cx + px, cy - py);
    XDrawPoint(m_display, m_backBuffer, m_gc, cx - px, cy - py);
    XDrawPoint(m_display, m_backBuffer, m_gc, cx + py, cy + px);
    XDrawPoint(m_display, m_backBuffer, m_gc, cx - py, cy + px);
    XDrawPoint(m_display, m_backBuffer, m_gc, cx + py, cy - px);
    XDrawPoint(m_display, m_backBuffer, m_gc, cx - py, cy - px);
}

void Renderer::drawCircle(Vec2 center, float radius, XColor color) {
    XSetForeground(m_display, m_gc, color.pixel);

    int cx = (int)center.x;
    int cy = (int)center.y;
    int r  = (int)radius;
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

void Renderer::drawLine(Vec2 start, Vec2 end, XColor color) {
    XSetForeground(m_display, m_gc, color.pixel);
    XDrawLine(m_display, m_backBuffer, m_gc,
              (int)start.x, (int)start.y,
              (int)end.x, (int)end.y);
}

void Renderer::drawFilledCircle(Vec2 center, float radius, XColor color) {
    XSetForeground(m_display, m_gc, color.pixel);
    int x = (int)(center.x - radius);
    int y = (int)(center.y - radius);
    int d = (int)(radius * 2);
    XFillArc(m_display, m_backBuffer, m_gc, x, y, d, d, 0, 360 * 64);
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
    //clear(); // Limpiar el fondo antes de copiar el back buffer
    XCopyArea(m_display, m_backBuffer, m_window, m_gc, 0, 0, WIDTH, HEIGHT, 0, 0);
    XFlush(m_display);
}

bool Renderer::pollEvents() {
    while (XPending(m_display)) {
        XEvent e;
        XNextEvent(m_display, &e);

        if (e.type == KeyPress) {
            Logger::info(FILE_NAME, "Renderer::pollEvents", "Key press detected - exiting application");
            return false; // cualquier tecla = salir
        }
        if (e.type == ClientMessage) {
            Logger::info(FILE_NAME, "Renderer::pollEvents", "Client message event - exiting application");
            return false;
        }
        if (e.type == DestroyNotify) {
            Logger::info(FILE_NAME, "Renderer::pollEvents", "Window destroy event - exiting application");
            return false;
        }
    }
    return true;
}

void Renderer::showMessage(const std::string& message) {
    Logger::debug(FILE_NAME, "Renderer::showMessage", "Displaying message: " + message);
    XSetForeground(m_display, m_gc, Colours::whiteColor.pixel);
    
    // Handle multiline messages by parsing newlines
    int lineHeight = 30;
    int startY = HEIGHT / 2 - 30;
    int lineNum = 0;
    size_t pos = 0;
    
    while (pos < message.length()) {
        size_t newlinePos = message.find('\n', pos);
        if (newlinePos == std::string::npos) {
            newlinePos = message.length();
        }
        
        std::string line = message.substr(pos, newlinePos - pos);
        int y = startY + (lineNum * lineHeight);
        
        // Center the text horizontally
        int textWidth = line.length() * 8; // Approximate character width
        int x = (WIDTH - textWidth) / 2;
        
        XDrawString(m_display, m_backBuffer, m_gc, x, y, line.c_str(), line.length());
        
        lineNum++;
        pos = newlinePos + 1;
    }
    
    present();
}

void Renderer::setFontSize(int size) {
    XFontStruct* font = XLoadQueryFont(m_display, ("-*-helvetica-bold-r-normal--" + std::to_string(size) + "-*" + "-*-*-*-*-iso8859-1").c_str());
    if (!font) {
        Logger::warning(FILE_NAME, "Renderer::setFontSize", "Failed to load font. Falling back to default.");
        return;
    }
    XSetFont(m_display, m_gc, font->fid);
}

void Renderer::setFont(const std::string& fontName) {
    XFontStruct* font = XLoadQueryFont(m_display, fontName.c_str());
    if (!font) {
        Logger::warning(FILE_NAME, "Renderer::setFont", "Failed to load font: " + fontName);
        return;
    }
    XSetFont(m_display, m_gc, font->fid);
}

void Renderer::setScreenColor(XColor color) {
    // Create a semi-transparent overlay
    XSetForeground(m_display, m_gc, color.pixel);
    XFillRectangle(m_display, m_backBuffer, m_gc, 0, 0, WIDTH, HEIGHT);
    XCopyArea(m_display, m_backBuffer, m_window, m_gc, 0, 0, WIDTH, HEIGHT, 0, 0);
    XFlush(m_display);
}

uint8_t* Renderer::captureFrame(int& outSize) {
    Logger::debug(FILE_NAME, "Renderer::captureFrame", "Capturing frame from backbuffer");
    
    XImage* image = XGetImage(m_display, m_backBuffer, 0, 0, WIDTH, HEIGHT, AllPlanes, ZPixmap);
    if (!image) {
        Logger::error(FILE_NAME, "Renderer::captureFrame", "Failed to capture image from backbuffer");
        outSize = 0;
        return nullptr;
    }

    // Allocate buffer aligned to 32 bytes for SIMD optimization (FFmpeg requirement)
    // Expected size: WIDTH * HEIGHT * 4 bytes (RGBA)
    int stride = ((WIDTH * 4 + 31) / 32) * 32; // Align to 32 bytes
    outSize = stride * HEIGHT;
    
    uint8_t* buffer = nullptr;
    if (posix_memalign((void**)&buffer, 32, outSize) != 0) {
        Logger::error(FILE_NAME, "Renderer::captureFrame", "Failed to allocate aligned memory");
        XDestroyImage(image);
        return nullptr;
    }

    // Copy pixel data from XImage to buffer
    // XImage data layout depends on the server - we need to convert to RGB32
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            unsigned long pixel = XGetPixel(image, x, y);
            
            // Extract RGB components (assuming 24-bit or 32-bit color)
            uint8_t r = (pixel >> 16) & 0xFF;
            uint8_t g = (pixel >> 8) & 0xFF;
            uint8_t b = pixel & 0xFF;
            
            int idx = (y * stride) + (x * 4);
            buffer[idx + 0] = r;
            buffer[idx + 1] = g;
            buffer[idx + 2] = b;
            buffer[idx + 3] = 0xFF; // Alpha channel (unused)
        }
    }

    XDestroyImage(image);
    return buffer;
}

void Renderer::releaseFrameBuffer(uint8_t* buffer) {
    if (buffer) {
        free(buffer);  // Use free() for posix_memalign allocated memory
    }
}