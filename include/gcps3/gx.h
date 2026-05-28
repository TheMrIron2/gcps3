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
void GXBeginTriangles(void);
void GXPosition3f32(float x, float y, float z);
void GXColor4u8(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
void GXEnd(void);

#ifdef __cplusplus
}
#endif

#endif
