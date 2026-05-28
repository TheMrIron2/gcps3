#include "gcps3/dvd.h"

#include "gcps3/log.h"

static const char *s_dvd_root = ".";
static int s_dvd_initialized;

void DVDInit(void)
{
    if (s_dvd_initialized) {
        GCPS3_LOG_WARN("dvd", "DVDInit called more than once");
        return;
    }

    s_dvd_initialized = 1;
    GCPS3_LOG_INFO("dvd", "DVD layer initialized");
}

void DVDShutdown(void)
{
    if (!s_dvd_initialized) {
        return;
    }

    s_dvd_initialized = 0;
    GCPS3_LOG_INFO("dvd", "DVD layer shut down");
}

void DVDSetRoot(const char *root_path)
{
    s_dvd_root = root_path ? root_path : ".";
    GCPS3_LOG_INFO("dvd", "asset root set to %s", s_dvd_root);
}

const char *DVDGetRoot(void)
{
    return s_dvd_root;
}
