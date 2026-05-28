#include "gcps3/config.h"
#include "gcps3/gx.h"
#include "gcps3/log.h"
#include "gcps3/runtime.h"

typedef struct CubeVertex {
    float x;
    float y;
    float z;
    float s;
    float t;
} CubeVertex;

static void emit_vertex(const CubeVertex *vertex)
{
    GXColor4u8(255, 255, 255, 255);
    GXTexCoord2f32(vertex->s, vertex->t);
    GXPosition3f32(vertex->x, vertex->y, vertex->z);
}

static void emit_triangle(const CubeVertex *a, const CubeVertex *b, const CubeVertex *c)
{
    emit_vertex(a);
    emit_vertex(b);
    emit_vertex(c);
}

static void emit_face(CubeVertex a, CubeVertex b, CubeVertex c, CubeVertex d)
{
    emit_triangle(&a, &b, &c);
    emit_triangle(&a, &c, &d);
}

static CubeVertex cube_vertex(float x, float y, float z, float s, float t)
{
    CubeVertex vertex;

    vertex.x = x;
    vertex.y = y;
    vertex.z = z;
    vertex.s = s;
    vertex.t = t;
    return vertex;
}

int main(void)
{
    static const uint8_t checker_rgba8[] = {
        255, 255, 255, 255, 48, 48, 48, 255, 255, 255, 255, 48, 48, 48, 255,
        48, 48, 48, 255, 255, 255, 255, 48, 48, 48, 255, 255, 255, 255,
        255, 255, 255, 255, 48, 48, 48, 255, 255, 255, 255, 48, 48, 48, 255,
        48, 48, 48, 255, 255, 255, 255, 48, 48, 48, 255, 255, 255, 255
    };
    Gcps3Config config = gcps3_config_default();
    GXColor clear_color;
    GXTexObj texture;
    float model[3][4] = {
        { 0.85f, 0.0f, 0.0f, -0.05f },
        { 0.0f, 0.85f, 0.0f, -0.05f },
        { 0.0f, 0.0f, 1.0f, 0.0f }
    };

    CubeVertex f0 = cube_vertex(-0.45f, -0.45f, 0.25f, 0.0f, 1.0f);
    CubeVertex f1 = cube_vertex(0.45f, -0.45f, 0.25f, 1.0f, 1.0f);
    CubeVertex f2 = cube_vertex(0.45f, 0.45f, 0.25f, 1.0f, 0.0f);
    CubeVertex f3 = cube_vertex(-0.45f, 0.45f, 0.25f, 0.0f, 0.0f);
    CubeVertex b0 = cube_vertex(-0.20f, -0.20f, -0.25f, 0.0f, 1.0f);
    CubeVertex b1 = cube_vertex(0.60f, -0.20f, -0.25f, 1.0f, 1.0f);
    CubeVertex b2 = cube_vertex(0.60f, 0.60f, -0.25f, 1.0f, 0.0f);
    CubeVertex b3 = cube_vertex(-0.20f, 0.60f, -0.25f, 0.0f, 0.0f);

    config.app_name = "gx_textured_cube";
    config.asset_root = "assets";

    if (gcps3_runtime_init(&config) != 0) {
        GCPS3_LOG_ERROR("sample", "failed to initialize runtime");
        return 1;
    }

    clear_color.r = 10;
    clear_color.g = 14;
    clear_color.b = 22;
    clear_color.a = 255;

    GXInit();
    GXSetViewport(0.0f, 0.0f, 640.0f, 480.0f, 0.0f, 1.0f);
    GXSetClearColor(clear_color);
    GXClear();

    GXInitTexObj(&texture, checker_rgba8, 4, 4);
    GXLoadTexObj(&texture);

    GXClearVtxDesc();
    GXSetVtxDesc(GX_ATTR_POSITION, GX_ATTR_DIRECT);
    GXSetVtxDesc(GX_ATTR_COLOR0, GX_ATTR_DIRECT);
    GXSetVtxDesc(GX_ATTR_TEX0, GX_ATTR_DIRECT);

    GXLoadPosMtxImm(model, 1);
    GXSetCurrentMtx(1);

    GXBeginTriangles();
    emit_face(b0, b1, b2, b3);
    emit_face(f0, b0, b3, f3);
    emit_face(f1, b1, b2, f2);
    emit_face(f3, b3, b2, f2);
    emit_face(f0, b0, b1, f1);
    emit_face(f0, f1, f2, f3);
    GXEnd();

    GXShutdown();
    gcps3_runtime_shutdown();
    return 0;
}
