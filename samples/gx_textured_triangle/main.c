#include "gcps3/config.h"
#include "gcps3/gx.h"
#include "gcps3/log.h"
#include "gcps3/runtime.h"

int main(void)
{
    static const uint8_t checker_rgba8[] = {
        255, 255, 255, 255, 32, 32, 32, 255,
        32, 32, 32, 255, 255, 255, 255
    };
    Gcps3Config config = gcps3_config_default();
    GXColor clear_color;
    GXTexObj texture;

    config.app_name = "gx_textured_triangle";
    config.asset_root = "assets";

    if (gcps3_runtime_init(&config) != 0) {
        GCPS3_LOG_ERROR("sample", "failed to initialize runtime");
        return 1;
    }

    clear_color.r = 12;
    clear_color.g = 16;
    clear_color.b = 24;
    clear_color.a = 255;

    GXInit();
    GXSetViewport(0.0f, 0.0f, 640.0f, 480.0f, 0.0f, 1.0f);
    GXSetClearColor(clear_color);
    GXClear();

    GXInitTexObj(&texture, checker_rgba8, 2, 2);
    GXLoadTexObj(&texture);

    GXClearVtxDesc();
    GXSetVtxDesc(GX_ATTR_POSITION, GX_ATTR_DIRECT);
    GXSetVtxDesc(GX_ATTR_COLOR0, GX_ATTR_DIRECT);
    GXSetVtxDesc(GX_ATTR_TEX0, GX_ATTR_DIRECT);

    GXBeginTriangles();
    GXColor4u8(255, 255, 255, 255);
    GXTexCoord2f32(0.5f, 0.0f);
    GXPosition3f32(0.0f, 0.65f, 0.0f);
    GXColor4u8(255, 255, 255, 255);
    GXTexCoord2f32(0.0f, 1.0f);
    GXPosition3f32(-0.65f, -0.55f, 0.0f);
    GXColor4u8(255, 255, 255, 255);
    GXTexCoord2f32(1.0f, 1.0f);
    GXPosition3f32(0.65f, -0.55f, 0.0f);
    GXEnd();

    GXShutdown();
    gcps3_runtime_shutdown();
    return 0;
}
