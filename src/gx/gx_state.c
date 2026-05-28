#include "gx_internal.h"

static Gcps3GXState s_gx_state;

Gcps3GXState *gcps3_gx_state(void)
{
    return &s_gx_state;
}

void gcps3_gx_state_reset(void)
{
    s_gx_state.initialized = 0;
    s_gx_state.drawing = 0;
    s_gx_state.clear_color.r = 0;
    s_gx_state.clear_color.g = 0;
    s_gx_state.clear_color.b = 0;
    s_gx_state.clear_color.a = 255;
    s_gx_state.current_color.r = 255;
    s_gx_state.current_color.g = 255;
    s_gx_state.current_color.b = 255;
    s_gx_state.current_color.a = 255;
    s_gx_state.viewport.x = 0.0f;
    s_gx_state.viewport.y = 0.0f;
    s_gx_state.viewport.width = 640.0f;
    s_gx_state.viewport.height = 480.0f;
    s_gx_state.viewport.near_z = 0.0f;
    s_gx_state.viewport.far_z = 1.0f;
    s_gx_state.packet.primitive = GCPS3_GX_PRIMITIVE_TRIANGLES;
    s_gx_state.packet.vertex_count = 0;
}
