#ifndef GCPS3_GX_H
#define GCPS3_GX_H

#include "gcps3/gx_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum GXAttr {
    GX_ATTR_POSITION = 0,
    GX_ATTR_COLOR0 = 1,
    GX_ATTR_TEX0 = 2
} GXAttr;

typedef enum GXAttrType {
    GX_ATTR_NONE = 0,
    GX_ATTR_DIRECT = 1,
    GX_ATTR_INDEX8 = 2
} GXAttrType;

#define GX_NONE GX_ATTR_NONE
#define GX_DIRECT GX_ATTR_DIRECT
#define GX_INDEX8 GX_ATTR_INDEX8

typedef enum GXCompare {
    GX_NEVER = 0,
    GX_LESS = 1,
    GX_EQUAL = 2,
    GX_LEQUAL = 3,
    GX_GREATER = 4,
    GX_NEQUAL = 5,
    GX_GEQUAL = 6,
    GX_ALWAYS = 7
} GXCompare;

typedef enum GXPrimitive {
    GX_TRIANGLES = 0,
    GX_QUADS = 1
} GXPrimitive;

typedef enum GXVtxFmt {
    GX_VTXFMT0 = 0
} GXVtxFmt;

void GXInit(void);
void GXShutdown(void);
void GXSetViewport(float x, float y, float width, float height, float near_z, float far_z);
void GXSetClearColor(GXColor color);
void GXClear(void);
void GXSetZMode(int enable, GXCompare func, int update_enable);
void GXClearVtxDesc(void);
void GXSetVtxDesc(GXAttr attr, GXAttrType type);
void GX_SetArray(GXAttr attr, const void *base, uint8_t stride);
void GXInitTexObj(GXTexObj *obj, const void *rgba8_pixels, uint32_t width, uint32_t height);
void GXLoadTexObj(const GXTexObj *obj);
void GXLoadPosMtxImm(const float mtx[3][4], uint32_t id);
void GXSetCurrentMtx(uint32_t id);
void GXBeginTriangles(void);
void GXPosition3f32(float x, float y, float z);
void GXColor4u8(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
void GXTexCoord2f32(float s, float t);
void GXEnd(void);
void GX_Begin(GXPrimitive primitive, GXVtxFmt vtxfmt, uint16_t vertex_count);
void GX_End(void);
void GX_Position3f32(float x, float y, float z);
void GX_Color4u8(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
void GX_TexCoord2f32(float s, float t);
void GX_Position1x8(uint8_t index);
void GX_Color1x8(uint8_t index);

#ifdef __cplusplus
}
#endif

#endif
