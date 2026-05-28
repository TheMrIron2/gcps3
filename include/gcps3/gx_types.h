#ifndef GCPS3_GX_TYPES_H
#define GCPS3_GX_TYPES_H

#include <stdint.h>

typedef struct GXColor {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} GXColor;

typedef struct GXViewport {
    float x;
    float y;
    float width;
    float height;
    float near_z;
    float far_z;
} GXViewport;

typedef struct GXTexObj {
    const void *rgba8_pixels;
    uint32_t width;
    uint32_t height;
} GXTexObj;

#endif
