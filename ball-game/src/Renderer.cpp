#include "../include/Renderer.h"
#include "../include/Colours.h"

#include <X11/Xutil.h>
#include <stdexcept>
#include <cmath>
#include <vector>
#include <iostream>

Renderer::Renderer()
    : m_display(nullptr), m_window(0), m_gc(0), m_backBuffer(0) {}

Renderer::~Renderer() {
    if (m_display) {
        if (m_backBuffer) XFreePixmap(m_display, m_backBuffer);
        if (m_gc)         XFreeGC(m_display, m_gc);
        XDestroyWindow(m_display, m_window);
        XCloseDisplay(m_display);
    }
}

bool Renderer::init(const std::string& title) {
    m_display = XOpenDisplay(nullptr);
    if (!m_display) return false;

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

    XFlush(m_display);
    return true;
}

unsigned long Renderer::allocColor(int r, int g, int b) {
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

        if (e.type == KeyPress)          return false; // cualquier tecla = salir
        if (e.type == ClientMessage)     return false;
        if (e.type == DestroyNotify)     return false;
    }
    return true;
}