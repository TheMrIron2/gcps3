#include "gcps3/gx.h"

#include "gcps3/log.h"

#include "gx_internal.h"

static void gx_warn_uninitialized(const char *call_name)
{
    GCPS3_LOG_WARN("gx", "%s called before GXInit", call_name);
}

static int16_t gx_read_s16_native(const void *ptr);

static void gx_reset_triangle_packet(Gcps3GXState *state)
{
    gcps3_gx_draw_packet_reset(&state->packet, state, GCPS3_GX_PRIMITIVE_TRIANGLES);
    state->quad_stage_count = 0;
}

static int gx_primitive_is_supported(GXPrimitive primitive)
{
    return primitive == GX_TRIANGLES || primitive == GX_QUADS;
}

static int gx_vtxfmt_is_supported(GXVtxFmt vtxfmt)
{
    return vtxfmt == GX_VTXFMT0;
}

static Gcps3GXSourcePrimitive gx_source_primitive_from_public(GXPrimitive primitive)
{
    if (primitive == GX_QUADS) {
        return GCPS3_GX_SOURCE_PRIMITIVE_QUADS;
    }

    return GCPS3_GX_SOURCE_PRIMITIVE_TRIANGLES;
}

static void gx_begin_with_source_primitive(
    const char *call_name,
    Gcps3GXSourcePrimitive source_primitive,
    uint16_t expected_vertex_count)
{
    Gcps3GXState *state = gcps3_gx_state();

    if (!state->initialized) {
        gx_warn_uninitialized(call_name);
        return;
    }

    if (state->drawing) {
        GCPS3_LOG_WARN("gx", "%s called while a draw is already active; nested begin rejected", call_name);
        return;
    }

    state->drawing = 1;
    state->active_source_primitive = source_primitive;
    gx_reset_triangle_packet(state);
    state->packet.source_primitive = source_primitive;
    state->packet.expected_vertex_count = expected_vertex_count;
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

static void gx_validate_expected_vertex_count(const Gcps3GXDrawPacket *packet)
{
    if (packet->expected_vertex_count == 0u) {
        return;
    }

    if (packet->expected_vertex_count != packet->source_vertex_count) {
        GCPS3_LOG_WARN(
            "gx",
            "GX_Begin expected %u source vertices but saw %u source vertex/vertices",
            (unsigned int)packet->expected_vertex_count,
            packet->source_vertex_count);
    }
}

static int gx_append_packet_vertex(Gcps3GXState *state, const Gcps3GXVertex *vertex)
{
    if (state->packet.vertex_count >= GCPS3_GX_MAX_PACKET_VERTICES) {
        GCPS3_LOG_WARN(
            "gx",
            "GX immediate packet capacity %u reached; dropping vertex",
            (unsigned int)GCPS3_GX_MAX_PACKET_VERTICES);
        return 0;
    }

    state->packet.vertices[state->packet.vertex_count] = *vertex;
    state->packet.vertex_count++;
    return 1;
}

static int gx_append_quad_as_triangles(Gcps3GXState *state)
{
    if (state->packet.vertex_count + 6u > GCPS3_GX_MAX_PACKET_VERTICES) {
        GCPS3_LOG_WARN(
            "gx",
            "GX immediate packet capacity %u cannot fit another converted quad; dropping quad",
            (unsigned int)GCPS3_GX_MAX_PACKET_VERTICES);
        return 0;
    }

    (void)gx_append_packet_vertex(state, &state->quad_stage[0]);
    (void)gx_append_packet_vertex(state, &state->quad_stage[1]);
    (void)gx_append_packet_vertex(state, &state->quad_stage[2]);
    (void)gx_append_packet_vertex(state, &state->quad_stage[0]);
    (void)gx_append_packet_vertex(state, &state->quad_stage[2]);
    (void)gx_append_packet_vertex(state, &state->quad_stage[3]);
    return 1;
}

static void gx_submit_source_vertex(Gcps3GXState *state, const Gcps3GXVertex *vertex)
{
    state->packet.source_vertex_count++;

    if (state->active_source_primitive == GCPS3_GX_SOURCE_PRIMITIVE_QUADS) {
        state->quad_stage[state->quad_stage_count] = *vertex;
        state->quad_stage_count++;

        if (state->quad_stage_count == 4u) {
            (void)gx_append_quad_as_triangles(state);
            state->quad_stage_count = 0;
        }
        return;
    }

    (void)gx_append_packet_vertex(state, vertex);
}

static void gx_submit_position3f32(Gcps3GXState *state, float x, float y, float z)
{
    Gcps3GXVertex vertex;

    vertex.x = x;
    vertex.y = y;
    vertex.z = z;
    vertex.color = state->current_color;
    vertex.s = state->current_s;
    vertex.t = state->current_t;
    gx_submit_source_vertex(state, &vertex);
}

static void gx_position3f32(const char *call_name, float x, float y, float z)
{
    Gcps3GXState *state = gcps3_gx_state();

    if (!state->initialized) {
        gx_warn_uninitialized(call_name);
        return;
    }

    if (!state->drawing) {
        GCPS3_LOG_WARN("gx", "%s called outside GX begin/end", call_name);
        return;
    }

    if (state->packet.descriptor.position != GX_ATTR_DIRECT) {
        GCPS3_LOG_WARN("gx", "%s ignored because position is not enabled as direct", call_name);
        return;
    }

    gx_submit_position3f32(state, x, y, z);
}

static void gx_color4u8(const char *call_name, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    Gcps3GXState *state = gcps3_gx_state();

    if (!state->initialized) {
        gx_warn_uninitialized(call_name);
        return;
    }

    if (!state->drawing) {
        GCPS3_LOG_WARN("gx", "%s called outside GX begin/end", call_name);
        return;
    }

    if (state->packet.descriptor.color0 != GX_ATTR_DIRECT) {
        GCPS3_LOG_WARN("gx", "%s ignored because color0 is not enabled as direct", call_name);
        return;
    }

    state->current_color.r = r;
    state->current_color.g = g;
    state->current_color.b = b;
    state->current_color.a = a;
}

static void gx_position1x8(const char *call_name, uint8_t index)
{
    Gcps3GXState *state = gcps3_gx_state();
    const uint8_t *base;
    const uint8_t *entry;
    int16_t x;
    int16_t y;
    int16_t z;

    if (!state->initialized) {
        gx_warn_uninitialized(call_name);
        return;
    }

    if (!state->drawing) {
        GCPS3_LOG_WARN("gx", "%s called outside GX begin/end", call_name);
        return;
    }

    if (state->packet.descriptor.position != GX_ATTR_INDEX8) {
        GCPS3_LOG_WARN("gx", "%s ignored because position is not enabled as index8", call_name);
        return;
    }

    if (!state->position_array.base || state->position_array.stride == 0u) {
        GCPS3_LOG_WARN("gx", "%s ignored because no valid position array is bound", call_name);
        return;
    }

    base = (const uint8_t *)state->position_array.base;
    entry = base + ((unsigned int)index * (unsigned int)state->position_array.stride);
    x = gx_read_s16_native(entry + 0u);
    y = gx_read_s16_native(entry + 2u);
    z = gx_read_s16_native(entry + 4u);
    gx_submit_position3f32(state, (float)x, (float)y, (float)z);
}

static void gx_color1x8(const char *call_name, uint8_t index)
{
    Gcps3GXState *state = gcps3_gx_state();
    const uint8_t *base;
    const uint8_t *entry;

    if (!state->initialized) {
        gx_warn_uninitialized(call_name);
        return;
    }

    if (!state->drawing) {
        GCPS3_LOG_WARN("gx", "%s called outside GX begin/end", call_name);
        return;
    }

    if (state->packet.descriptor.color0 != GX_ATTR_INDEX8) {
        GCPS3_LOG_WARN("gx", "%s ignored because color0 is not enabled as index8", call_name);
        return;
    }

    if (!state->color0_array.base || state->color0_array.stride == 0u) {
        GCPS3_LOG_WARN("gx", "%s ignored because no valid color0 array is bound", call_name);
        return;
    }

    base = (const uint8_t *)state->color0_array.base;
    entry = base + ((unsigned int)index * (unsigned int)state->color0_array.stride);
    state->current_color.r = entry[0];
    state->current_color.g = entry[1];
    state->current_color.b = entry[2];
    state->current_color.a = entry[3];
}

static void gx_texcoord2f32(const char *call_name, float s, float t)
{
    Gcps3GXState *state = gcps3_gx_state();

    if (!state->initialized) {
        gx_warn_uninitialized(call_name);
        return;
    }

    if (!state->drawing) {
        GCPS3_LOG_WARN("gx", "%s called outside GX begin/end", call_name);
        return;
    }

    if (state->packet.descriptor.tex0 != GX_ATTR_DIRECT) {
        GCPS3_LOG_WARN("gx", "%s ignored because tex0 is not enabled as direct", call_name);
        return;
    }

    state->current_s = s;
    state->current_t = t;
}

static const char *gx_attr_name(GXAttr attr)
{
    switch (attr) {
    case GX_ATTR_POSITION:
        return "position";
    case GX_ATTR_COLOR0:
        return "color0";
    case GX_ATTR_TEX0:
        return "tex0";
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
    case GX_ATTR_INDEX8:
        return "index8";
    default:
        return "unknown";
    }
}

static int gx_attr_type_supported_for_attr(GXAttr attr, GXAttrType type)
{
    if (type == GX_ATTR_NONE || type == GX_ATTR_DIRECT) {
        return 1;
    }

    if (type == GX_ATTR_INDEX8 && (attr == GX_ATTR_POSITION || attr == GX_ATTR_COLOR0)) {
        return 1;
    }

    return 0;
}

static int16_t gx_read_s16_native(const void *ptr)
{
    const uint8_t *bytes = (const uint8_t *)ptr;
    union {
        int16_t value;
        uint8_t bytes[2];
    } reader;

    reader.bytes[0] = bytes[0];
    reader.bytes[1] = bytes[1];
    return reader.value;
}

static int gx_compare_is_valid(GXCompare func)
{
    switch (func) {
    case GX_NEVER:
    case GX_LESS:
    case GX_EQUAL:
    case GX_LEQUAL:
    case GX_GREATER:
    case GX_NEQUAL:
    case GX_GEQUAL:
    case GX_ALWAYS:
        return 1;
    default:
        return 0;
    }
}

static const char *gx_compare_name(GXCompare func)
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

void GXSetZMode(int enable, GXCompare func, int update_enable)
{
    Gcps3GXState *state = gcps3_gx_state();

    if (!gx_compare_is_valid(func)) {
        GCPS3_LOG_WARN("gx", "GXSetZMode called with unsupported compare func=%d", (int)func);
        return;
    }

    state->depth.z_enable = enable ? 1 : 0;
    state->depth.z_func = func;
    state->depth.z_update_enable = update_enable ? 1 : 0;

    if (!state->initialized) {
        GCPS3_LOG_WARN("gx", "GXSetZMode called before GXInit; state recorded only");
        return;
    }

    if (state->drawing) {
        GCPS3_LOG_WARN("gx", "GXSetZMode called during an active immediate draw; depth changes apply to the next packet");
        return;
    }

    GCPS3_LOG_INFO(
        "gx",
        "z mode enable=%d func=%s update=%d",
        state->depth.z_enable,
        gx_compare_name(func),
        state->depth.z_update_enable);
}

void GXClearVtxDesc(void)
{
    Gcps3GXState *state = gcps3_gx_state();

    state->descriptor.position = GX_ATTR_NONE;
    state->descriptor.color0 = GX_ATTR_NONE;
    state->descriptor.tex0 = GX_ATTR_NONE;

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

    if (!gx_attr_type_supported_for_attr(attr, type)) {
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
    case GX_ATTR_TEX0:
        target = &state->descriptor.tex0;
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

void GX_SetArray(GXAttr attr, const void *base, uint8_t stride)
{
    Gcps3GXState *state = gcps3_gx_state();
    Gcps3GXArrayBinding *target = 0;

    if (!base || stride == 0u) {
        GCPS3_LOG_WARN("gx", "GX_SetArray attr=%s called with invalid base or stride", gx_attr_name(attr));
        return;
    }

    switch (attr) {
    case GX_ATTR_POSITION:
        target = &state->position_array;
        break;
    case GX_ATTR_COLOR0:
        target = &state->color0_array;
        break;
    default:
        GCPS3_LOG_WARN("gx", "GX_SetArray called with unsupported attr=%d", (int)attr);
        return;
    }

    target->base = base;
    target->stride = stride;

    if (!state->initialized) {
        GCPS3_LOG_WARN("gx", "GX_SetArray called before GXInit; state recorded only");
        return;
    }

    GCPS3_LOG_INFO("gx", "array %s base=%p stride=%u", gx_attr_name(attr), base, (unsigned int)stride);
}

void GXInitTexObj(GXTexObj *obj, const void *rgba8_pixels, uint32_t width, uint32_t height)
{
    if (!obj) {
        GCPS3_LOG_WARN("gx", "GXInitTexObj called with null texture object");
        return;
    }

    obj->rgba8_pixels = rgba8_pixels;
    obj->width = width;
    obj->height = height;
}

void GXLoadTexObj(const GXTexObj *obj)
{
    Gcps3GXState *state = gcps3_gx_state();

    if (!state->initialized) {
        gx_warn_uninitialized("GXLoadTexObj");
        return;
    }

    if (!obj || !obj->rgba8_pixels || obj->width == 0u || obj->height == 0u) {
        GCPS3_LOG_WARN("gx", "GXLoadTexObj called with invalid RGBA8 texture object");
        return;
    }

    state->texture.bound = 1;
    state->texture.rgba8_pixels = obj->rgba8_pixels;
    state->texture.width = obj->width;
    state->texture.height = obj->height;
    GCPS3_LOG_INFO("gx", "loaded RGBA8 texture %ux%u", (unsigned int)obj->width, (unsigned int)obj->height);
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
    gx_begin_with_source_primitive("GXBeginTriangles", GCPS3_GX_SOURCE_PRIMITIVE_TRIANGLES, 0);
}

void GXPosition3f32(float x, float y, float z)
{
    gx_position3f32("GXPosition3f32", x, y, z);
}

void GXColor4u8(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    gx_color4u8("GXColor4u8", r, g, b, a);
}

void GXTexCoord2f32(float s, float t)
{
    gx_texcoord2f32("GXTexCoord2f32", s, t);
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

    if (state->active_source_primitive == GCPS3_GX_SOURCE_PRIMITIVE_QUADS && state->quad_stage_count != 0u) {
        GCPS3_LOG_WARN(
            "gx",
            "quad source vertex count %u is not a multiple of 4; discarding %u trailing source vertex/vertices",
            state->packet.source_vertex_count,
            state->quad_stage_count);
        state->quad_stage_count = 0;
    }

    gx_trim_incomplete_triangles(&state->packet);
    gx_validate_expected_vertex_count(&state->packet);

    if (state->packet.vertex_count == 0u) {
        GCPS3_LOG_WARN("gx", "GXEnd submitted no complete triangles");
        state->drawing = 0;
        gx_reset_triangle_packet(state);
        return;
    }

    gx_submit_draw_packet(state);
}

void GX_Begin(GXPrimitive primitive, GXVtxFmt vtxfmt, uint16_t vertex_count)
{
    if (!gx_primitive_is_supported(primitive)) {
        GCPS3_LOG_WARN("gx", "GX_Begin called with unsupported primitive=%d", (int)primitive);
        return;
    }

    if (!gx_vtxfmt_is_supported(vtxfmt)) {
        GCPS3_LOG_WARN("gx", "GX_Begin called with unsupported vertex format=%d", (int)vtxfmt);
        return;
    }

    gx_begin_with_source_primitive("GX_Begin", gx_source_primitive_from_public(primitive), vertex_count);
}

void GX_End(void)
{
    GXEnd();
}

void GX_Position3f32(float x, float y, float z)
{
    gx_position3f32("GX_Position3f32", x, y, z);
}

void GX_Color4u8(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    gx_color4u8("GX_Color4u8", r, g, b, a);
}

void GX_TexCoord2f32(float s, float t)
{
    gx_texcoord2f32("GX_TexCoord2f32", s, t);
}

void GX_Position1x8(uint8_t index)
{
    gx_position1x8("GX_Position1x8", index);
}

void GX_Color1x8(uint8_t index)
{
    gx_color1x8("GX_Color1x8", index);
}
