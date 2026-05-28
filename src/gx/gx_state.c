#include "gx_internal.h"

static Gcps3GXState s_gx_state;

static void set_identity_pos_mtx(Gcps3GXPosMtx matrix)
{
    matrix[0][0] = 1.0f;
    matrix[0][1] = 0.0f;
    matrix[0][2] = 0.0f;
    matrix[0][3] = 0.0f;
    matrix[1][0] = 0.0f;
    matrix[1][1] = 1.0f;
    matrix[1][2] = 0.0f;
    matrix[1][3] = 0.0f;
    matrix[2][0] = 0.0f;
    matrix[2][1] = 0.0f;
    matrix[2][2] = 1.0f;
    matrix[2][3] = 0.0f;
}

void gcps3_gx_copy_pos_mtx(Gcps3GXPosMtx dst, const float src[3][4])
{
    unsigned int row;
    unsigned int col;

    for (row = 0; row < 3u; row++) {
        for (col = 0; col < 4u; col++) {
            dst[row][col] = src[row][col];
        }
    }
}

void gcps3_gx_draw_packet_reset(Gcps3GXDrawPacket *packet, const Gcps3GXState *state, Gcps3GXPrimitive primitive)
{
    packet->primitive = primitive;
    packet->descriptor = state->descriptor;
    packet->current_matrix_id = state->current_matrix_id;
    gcps3_gx_copy_pos_mtx(
        packet->current_matrix,
        (const float (*)[4])state->position_matrices[state->current_matrix_id]);
    packet->texture = state->texture;
    packet->vertex_count = 0;
}

Gcps3GXState *gcps3_gx_state(void)
{
    return &s_gx_state;
}

void gcps3_gx_state_reset(void)
{
    unsigned int i;

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
    s_gx_state.current_s = 0.0f;
    s_gx_state.current_t = 0.0f;
    s_gx_state.viewport.x = 0.0f;
    s_gx_state.viewport.y = 0.0f;
    s_gx_state.viewport.width = 640.0f;
    s_gx_state.viewport.height = 480.0f;
    s_gx_state.viewport.near_z = 0.0f;
    s_gx_state.viewport.far_z = 1.0f;
    s_gx_state.descriptor.position = GX_ATTR_DIRECT;
    s_gx_state.descriptor.color0 = GX_ATTR_DIRECT;
    s_gx_state.descriptor.tex0 = GX_ATTR_NONE;
    s_gx_state.texture.bound = 0;
    s_gx_state.texture.rgba8_pixels = 0;
    s_gx_state.texture.width = 0;
    s_gx_state.texture.height = 0;
    s_gx_state.current_matrix_id = 0;
    for (i = 0; i < GCPS3_GX_MAX_POS_MATRICES; i++) {
        set_identity_pos_mtx(s_gx_state.position_matrices[i]);
    }
    gcps3_gx_draw_packet_reset(&s_gx_state.packet, &s_gx_state, GCPS3_GX_PRIMITIVE_TRIANGLES);
}
