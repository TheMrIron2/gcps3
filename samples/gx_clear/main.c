#include "gcps3/config.h"
#include "gcps3/gx.h"
#include "gcps3/log.h"
#include "gcps3/runtime.h"

int main(void)
{
    Gcps3Config config = gcps3_config_default();
    GXColor clear_color;

    config.app_name = "gx_clear";
    config.asset_root = "assets";

    if (gcps3_runtime_init(&config) != 0) {
        GCPS3_LOG_ERROR("sample", "failed to initialize runtime");
        return 1;
    }

    clear_color.r = 32;
    clear_color.g = 64;
    clear_color.b = 96;
    clear_color.a = 255;

    GXInit();
    GXSetViewport(0.0f, 0.0f, 640.0f, 480.0f, 0.0f, 1.0f);
    GXSetClearColor(clear_color);
    GXClear();
    GXShutdown();

    gcps3_runtime_shutdown();
    return 0;
}
