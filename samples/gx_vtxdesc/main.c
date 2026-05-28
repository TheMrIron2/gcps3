#include "gcps3/config.h"
#include "gcps3/gx.h"
#include "gcps3/log.h"
#include "gcps3/runtime.h"

int main(void)
{
    Gcps3Config config = gcps3_config_default();
    GXColor clear_color;

    config.app_name = "gx_vtxdesc";
    config.asset_root = "assets";

    if (gcps3_runtime_init(&config) != 0) {
        GCPS3_LOG_ERROR("sample", "failed to initialize runtime");
        return 1;
    }

    clear_color.r = 16;
    clear_color.g = 16;
    clear_color.b = 24;
    clear_color.a = 255;

    GXInit();
    GXSetViewport(0.0f, 0.0f, 640.0f, 480.0f, 0.0f, 1.0f);
    GXSetClearColor(clear_color);
    GXClear();

    GXClearVtxDesc();
    GXSetVtxDesc(GX_ATTR_POSITION, GX_ATTR_DIRECT);

    GXBeginTriangles();
    GXColor4u8(255, 64, 64, 255);
    GXPosition3f32(0.0f, 0.6f, 0.0f);
    GXColor4u8(64, 255, 64, 255);
    GXPosition3f32(-0.6f, -0.5f, 0.0f);
    GXColor4u8(64, 128, 255, 255);
    GXPosition3f32(0.6f, -0.5f, 0.0f);
    GXEnd();

    GXShutdown();
    gcps3_runtime_shutdown();
    return 0;
}
