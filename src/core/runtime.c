#include "gcps3/runtime.h"

#include "gcps3/dvd.h"
#include "gcps3/log.h"
#include "gcps3/os.h"
#include "gcps3/pad.h"

static int s_runtime_initialized;

int gcps3_runtime_init(const Gcps3Config *config)
{
    Gcps3Config resolved_config;

    if (s_runtime_initialized) {
        GCPS3_LOG_WARN("runtime", "runtime already initialized");
        return 0;
    }

    resolved_config = config ? *config : gcps3_config_default();
    GCPS3_LOG_INFO("runtime", "starting %s", resolved_config.app_name ? resolved_config.app_name : "gcps3");

    OSInit();
    PADInit();
    DVDInit();
    DVDSetRoot(resolved_config.asset_root);

    s_runtime_initialized = 1;
    GCPS3_LOG_INFO("runtime", "runtime initialized");
    return 0;
}

void gcps3_runtime_shutdown(void)
{
    if (!s_runtime_initialized) {
        return;
    }

    DVDShutdown();
    PADShutdown();
    OSShutdown();

    s_runtime_initialized = 0;
    GCPS3_LOG_INFO("runtime", "runtime shut down");
}
