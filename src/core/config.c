#include "gcps3/config.h"

Gcps3Config gcps3_config_default(void)
{
    Gcps3Config config;
    config.app_name = "gcps3";
    config.asset_root = ".";
    return config;
}
