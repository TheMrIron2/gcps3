#include "gcps3/config.h"
#include "gcps3/gx.h"
#include "gcps3/log.h"
#include "gcps3/runtime.h"

int main(void)
{
    Gcps3Config config = gcps3_config_default();
    GXColor clear_color;
    float translate[3][4] = {
        { 1.0f, 0.0f, 0.0f, 0.35f },
        { 0.0f, 1.0f, 0.0f, 0.20f },
        { 0.0f, 0.0f, 1.0f, 0.00f }
    };

    config.app_name = "gx_matrix";
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

    GXLoadPosMtxImm(translate, 1);
    GXSetCurrentMtx(1);

    GXBeginTriangles();
    GXColor4u8(255, 96, 96, 255);
    GXPosition3f32(0.0f, 0.6f, 0.0f);
    GXColor4u8(96, 255, 96, 255);
    GXPosition3f32(-0.6f, -0.5f, 0.0f);
    GXColor4u8(96, 160, 255, 255);
    GXPosition3f32(0.6f, -0.5f, 0.0f);
    GXEnd();

    GXShutdown();
    gcps3_runtime_shutdown();
    return 0;
}
