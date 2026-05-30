#include "gcps3/config.h"
#include "gcps3/gx.h"
#include "gcps3/log.h"
#include "gcps3/runtime.h"

int main(void)
{
    static const int16_t positions[][3] = {
        { -1, 1, 0 },
        { -1, -1, 0 },
        { 1, -1, 0 },
        { 1, 1, 0 }
    };
    static const uint8_t colors[][4] = {
        { 255, 64, 64, 255 },
        { 64, 255, 64, 255 },
        { 64, 128, 255, 255 },
        { 255, 224, 64, 255 }
    };
    Gcps3Config config = gcps3_config_default();
    GXColor clear_color;

    config.app_name = "gx_compat_indexed_quad";
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

    GXClearVtxDesc();
    GXSetVtxDesc(GX_ATTR_POSITION, GX_INDEX8);
    GXSetVtxDesc(GX_ATTR_COLOR0, GX_INDEX8);
    GX_SetArray(GX_ATTR_POSITION, positions, (uint8_t)sizeof(positions[0]));
    GX_SetArray(GX_ATTR_COLOR0, colors, (uint8_t)sizeof(colors[0]));

    GX_Begin(GX_QUADS, GX_VTXFMT0, 4);
    GX_Color1x8(0);
    GX_Position1x8(0);
    GX_Color1x8(1);
    GX_Position1x8(1);
    GX_Color1x8(2);
    GX_Position1x8(2);
    GX_Color1x8(3);
    GX_Position1x8(3);
    GX_End();

    GXShutdown();
    gcps3_runtime_shutdown();
    return 0;
}
