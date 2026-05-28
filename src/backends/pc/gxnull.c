#include "../../gx/gx_internal.h"

#include "gcps3/log.h"

void gcps3_gx_backend_init(const Gcps3GXState *state)
{
    GCPS3_LOG_INFO(
        "gxnull",
        "init clear=(%u,%u,%u,%u) viewport=(%.1f,%.1f %.1fx%.1f %.3f..%.3f)",
        state->clear_color.r,
        state->clear_color.g,
        state->clear_color.b,
        state->clear_color.a,
        state->viewport.x,
        state->viewport.y,
        state->viewport.width,
        state->viewport.height,
        state->viewport.near_z,
        state->viewport.far_z);
}

void gcps3_gx_backend_shutdown(const Gcps3GXState *state)
{
    (void)state;
    GCPS3_LOG_INFO("gxnull", "shutdown");
}

void gcps3_gx_backend_set_viewport(const GXViewport *viewport)
{
    GCPS3_LOG_INFO(
        "gxnull",
        "viewport x=%.1f y=%.1f width=%.1f height=%.1f near=%.3f far=%.3f",
        viewport->x,
        viewport->y,
        viewport->width,
        viewport->height,
        viewport->near_z,
        viewport->far_z);
}

void gcps3_gx_backend_set_clear_color(GXColor color)
{
    GCPS3_LOG_INFO(
        "gxnull",
        "clear color r=%u g=%u b=%u a=%u",
        color.r,
        color.g,
        color.b,
        color.a);
}

void gcps3_gx_backend_clear(const Gcps3GXState *state)
{
    GCPS3_LOG_INFO(
        "gxnull",
        "clear color=(%u,%u,%u,%u) viewport=(%.1f,%.1f %.1fx%.1f)",
        state->clear_color.r,
        state->clear_color.g,
        state->clear_color.b,
        state->clear_color.a,
        state->viewport.x,
        state->viewport.y,
        state->viewport.width,
        state->viewport.height);
}
