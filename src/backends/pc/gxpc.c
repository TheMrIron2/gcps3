#include "../../gx/gx_internal.h"

#include "gcps3/log.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

/*
 * gxpc is a development-only SDL2/OpenGL debug visualizer for early GX frontend
 * validation. It is not the final renderer and deliberately ignores textures,
 * TEV, and platform-specific rendering paths.
 */
#define GCPS3_GXPC_PRESENT_MS 1200u

static SDL_Window *s_window;
static SDL_GLContext s_gl_context;
static GXColor s_clear_color;
static int s_sdl_initialized;

static float color_channel(uint8_t channel)
{
    return (float)channel / 255.0f;
}

static void pump_events(void)
{
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
    }
}

static void apply_viewport(const GXViewport *viewport)
{
    glViewport((GLint)viewport->x, (GLint)viewport->y, (GLsizei)viewport->width, (GLsizei)viewport->height);
}

static void apply_clear_color(GXColor color)
{
    glClearColor(
        color_channel(color.r),
        color_channel(color.g),
        color_channel(color.b),
        color_channel(color.a));
}

static void transform_position(const Gcps3GXDrawPacket *packet, const Gcps3GXVertex *vertex, float *x, float *y, float *z)
{
    const float(*mtx)[4] = packet->current_matrix;

    *x = mtx[0][0] * vertex->x + mtx[0][1] * vertex->y + mtx[0][2] * vertex->z + mtx[0][3];
    *y = mtx[1][0] * vertex->x + mtx[1][1] * vertex->y + mtx[1][2] * vertex->z + mtx[1][3];
    *z = mtx[2][0] * vertex->x + mtx[2][1] * vertex->y + mtx[2][2] * vertex->z + mtx[2][3];
}

static int ensure_window(const Gcps3GXState *state)
{
    int width;
    int height;

    if (s_window && s_gl_context) {
        return 1;
    }

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        GCPS3_LOG_ERROR("gxpc", "SDL_Init failed: %s", SDL_GetError());
        return 0;
    }

    s_sdl_initialized = 1;
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

    width = state->viewport.width > 0.0f ? (int)state->viewport.width : 640;
    height = state->viewport.height > 0.0f ? (int)state->viewport.height : 480;

    s_window = SDL_CreateWindow(
        "gcps3 gxpc debug visualizer",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (!s_window) {
        GCPS3_LOG_ERROR("gxpc", "SDL_CreateWindow failed: %s", SDL_GetError());
        return 0;
    }

    s_gl_context = SDL_GL_CreateContext(s_window);
    if (!s_gl_context) {
        GCPS3_LOG_ERROR("gxpc", "SDL_GL_CreateContext failed: %s", SDL_GetError());
        return 0;
    }

    SDL_GL_SetSwapInterval(1);
    apply_viewport(&state->viewport);
    apply_clear_color(state->clear_color);

    GCPS3_LOG_INFO("gxpc", "debug visualizer window created (%dx%d)", width, height);
    return 1;
}

void gcps3_gx_backend_init(const Gcps3GXState *state)
{
    s_clear_color = state->clear_color;
    GCPS3_LOG_INFO("gxpc", "init debug visualizer backend");
    (void)ensure_window(state);
}

void gcps3_gx_backend_shutdown(const Gcps3GXState *state)
{
    (void)state;

    if (s_gl_context) {
        SDL_GL_DeleteContext(s_gl_context);
        s_gl_context = 0;
    }

    if (s_window) {
        SDL_DestroyWindow(s_window);
        s_window = 0;
    }

    if (s_sdl_initialized) {
        SDL_Quit();
        s_sdl_initialized = 0;
    }

    GCPS3_LOG_INFO("gxpc", "shutdown debug visualizer backend");
}

void gcps3_gx_backend_set_viewport(const GXViewport *viewport)
{
    if (!s_window) {
        return;
    }

    apply_viewport(viewport);
}

void gcps3_gx_backend_set_clear_color(GXColor color)
{
    s_clear_color = color;

    if (!s_window) {
        return;
    }

    apply_clear_color(color);
}

void gcps3_gx_backend_clear(const Gcps3GXState *state)
{
    if (!ensure_window(state)) {
        return;
    }

    apply_clear_color(s_clear_color);
    glClear(GL_COLOR_BUFFER_BIT);
    SDL_GL_SwapWindow(s_window);
    pump_events();
}

void gcps3_gx_backend_submit_draw_packet(const Gcps3GXDrawPacket *packet)
{
    unsigned int i;

    if (!s_window || packet->primitive != GCPS3_GX_PRIMITIVE_TRIANGLES) {
        return;
    }

    GCPS3_LOG_INFO("gxpc", "drawing triangle packet with %u vertices", packet->vertex_count);

    apply_clear_color(s_clear_color);
    glClear(GL_COLOR_BUFFER_BIT);

    glBegin(GL_TRIANGLES);
    for (i = 0; i < packet->vertex_count; i++) {
        const Gcps3GXVertex *vertex = &packet->vertices[i];
        float x;
        float y;
        float z;

        transform_position(packet, vertex, &x, &y, &z);
        glColor4ub(vertex->color.r, vertex->color.g, vertex->color.b, vertex->color.a);
        glVertex3f(x, y, z);
    }
    glEnd();

    SDL_GL_SwapWindow(s_window);
    pump_events();
    SDL_Delay(GCPS3_GXPC_PRESENT_MS);
}
