#ifndef GCPS3_GX_INTERNAL_H
#define GCPS3_GX_INTERNAL_H

#include "gcps3/gx.h"

/*
 * Temporary immediate-mode packet capacity for early runtime validation.
 * This is deliberately small and is not the final high-performance renderer path.
 */
#define GCPS3_GX_MAX_PACKET_VERTICES 64

typedef struct Gcps3GXVtxDesc {
    GXAttrType position;
    GXAttrType color0;
} Gcps3GXVtxDesc;

typedef enum Gcps3GXPrimitive {
    GCPS3_GX_PRIMITIVE_TRIANGLES = 0
} Gcps3GXPrimitive;

typedef struct Gcps3GXVertex {
    float x;
    float y;
    float z;
    GXColor color;
} Gcps3GXVertex;

typedef struct Gcps3GXDrawPacket {
    Gcps3GXPrimitive primitive;
    Gcps3GXVtxDesc descriptor;
    unsigned int vertex_count;
    Gcps3GXVertex vertices[GCPS3_GX_MAX_PACKET_VERTICES];
} Gcps3GXDrawPacket;

typedef struct Gcps3GXState {
    int initialized;
    int drawing;
    GXColor clear_color;
    GXColor current_color;
    GXViewport viewport;
    Gcps3GXVtxDesc descriptor;
    Gcps3GXDrawPacket packet;
} Gcps3GXState;

Gcps3GXState *gcps3_gx_state(void);
void gcps3_gx_state_reset(void);

void gcps3_gx_backend_init(const Gcps3GXState *state);
void gcps3_gx_backend_shutdown(const Gcps3GXState *state);
void gcps3_gx_backend_set_viewport(const GXViewport *viewport);
void gcps3_gx_backend_set_clear_color(GXColor color);
void gcps3_gx_backend_clear(const Gcps3GXState *state);
void gcps3_gx_backend_submit_draw_packet(const Gcps3GXDrawPacket *packet);

#endif
