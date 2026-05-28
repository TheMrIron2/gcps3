#include "gcps3/gx.h"

#include "gcps3/log.h"

#include "gx_internal.h"

static void gx_warn_uninitialized(const char *call_name)
{
    GCPS3_LOG_WARN("gx", "%s called before GXInit", call_name);
}

void GXInit(void)
{
    Gcps3GXState *state = gcps3_gx_state();

    if (state->initialized) {
        GCPS3_LOG_WARN("gx", "GXInit called more than once");
        return;
    }

    gcps3_gx_state_reset();
    state = gcps3_gx_state();
    state->initialized = 1;

    GCPS3_LOG_INFO("gx", "GX frontend initialized");
    gcps3_gx_backend_init(state);
}

void GXShutdown(void)
{
    Gcps3GXState *state = gcps3_gx_state();

    if (!state->initialized) {
        GCPS3_LOG_WARN("gx", "GXShutdown called before GXInit");
        return;
    }

    gcps3_gx_backend_shutdown(state);
    state->initialized = 0;
    GCPS3_LOG_INFO("gx", "GX frontend shut down");
}

void GXSetViewport(float x, float y, float width, float height, float near_z, float far_z)
{
    Gcps3GXState *state = gcps3_gx_state();

    state->viewport.x = x;
    state->viewport.y = y;
    state->viewport.width = width;
    state->viewport.height = height;
    state->viewport.near_z = near_z;
    state->viewport.far_z = far_z;

    if (!state->initialized) {
        GCPS3_LOG_WARN("gx", "GXSetViewport called before GXInit; state recorded only");
        return;
    }

    gcps3_gx_backend_set_viewport(&state->viewport);
}

void GXSetClearColor(GXColor color)
{
    Gcps3GXState *state = gcps3_gx_state();

    state->clear_color = color;

    if (!state->initialized) {
        GCPS3_LOG_WARN("gx", "GXSetClearColor called before GXInit; state recorded only");
        return;
    }

    gcps3_gx_backend_set_clear_color(color);
}

void GXClear(void)
{
    Gcps3GXState *state = gcps3_gx_state();

    if (!state->initialized) {
        gx_warn_uninitialized("GXClear");
        return;
    }

    gcps3_gx_backend_clear(state);
}

void GXBeginTriangles(void)
{
    Gcps3GXState *state = gcps3_gx_state();

    if (!state->initialized) {
        gx_warn_uninitialized("GXBeginTriangles");
        return;
    }

    if (state->drawing) {
        GCPS3_LOG_WARN("gx", "GXBeginTriangles called while a draw is already active; restarting packet");
    }

    state->drawing = 1;
    state->packet.primitive = GCPS3_GX_PRIMITIVE_TRIANGLES;
    state->packet.vertex_count = 0;
}

void GXPosition3f32(float x, float y, float z)
{
    Gcps3GXState *state = gcps3_gx_state();
    Gcps3GXVertex *vertex;

    if (!state->initialized) {
        gx_warn_uninitialized("GXPosition3f32");
        return;
    }

    if (!state->drawing) {
        GCPS3_LOG_WARN("gx", "GXPosition3f32 called outside GXBeginTriangles/GXEnd");
        return;
    }

    if (state->packet.vertex_count >= GCPS3_GX_MAX_PACKET_VERTICES) {
        GCPS3_LOG_WARN("gx", "GX draw packet vertex capacity reached; dropping vertex");
        return;
    }

    vertex = &state->packet.vertices[state->packet.vertex_count];
    vertex->x = x;
    vertex->y = y;
    vertex->z = z;
    vertex->color = state->current_color;
    state->packet.vertex_count++;
}

void GXColor4u8(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    Gcps3GXState *state = gcps3_gx_state();

    state->current_color.r = r;
    state->current_color.g = g;
    state->current_color.b = b;
    state->current_color.a = a;

    if (!state->initialized) {
        gx_warn_uninitialized("GXColor4u8");
        return;
    }

    if (!state->drawing) {
        GCPS3_LOG_WARN("gx", "GXColor4u8 called outside GXBeginTriangles/GXEnd; current color recorded");
    }
}

void GXEnd(void)
{
    Gcps3GXState *state = gcps3_gx_state();
    unsigned int trailing_vertex_count;

    if (!state->initialized) {
        gx_warn_uninitialized("GXEnd");
        return;
    }

    if (!state->drawing) {
        GCPS3_LOG_WARN("gx", "GXEnd called without GXBeginTriangles");
        return;
    }

    trailing_vertex_count = state->packet.vertex_count % 3u;
    if (trailing_vertex_count != 0u) {
        GCPS3_LOG_WARN("gx", "dropping %u incomplete triangle-list vertex/vertices", trailing_vertex_count);
        state->packet.vertex_count -= trailing_vertex_count;
    }

    if (state->packet.vertex_count == 0u) {
        GCPS3_LOG_WARN("gx", "GXEnd submitted no complete triangles");
        state->drawing = 0;
        return;
    }

    gcps3_gx_backend_submit_draw_packet(&state->packet);
    state->drawing = 0;
}
