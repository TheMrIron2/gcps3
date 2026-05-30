#include "gcps3/config.h"
#include "gcps3/gx.h"
#include "gcps3/log.h"
#include "gcps3/runtime.h"

int main(void)
{
    Gcps3Config config = gcps3_config_default();
    GXColor clear_color;

    config.app_name = "gx_compat_quad";
    config.asset_root = "assets";

    if (gcps3_runtime_init(&config) != 0) {
        GCPS3_LOG_ERROR("sample", "failed to initialize runtime");
        return 1;
    }

    clear_color.r = 8;
    clear_color.g = 12;
    clear_color.b = 20;
    clear_color.a = 255;

    GXInit();
    GXSetViewport(0.0f, 0.0f, 640.0f, 480.0f, 0.0f, 1.0f);
    GXSetClearColor(clear_color);
    GXClear();

    GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
    GX_Color4u8(255, 64, 64, 255);
    GX_Position3f32(-0.55f, 0.55f, 0.0f);
    GX_Color4u8(64, 255, 64, 255);
    GX_Position3f32(-0.55f, -0.55f, 0.0f);
    GX_Color4u8(64, 128, 255, 255);
    GX_Position3f32(0.55f, -0.55f, 0.0f);
    GX_Color4u8(255, 224, 64, 255);
    GX_Position3f32(0.55f, 0.55f, 0.0f);
    GX_End();

    GXShutdown();
    gcps3_runtime_shutdown();
    return 0;
}
