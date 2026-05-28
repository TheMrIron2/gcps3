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

typedef enum Gcps3GXPrimitive {
    GCPS3_GX_PRIMITIVE_TRIANGLES = 0
} Gcps3GXPrimitive;

typedef struct Gcps3GXVertex {
    float x;
    float y;
    float z;
    GXColor color;
    float s;
    float t;
} Gcps3GXVertex;

typedef struct Gcps3GXDrawPacket {
    Gcps3GXPrimitive primitive;
    Gcps3GXVtxDesc descriptor;
    uint32_t current_matrix_id;
    Gcps3GXPosMtx current_matrix;
    Gcps3GXTextureState texture;
    unsigned int vertex_count;
    Gcps3GXVertex vertices[GCPS3_GX_MAX_PACKET_VERTICES];
} Gcps3GXDrawPacket;

typedef struct Gcps3GXState {
    int initialized;
    int drawing;
    GXColor clear_color;
    GXColor current_color;
    float current_s;
    float current_t;
    GXViewport viewport;
    Gcps3GXVtxDesc descriptor;
    Gcps3GXTextureState texture;
    uint32_t current_matrix_id;
    Gcps3GXPosMtx position_matrices[GCPS3_GX_MAX_POS_MATRICES];
    Gcps3GXDrawPacket packet;
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
