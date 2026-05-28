#ifndef GCPS3_GX_H
#define GCPS3_GX_H

#include "gcps3/gx_types.h"

#ifdef __cplusplus
extern "C" {
#endif

void GXInit(void);
void GXShutdown(void);
void GXSetViewport(float x, float y, float width, float height, float near_z, float far_z);
void GXSetClearColor(GXColor color);
void GXClear(void);

#ifdef __cplusplus
}
#endif

#endif
