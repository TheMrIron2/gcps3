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

static const char *source_primitive_name(Gcps3GXSourcePrimitive primitive)
{
    switch (primitive) {
    case GCPS3_GX_SOURCE_PRIMITIVE_TRIANGLES:
        return "triangles";
    case GCPS3_GX_SOURCE_PRIMITIVE_QUADS:
        return "quads";
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
    case GX_ATTR_INDEX8:
        return "index8";
    default:
        return "unknown";
    }
}

static const char *compare_name(GXCompare func)
{
    switch (func) {
    case GX_NEVER:
        return "never";
    case GX_LESS:
        return "less";
    case GX_EQUAL:
        return "equal";
    case GX_LEQUAL:
        return "lequal";
    case GX_GREATER:
        return "greater";
    case GX_NEQUAL:
        return "nequal";
    case GX_GEQUAL:
        return "gequal";
    case GX_ALWAYS:
        return "always";
    default:
        return "unknown";
    }
}

static void log_depth(const Gcps3GXDrawPacket *packet)
{
    GCPS3_LOG_INFO(
        "gxnull",
        "depth enable=%d func=%s update=%d",
        packet->depth.z_enable,
        compare_name(packet->depth.z_func),
        packet->depth.z_update_enable);
}

static void log_texture(const Gcps3GXDrawPacket *packet)
{
    if (!packet->texture.bound) {
        GCPS3_LOG_INFO("gxnull", "texture bound=no");
        return;
    }

    GCPS3_LOG_INFO(
        "gxnull",
        "texture bound=yes size=%ux%u",
        (unsigned int)packet->texture.width,
        (unsigned int)packet->texture.height);
}

static void log_matrix(const Gcps3GXDrawPacket *packet)
{
    GCPS3_LOG_INFO(
        "gxnull",
        "matrix row0=(%.3f, %.3f, %.3f, %.3f)",
        packet->current_matrix[0][0],
        packet->current_matrix[0][1],
        packet->current_matrix[0][2],
        packet->current_matrix[0][3]);
    GCPS3_LOG_INFO(
        "gxnull",
        "matrix row1=(%.3f, %.3f, %.3f, %.3f)",
        packet->current_matrix[1][0],
        packet->current_matrix[1][1],
        packet->current_matrix[1][2],
        packet->current_matrix[1][3]);
    GCPS3_LOG_INFO(
        "gxnull",
        "matrix row2=(%.3f, %.3f, %.3f, %.3f)",
        packet->current_matrix[2][0],
        packet->current_matrix[2][1],
        packet->current_matrix[2][2],
        packet->current_matrix[2][3]);
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
        "draw source_primitive=%s backend_primitive=%s source_vertex_count=%u vertex_count=%u expected_vertex_count=%u desc[position=%s color0=%s tex0=%s] mtx=%u",
        source_primitive_name(packet->source_primitive),
        primitive_name(packet->primitive),
        packet->source_vertex_count,
        packet->vertex_count,
        packet->expected_vertex_count,
        attr_type_name(packet->descriptor.position),
        attr_type_name(packet->descriptor.color0),
        attr_type_name(packet->descriptor.tex0),
        (unsigned int)packet->current_matrix_id);

    if (packet->source_primitive == GCPS3_GX_SOURCE_PRIMITIVE_QUADS) {
        GCPS3_LOG_INFO("gxnull", "source quads were converted to backend triangles");
    }

    log_matrix(packet);
    log_depth(packet);
    log_texture(packet);

    for (i = 0; i < packet->vertex_count; i++) {
        const Gcps3GXVertex *vertex = &packet->vertices[i];
        GCPS3_LOG_INFO(
            "gxnull",
            "vertex[%u] position=(%.3f, %.3f, %.3f) color=(%u,%u,%u,%u) texcoord=(%.3f, %.3f)",
            i,
            vertex->x,
            vertex->y,
            vertex->z,
            vertex->color.r,
            vertex->color.g,
            vertex->color.b,
            vertex->color.a,
            vertex->s,
            vertex->t);
    }
}
