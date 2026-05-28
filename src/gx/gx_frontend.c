#include "gcps3/gx.h"

#include "gcps3/log.h"

#include "gx_internal.h"

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
        GCPS3_LOG_WARN("gx", "GXClear called before GXInit");
        return;
    }

    gcps3_gx_backend_clear(state);
}
