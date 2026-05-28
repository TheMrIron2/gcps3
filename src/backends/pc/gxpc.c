#include "../../gx/gx_internal.h"

#include "gcps3/log.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

/*
 * gxpc is a development-only SDL2/OpenGL debug visualizer for early GX frontend
 * validation. It is not the final renderer; its RGBA8 texture upload path is
 * only for inspecting early immediate-mode samples.
 */
#define GCPS3_GXPC_PRESENT_MS 1200u

static SDL_Window *s_window;
static SDL_GLContext s_gl_context;
static GXColor s_clear_color;
static int s_sdl_initialized;
static GLuint s_texture;

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

static int should_use_texture(const Gcps3GXDrawPacket *packet)
{
    return packet->descriptor.tex0 == GX_ATTR_DIRECT && packet->texture.bound && packet->texture.rgba8_pixels;
}

static int upload_texture_for_draw(const Gcps3GXDrawPacket *packet)
{
    if (!should_use_texture(packet)) {
        return 0;
    }

    if (s_texture == 0u) {
        glGenTextures(1, &s_texture);
    }

    glBindTexture(GL_TEXTURE_2D, s_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        (GLsizei)packet->texture.width,
        (GLsizei)packet->texture.height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        packet->texture.rgba8_pixels);
    return 1;
}

/*
 * Apply the captured 3x4 position matrix as a local visualizer transform.
 * This intentionally does not mutate packet vertices and should not be treated
 * as the final GX transform pipeline.
 */
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
        if (s_texture != 0u) {
            glDeleteTextures(1, &s_texture);
            s_texture = 0u;
        }

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
    int use_texture;

    if (!s_window || packet->primitive != GCPS3_GX_PRIMITIVE_TRIANGLES) {
        return;
    }

    GCPS3_LOG_INFO("gxpc", "drawing triangle packet with %u vertices", packet->vertex_count);

    apply_clear_color(s_clear_color);
    glClear(GL_COLOR_BUFFER_BIT);
    use_texture = upload_texture_for_draw(packet);

    if (use_texture) {
        glEnable(GL_TEXTURE_2D);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    } else {
        glDisable(GL_TEXTURE_2D);
    }

    /*
     * Temporary visualizer path: vertices are treated as already suitable for
     * the current OpenGL debug coordinate convention after matrix application.
     * Do not treat this as final GX projection or viewport behaviour.
     */
    glBegin(GL_TRIANGLES);
    for (i = 0; i < packet->vertex_count; i++) {
        const Gcps3GXVertex *vertex = &packet->vertices[i];
        float x;
        float y;
        float z;

        transform_position(packet, vertex, &x, &y, &z);
        glColor4ub(vertex->color.r, vertex->color.g, vertex->color.b, vertex->color.a);
        if (use_texture) {
            glTexCoord2f(vertex->s, vertex->t);
        }
        glVertex3f(x, y, z);
    }
    glEnd();
    glDisable(GL_TEXTURE_2D);

    SDL_GL_SwapWindow(s_window);
    pump_events();
    SDL_Delay(GCPS3_GXPC_PRESENT_MS);
}
