#include "gcps3/config.h"
#include "gcps3/gx.h"
#include "gcps3/log.h"
#include "gcps3/runtime.h"

int main(void)
{
    Gcps3Config config = gcps3_config_default();
    unsigned int i;

    config.app_name = "gx_validation";
    config.asset_root = "assets";

    if (gcps3_runtime_init(&config) != 0) {
        GCPS3_LOG_ERROR("sample", "failed to initialize runtime");
        return 1;
    }

    GXInit();

    GXPosition3f32(0.0f, 0.0f, 0.0f);
    GXColor4u8(255, 255, 255, 255);
    GXEnd();

    GXBeginTriangles();
    GXBeginTriangles();
    GXColor4u8(255, 0, 0, 255);
    GXPosition3f32(-0.5f, -0.5f, 0.0f);
    GXPosition3f32(0.5f, -0.5f, 0.0f);
    GXEnd();

    GXBeginTriangles();
    GXColor4u8(0, 255, 0, 255);
    for (i = 0; i < 66u; i++) {
        GXPosition3f32((float)i, 0.0f, 0.0f);
    }

    GXShutdown();
    gcps3_runtime_shutdown();
    return 0;
}
