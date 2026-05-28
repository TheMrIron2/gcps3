#include "gcps3/pad.h"

#include "gcps3/log.h"

#include <stddef.h>
#include <string.h>

static int s_pad_initialized;

void PADInit(void)
{
    if (s_pad_initialized) {
        GCPS3_LOG_WARN("pad", "PADInit called more than once");
        return;
    }

    s_pad_initialized = 1;
    GCPS3_LOG_INFO("pad", "PAD layer initialized");
}

void PADShutdown(void)
{
    if (!s_pad_initialized) {
        return;
    }

    s_pad_initialized = 0;
    GCPS3_LOG_INFO("pad", "PAD layer shut down");
}

int PADRead(PADStatus *status, int count)
{
    if (!status || count <= 0) {
        return 0;
    }

    memset(status, 0, sizeof(*status) * (size_t)count);
    return count;
}
