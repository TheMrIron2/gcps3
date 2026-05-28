#include "gcps3/log.h"

#include <stdarg.h>
#include <stdio.h>

static Gcps3LogLevel s_log_level = GCPS3_LOG_INFO;

static const char *level_name(Gcps3LogLevel level)
{
    switch (level) {
    case GCPS3_LOG_TRACE:
        return "trace";
    case GCPS3_LOG_INFO:
        return "info";
    case GCPS3_LOG_WARN:
        return "warn";
    case GCPS3_LOG_ERROR:
        return "error";
    default:
        return "unknown";
    }
}

void gcps3_log_set_level(Gcps3LogLevel level)
{
    s_log_level = level;
}

Gcps3LogLevel gcps3_log_get_level(void)
{
    return s_log_level;
}

void gcps3_log(Gcps3LogLevel level, const char *category, const char *fmt, ...)
{
    va_list args;
    FILE *stream;

    if (level < s_log_level) {
        return;
    }

    stream = level >= GCPS3_LOG_WARN ? stderr : stdout;
    fprintf(stream, "[gcps3] [%s] [%s] ", level_name(level), category ? category : "core");

    va_start(args, fmt);
    vfprintf(stream, fmt, args);
    va_end(args);

    fputc('\n', stream);
}
