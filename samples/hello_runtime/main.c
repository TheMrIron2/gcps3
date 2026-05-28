#include "gcps3/config.h"
#include "gcps3/dvd.h"
#include "gcps3/log.h"
#include "gcps3/os.h"
#include "gcps3/pad.h"
#include "gcps3/runtime.h"

int main(void)
{
    Gcps3Config config = gcps3_config_default();
    PADStatus pads[4];
    int pad_count;

    config.app_name = "hello_runtime";
    config.asset_root = "assets";

    if (gcps3_runtime_init(&config) != 0) {
        GCPS3_LOG_ERROR("sample", "failed to initialize runtime");
        return 1;
    }

    GCPS3_LOG_INFO("sample", "hello from the host runtime");
    OSReport("OS time: %llu", (unsigned long long)OSGetTime());
    OSReport("DVD root: %s", DVDGetRoot());

    pad_count = PADRead(pads, 4);
    GCPS3_LOG_INFO("sample", "PADRead returned %d empty controller states", pad_count);

    gcps3_runtime_shutdown();
    return 0;
}
