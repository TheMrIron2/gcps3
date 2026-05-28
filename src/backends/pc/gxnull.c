#include "../../gx/gx_internal.h"

#include "gcps3/log.h"

static const char *primitive_name(Gcps3GXPrimitive primitive)
{
    switch (primitive) {
    case GCPS3_GX_PRIMITIVE_TRIANGLES:
        return "triangles";
    default:
        return "unknown";
    }
}

static const char *attr_type_name(GXAttrType type)
{
    switch (type) {
    case GX_ATTR_NONE:
        return "none";
    case GX_ATTR_DIRECT:
        return "direct";
    default:
        return "unknown";
    }
}

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

void gcps3_gx_backend_submit_draw_packet(const Gcps3GXDrawPacket *packet)
{
    unsigned int i;

    GCPS3_LOG_INFO(
        "gxnull",
        "draw primitive=%s vertex_count=%u desc[position=%s color0=%s]",
        primitive_name(packet->primitive),
        packet->vertex_count,
        attr_type_name(packet->descriptor.position),
        attr_type_name(packet->descriptor.color0));

    for (i = 0; i < packet->vertex_count; i++) {
        const Gcps3GXVertex *vertex = &packet->vertices[i];
        GCPS3_LOG_INFO(
            "gxnull",
            "vertex[%u] position=(%.3f, %.3f, %.3f) color=(%u,%u,%u,%u)",
            i,
            vertex->x,
            vertex->y,
            vertex->z,
            vertex->color.r,
            vertex->color.g,
            vertex->color.b,
            vertex->color.a);
    }
}
