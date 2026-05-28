#ifndef GCPS3_GX_H
#define GCPS3_GX_H

#include "gcps3/gx_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum GXAttr {
    GX_ATTR_POSITION = 0,
    GX_ATTR_COLOR0 = 1
} GXAttr;

typedef enum GXAttrType {
    GX_ATTR_NONE = 0,
    GX_ATTR_DIRECT = 1
} GXAttrType;

void GXInit(void);
void GXShutdown(void);
void GXSetViewport(float x, float y, float width, float height, float near_z, float far_z);
void GXSetClearColor(GXColor color);
void GXClear(void);
void GXClearVtxDesc(void);
void GXSetVtxDesc(GXAttr attr, GXAttrType type);
void GXLoadPosMtxImm(const float mtx[3][4], uint32_t id);
void GXSetCurrentMtx(uint32_t id);
void GXBeginTriangles(void);
void GXPosition3f32(float x, float y, float z);
void GXColor4u8(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
void GXEnd(void);

#ifdef __cplusplus
}
#endif

#endif
