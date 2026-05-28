#ifndef GCPS3_GX_INTERNAL_H
#define GCPS3_GX_INTERNAL_H

#include "gcps3/gx_types.h"

typedef struct Gcps3GXState {
    int initialized;
    GXColor clear_color;
    GXViewport viewport;
} Gcps3GXState;

Gcps3GXState *gcps3_gx_state(void);
void gcps3_gx_state_reset(void);

void gcps3_gx_backend_init(const Gcps3GXState *state);
void gcps3_gx_backend_shutdown(const Gcps3GXState *state);
void gcps3_gx_backend_set_viewport(const GXViewport *viewport);
void gcps3_gx_backend_set_clear_color(GXColor color);
void gcps3_gx_backend_clear(const Gcps3GXState *state);

#endif
