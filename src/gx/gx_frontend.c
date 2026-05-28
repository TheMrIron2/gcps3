#include "gcps3/gx.h"

#include "gcps3/log.h"

#include "gx_internal.h"

static void gx_warn_uninitialized(const char *call_name)
{
    GCPS3_LOG_WARN("gx", "%s called before GXInit", call_name);
}

static void gx_reset_triangle_packet(Gcps3GXState *state)
{
    gcps3_gx_draw_packet_reset(&state->packet, state, GCPS3_GX_PRIMITIVE_TRIANGLES);
}

static unsigned int gx_trim_incomplete_triangles(Gcps3GXDrawPacket *packet)
{
    unsigned int trailing_vertex_count;

    trailing_vertex_count = packet->vertex_count % 3u;
    if (trailing_vertex_count != 0u) {
        GCPS3_LOG_WARN(
            "gx",
            "triangle-list vertex count %u is not a multiple of 3; discarding %u trailing vertex/vertices",
            packet->vertex_count,
            trailing_vertex_count);
        packet->vertex_count -= trailing_vertex_count;
    }

    return trailing_vertex_count;
}

static void gx_submit_draw_packet(Gcps3GXState *state)
{
    gcps3_gx_backend_submit_draw_packet(&state->packet);
    state->drawing = 0;
    gx_reset_triangle_packet(state);
}

static const char *gx_attr_name(GXAttr attr)
{
    switch (attr) {
    case GX_ATTR_POSITION:
        return "position";
    case GX_ATTR_COLOR0:
        return "color0";
    default:
        return "unknown";
    }
}

static const char *gx_attr_type_name(GXAttrType type)
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

    if (state->drawing) {
        GCPS3_LOG_WARN("gx", "GXShutdown called during an active immediate draw; discarding packet");
        state->drawing = 0;
        gx_reset_triangle_packet(state);
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

void GXClearVtxDesc(void)
{
    Gcps3GXState *state = gcps3_gx_state();

    state->descriptor.position = GX_ATTR_NONE;
    state->descriptor.color0 = GX_ATTR_NONE;

    if (!state->initialized) {
        GCPS3_LOG_WARN("gx", "GXClearVtxDesc called before GXInit; state recorded only");
        return;
    }

    if (state->drawing) {
        GCPS3_LOG_WARN("gx", "GXClearVtxDesc called during an active immediate draw; descriptor changes apply to the next packet");
        return;
    }

    GCPS3_LOG_INFO("gx", "vertex descriptors cleared");
}

void GXSetVtxDesc(GXAttr attr, GXAttrType type)
{
    Gcps3GXState *state = gcps3_gx_state();
    GXAttrType *target = 0;

    if (type != GX_ATTR_NONE && type != GX_ATTR_DIRECT) {
        GCPS3_LOG_WARN("gx", "GXSetVtxDesc attr=%s has unsupported type=%d", gx_attr_name(attr), (int)type);
        return;
    }

    switch (attr) {
    case GX_ATTR_POSITION:
        target = &state->descriptor.position;
        break;
    case GX_ATTR_COLOR0:
        target = &state->descriptor.color0;
        break;
    default:
        GCPS3_LOG_WARN("gx", "GXSetVtxDesc called with unsupported attr=%d", (int)attr);
        return;
    }

    *target = type;

    if (!state->initialized) {
        GCPS3_LOG_WARN("gx", "GXSetVtxDesc called before GXInit; state recorded only");
        return;
    }

    if (state->drawing) {
        GCPS3_LOG_WARN("gx", "GXSetVtxDesc called during an active immediate draw; descriptor changes apply to the next packet");
        return;
    }

    GCPS3_LOG_INFO("gx", "vertex descriptor %s=%s", gx_attr_name(attr), gx_attr_type_name(type));
}

void GXLoadPosMtxImm(const float mtx[3][4], uint32_t id)
{
    Gcps3GXState *state = gcps3_gx_state();

    if (id >= GCPS3_GX_MAX_POS_MATRICES) {
        GCPS3_LOG_WARN(
            "gx",
            "GXLoadPosMtxImm id=%u is out of range; max position matrices=%u",
            (unsigned int)id,
            (unsigned int)GCPS3_GX_MAX_POS_MATRICES);
        return;
    }

    gcps3_gx_copy_pos_mtx(state->position_matrices[id], mtx);

    if (!state->initialized) {
        GCPS3_LOG_WARN("gx", "GXLoadPosMtxImm called before GXInit; state recorded only");
        return;
    }

    GCPS3_LOG_INFO("gx", "loaded position matrix id=%u", (unsigned int)id);
}

void GXSetCurrentMtx(uint32_t id)
{
    Gcps3GXState *state = gcps3_gx_state();

    if (id >= GCPS3_GX_MAX_POS_MATRICES) {
        GCPS3_LOG_WARN(
            "gx",
            "GXSetCurrentMtx id=%u is out of range; max position matrices=%u",
            (unsigned int)id,
            (unsigned int)GCPS3_GX_MAX_POS_MATRICES);
        return;
    }

    state->current_matrix_id = id;

    if (!state->initialized) {
        GCPS3_LOG_WARN("gx", "GXSetCurrentMtx called before GXInit; state recorded only");
        return;
    }

    GCPS3_LOG_INFO("gx", "current position matrix id=%u", (unsigned int)id);
}

/*
 * Temporary immediate-mode frontend used to validate early GX-style samples.
 * Real rendering backends should eventually consume batched, explicit draw data.
 */
void GXBeginTriangles(void)
{
    Gcps3GXState *state = gcps3_gx_state();

    if (!state->initialized) {
        gx_warn_uninitialized("GXBeginTriangles");
        return;
    }

    if (state->drawing) {
        GCPS3_LOG_WARN("gx", "GXBeginTriangles called while a draw is already active; nested begin rejected");
        return;
    }

    state->drawing = 1;
    gx_reset_triangle_packet(state);
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

    if (state->packet.descriptor.position != GX_ATTR_DIRECT) {
        GCPS3_LOG_WARN("gx", "GXPosition3f32 ignored because position is not enabled as direct");
        return;
    }

    if (state->packet.vertex_count >= GCPS3_GX_MAX_PACKET_VERTICES) {
        GCPS3_LOG_WARN(
            "gx",
            "GX immediate packet capacity %u reached; dropping vertex",
            (unsigned int)GCPS3_GX_MAX_PACKET_VERTICES);
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

    if (!state->initialized) {
        gx_warn_uninitialized("GXColor4u8");
        return;
    }

    if (!state->drawing) {
        GCPS3_LOG_WARN("gx", "GXColor4u8 called outside GXBeginTriangles/GXEnd");
        return;
    }

    if (state->packet.descriptor.color0 != GX_ATTR_DIRECT) {
        GCPS3_LOG_WARN("gx", "GXColor4u8 ignored because color0 is not enabled as direct");
        return;
    }

    state->current_color.r = r;
    state->current_color.g = g;
    state->current_color.b = b;
    state->current_color.a = a;
}

void GXEnd(void)
{
    Gcps3GXState *state = gcps3_gx_state();

    if (!state->initialized) {
        gx_warn_uninitialized("GXEnd");
        return;
    }

    if (!state->drawing) {
        GCPS3_LOG_WARN("gx", "GXEnd called without GXBeginTriangles");
        return;
    }

    gx_trim_incomplete_triangles(&state->packet);

    if (state->packet.vertex_count == 0u) {
        GCPS3_LOG_WARN("gx", "GXEnd submitted no complete triangles");
        state->drawing = 0;
        gx_reset_triangle_packet(state);
        return;
    }

    gx_submit_draw_packet(state);
}
