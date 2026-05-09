# Plan de Migración: X11 → SDL2

## 📋 Resumen Ejecutivo

Migración del sistema gráfico del proyecto `ball-game` de **X11** a **SDL2** para solucionar limitaciones de X11 (video recording, renderizado de texto, transparencia/canal alpha) y mejorar la portabilidad multiplataforma.

**Duración estimada:** 2-3 semanas  
**Complejidad:** Media-Alta  
**Impacto:** Significativo - cambio de toda la capa de renderizado

---

## 🎯 Objetivos

### Primarios
- ✅ Eliminar dependencias de X11
- ✅ Habilitar grabación de video nativa
- ✅ Soporte para renderizado de texto mejorado
- ✅ Soporte transparencia (canal alpha)

### Secundarios
- ✅ Mejorar portabilidad a otros sistemas (macOS, Windows)
- ✅ Simplificar el código de renderizado
- ✅ Mejor mantenibilidad a largo plazo

---

## 📊 Análisis del Estado Actual

### Archivos afectados por X11

| Archivo | Líneas X11 | Criticidad |
|---------|-----------|-----------|
| `include/Renderer.h` | ~40 | CRÍTICA |
| `src/Renderer.cpp` | ~250+ | CRÍTICA |
| `src/main.cpp` | Variable | ALTA |
| `include/Colours.h` | XColor | MEDIA |
| `src/Colours.cpp` | Conversión XColor | MEDIA |
| `src/VideoRecorder.cpp` | Captura X11 | CRÍTICA |

### Dependencias X11 identificadas

```
- X11/Xlib.h       → Display, Window, GC
- X11/Xutil.h      → XColor, primitivos gráficos
- libX11           → Sistema de eventos
- Pixmaps X11      → Back-buffer
```

---

## 🔄 Fases de Migración

### **Fase 1: Investigación y Prototipado (2-3 días)**

**Tareas:**
- [ ] Instalar y validar SDL2 en el entorno
- [ ] Crear proyecto de prueba SDL2 básico
- [ ] Documentar equivalencias X11 ↔ SDL2
- [ ] Evaluar bibliotecas complementarias (SDL_ttf, SDL_image)

**Dependencias a instalar:**
```bash
libsdl2-dev
libsdl2-ttf-dev      # Renderizado de texto
libsdl2-image-dev    # Carga de imágenes
```

**Deliverables:**
- [ ] Documento de equivalencias
- [ ] Proyecto prototipo funcional

---

### **Fase 2: Refactorización de Renderer (5-7 días)**

**Subtareas 2.1: Clase Renderer - Inicialización**
- [ ] Reemplazar `XOpenDisplay()` → `SDL_CreateWindow()`
- [ ] Reemplazar `XCreateSimpleWindow()` → integración nativa SDL
- [ ] Migrar sistema de eventos X11 → `SDL_PollEvent()`
- [ ] Adaptar constructor/destructor

**Subtareas 2.2: Sistema de Colores**
- [ ] Convertir `XColor` → `SDL_Color` (RGBA)
- [ ] Actualizar `allocColor()` para SDL2
- [ ] Refactorizar archivo [Colours.h](../../../ball-game/include/Colours.h)

**Subtareas 2.3: Primitivos Gráficos**
- [ ] Migrar `drawCircle()` → SDL2 (usar SDL_gfx o algoritmo Bresenham)
- [ ] Migrar `drawLine()` → SDL2
- [ ] Migrar `drawFilledCircle()` → SDL2
- [ ] Implementar blending para canal alpha

**Subtareas 2.4: Back-buffer y Presentación**
- [ ] Reemplazar `Pixmap` X11 → `SDL_Texture`
- [ ] Adaptar `clear()` → `SDL_RenderClear()`
- [ ] Adaptar `present()` → `SDL_RenderPresent()`

---

### **Fase 3: Renderizado de Texto (3-4 días)**

**Tareas:**
- [ ] Integrar SDL_ttf para fuentes
- [ ] Crear wrapper `TextRenderer` si es necesario
- [ ] Implementar `setFont()` y `setFontSize()`
- [ ] Validar `showMessage()` con nueva implementación

**Recursos:**
- Font por defecto: DejaVuSans-Regular.ttf (incluir en assets)

---

### **Fase 4: Grabación de Video (3-4 días)**

**Tareas:**
- [ ] Reemplazar captura X11 en `VideoRecorder.cpp`
- [ ] Implementar captura desde SDL2 texture
- [ ] Validar encoding de video (usar FFmpeg si es necesario)
- [ ] Pruebas de grabación

**Consideraciones:**
- SDL2 proporciona acceso directo a píxeles renderizados
- Simplificar significativamente respecto a método X11 actual

---

### **Fase 5: Eventos e Input (2-3 días)**

**Tareas:**
- [ ] Migrar `pollEvents()` de XEvent → SDL_Event
- [ ] Mapear eventos del teclado/ratón
- [ ] Implementar quit/close logic
- [ ] Validar responsividad

---

### **Fase 6: Pruebas Integrales (3-4 días)**

**Test suites:**
- [ ] Renderizado de bolas
- [ ] Colisiones visuales
- [ ] Grabación de video
- [ ] Renderizado de texto
- [ ] Eventos de entrada
- [ ] Performance (FPS)
- [ ] Portabilidad en sistemas Linux

**Criterios de aceptación:**
- FPS ≥ 60
- Sin memory leaks
- Video grabado válido (resolución, FPS, codec)
- Texto renderizado legible

---

## 🔀 Mapeo X11 → SDL2

| X11 | SDL2 | Notas |
|-----|------|-------|
| `Display*` | `SDL_Window*` | Contexto de renderizado |
| `Window` | `SDL_Renderer*` | Renderer |
| `XColor` | `SDL_Color` | Incluye alpha channel |
| `Pixmap` | `SDL_Texture` | Off-screen buffer |
| `GC` | Integrado en renderer | No necesario explícitamente |
| `XCreateSimpleWindow` | `SDL_CreateWindow` | Crear ventana |
| `XDrawLine` | `SDL_RenderDrawLine` | Dibujar línea |
| `XFillArc` | Manual + SDL_gfx | Círculo relleno |
| `XEvent` | `SDL_Event` | Sistema de eventos |
| `XPutImage` | `SDL_RenderCopy` | Presentar buffer |

---

## 📦 Cambios en Dependencias

### Remover
```makefile
# Remover de LDFLAGS
-lX11
-lXext
```

### Agregar
```makefile
# Agregar a LDFLAGS
-lSDL2
-lSDL2_ttf
-lSDL2_image
```

**Actualizar Makefile:**
```makefile
# Buscar compilación
CXXFLAGS += $(shell pkg-config --cflags sdl2)
LDFLAGS += $(shell pkg-config --libs sdl2)
LDFLAGS += $(shell pkg-config --libs SDL2_ttf)
LDFLAGS += $(shell pkg-config --libs SDL2_image)
```

---

## ⚠️ Riesgos y Mitigación

| Riesgo | Probabilidad | Impacto | Mitigación |
|--------|-------------|--------|-----------|
| Regresión de performance | MEDIA | ALTO | Profiling frecuente, comparativas X11 |
| Incompatibilidad gráficos Intel | BAJA | MEDIO | Testing en múltiples GPU |
| Cambio API impacta código del juego | MEDIA | MEDIO | Mantener interfaz pública consistente |
| Memory leaks en SDL | BAJA | ALTO | Valgrind, asan, testing intensivo |
| Dependencias SDL no disponibles | MUY BAJA | BAJO | Distribuir statically si es necesario |

---

## 📝 Tareas Específicas

### Cambios Críticos en Headers

**[include/Renderer.h](../../../ball-game/include/Renderer.h)**
```diff
- #include <X11/Xlib.h>
+ #include <SDL2/SDL.h>
+ #include <SDL2/SDL_ttf.h>
+ #include <SDL2/SDL_gfx.h>

- Display*    m_display;
- Window      m_window;
- GC          m_gc;
- Pixmap      m_backBuffer;

+ SDL_Window*      m_window;
+ SDL_Renderer*    m_renderer;
+ SDL_Texture*     m_backBuffer;
+ TTF_Font*        m_font;
```

### Cambios en Colours

**[include/Colours.h](../../../ball-game/include/Colours.h)**
```diff
- #include <X11/Xutil.h>
+ #include <SDL2/SDL.h>
  
- // Usar XColor
+ // Usar SDL_Color (r, g, b, a)
```

---

## 🚀 Estrategia de Implementación

### Opción A: Big Bang (Riesgo Mayor)
- Reemplazar todo de una vez
- Ventaja: Rápido si todo funciona
- Desventaja: Difícil de debuggear, sin punto de recuperación

### Opción B: Gradual (Recomendado) ✅
1. Implementar SDL2 sin remover X11 (feature flag)
2. Cambiar main.cpp para usar SDL2
3. Validar cada módulo
4. Remover X11 gradualmente
5. Limpiar código

**Implementación:**
```cpp
#define USE_SDL2  // Feature flag en Config.h
```

---

## 📅 Timeline

```
Semana 1:
  ├─ Lunes-Martes: Fase 1 (Investigación)
  ├─ Miércoles-Viernes: Fase 2.1-2.2 (Renderer base)

Semana 2:
  ├─ Lunes-Martes: Fase 2.3-2.4 (Gráficos)
  ├─ Miércoles-Viernes: Fase 3 (Texto)

Semana 3:
  ├─ Lunes-Martes: Fase 4 (Video)
  ├─ Miércoles: Fase 5 (Eventos)
  └─ Jueves-Viernes: Fase 6 (Testing)
```

---

## ✅ Checklist de Validación Final

- [ ] Compilación limpia sin warnings
- [ ] Aplicación se inicia correctamente
- [ ] FPS ≥ 60 sostenido
- [ ] No hay memory leaks (valgrind clean)
- [ ] Video se graba correctamente (formato validado)
- [ ] Texto se renderiza legible
- [ ] Colisiones funcionan
- [ ] Evento de cierre funciona
- [ ] Makefile actualizado
- [ ] Documentación actualizada
- [ ] README.md menciona requisitos SDL2

---

## 📚 Referencias y Recursos

- [SDL2 Documentation](https://wiki.libsdl.org/SDL2/CategoryAPI)
- [SDL2 TTF Tutorial](https://wiki.libsdl.org/SDL_ttf/CategoryAPI)
- [SDL_gfx Library](https://sourceforge.net/projects/sdlgfx/)
- [Equivalencias X11-SDL2](https://www.libsdl.org/faq.php)

---

## 🤝 Responsabilidades

| Fase | Responsable | Revisión |
|------|------------|----------|
| Todas | @developer | Code review post-fase |
| Testing | QA | Validación funcional |
| Deploy | DevOps | Release en main |

---

**Última actualización:** May 9, 2026  
**Estado:** PLANIFICADO  
**Prioridad:** ALTA
