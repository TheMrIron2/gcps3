#include "gcps3/os.h"

#include "gcps3/log.h"

#include <stdarg.h>
#include <stdio.h>
#include <time.h>

static int s_os_initialized;

void OSInit(void)
{
    if (s_os_initialized) {
        GCPS3_LOG_WARN("os", "OSInit called more than once");
        return;
    }

    s_os_initialized = 1;
    GCPS3_LOG_INFO("os", "OS layer initialized");
}

void OSShutdown(void)
{
    if (!s_os_initialized) {
        return;
    }

    s_os_initialized = 0;
    GCPS3_LOG_INFO("os", "OS layer shut down");
}

OSTime OSGetTime(void)
{
    return (OSTime)time(NULL);
}

void OSReport(const char *fmt, ...)
{
    va_list args;

    fprintf(stdout, "[OSReport] ");
    va_start(args, fmt);
    vfprintf(stdout, fmt, args);
    va_end(args);
    fputc('\n', stdout);
}
