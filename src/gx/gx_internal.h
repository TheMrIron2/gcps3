#ifndef GCPS3_GX_INTERNAL_H
#define GCPS3_GX_INTERNAL_H

#include "gcps3/gx.h"

/*
 * Temporary immediate-mode packet capacity for early runtime validation.
 * This is deliberately small and is not the final high-performance renderer path.
 */
#define GCPS3_GX_MAX_PACKET_VERTICES 64
#define GCPS3_GX_MAX_POS_MATRICES 10

typedef float Gcps3GXPosMtx[3][4];

typedef struct Gcps3GXVtxDesc {
    GXAttrType position;
    GXAttrType color0;
    GXAttrType tex0;
} Gcps3GXVtxDesc;

typedef struct Gcps3GXTextureState {
    int bound;
    const void *rgba8_pixels;
    uint32_t width;
    uint32_t height;
} Gcps3GXTextureState;

typedef struct Gcps3GXDepthState {
    int z_enable;
    GXCompare z_func;
    int z_update_enable;
} Gcps3GXDepthState;

typedef struct Gcps3GXArrayBinding {
    const void *base;
    uint8_t stride;
} Gcps3GXArrayBinding;

typedef enum Gcps3GXPrimitive {
    GCPS3_GX_PRIMITIVE_TRIANGLES = 0
} Gcps3GXPrimitive;

typedef enum Gcps3GXSourcePrimitive {
    GCPS3_GX_SOURCE_PRIMITIVE_TRIANGLES = 0,
    GCPS3_GX_SOURCE_PRIMITIVE_QUADS = 1
} Gcps3GXSourcePrimitive;

typedef struct Gcps3GXVertex {
    float x;
    float y;
    float z;
    GXColor color;
    float s;
    float t;
} Gcps3GXVertex;

typedef struct Gcps3GXDrawPacket {
    Gcps3GXSourcePrimitive source_primitive;
    Gcps3GXPrimitive primitive;
    uint16_t expected_vertex_count;
    unsigned int source_vertex_count;
    Gcps3GXVtxDesc descriptor;
    uint32_t current_matrix_id;
    Gcps3GXPosMtx current_matrix;
    Gcps3GXTextureState texture;
    Gcps3GXDepthState depth;
    unsigned int vertex_count;
    Gcps3GXVertex vertices[GCPS3_GX_MAX_PACKET_VERTICES];
} Gcps3GXDrawPacket;

typedef struct Gcps3GXState {
    int initialized;
    int drawing;
    Gcps3GXSourcePrimitive active_source_primitive;
    GXColor clear_color;
    GXColor current_color;
    float current_s;
    float current_t;
    GXViewport viewport;
    Gcps3GXVtxDesc descriptor;
    Gcps3GXArrayBinding position_array;
    Gcps3GXArrayBinding color0_array;
    Gcps3GXTextureState texture;
    Gcps3GXDepthState depth;
    uint32_t current_matrix_id;
    Gcps3GXPosMtx position_matrices[GCPS3_GX_MAX_POS_MATRICES];
    Gcps3GXDrawPacket packet;
    unsigned int quad_stage_count;
    Gcps3GXVertex quad_stage[4];
} Gcps3GXState;

Gcps3GXState *gcps3_gx_state(void);
void gcps3_gx_state_reset(void);
void gcps3_gx_copy_pos_mtx(Gcps3GXPosMtx dst, const float src[3][4]);
void gcps3_gx_draw_packet_reset(Gcps3GXDrawPacket *packet, const Gcps3GXState *state, Gcps3GXPrimitive primitive);

void gcps3_gx_backend_init(const Gcps3GXState *state);
void gcps3_gx_backend_shutdown(const Gcps3GXState *state);
void gcps3_gx_backend_set_viewport(const GXViewport *viewport);
void gcps3_gx_backend_set_clear_color(GXColor color);
void gcps3_gx_backend_clear(const Gcps3GXState *state);
void gcps3_gx_backend_submit_draw_packet(const Gcps3GXDrawPacket *packet);

#endif
