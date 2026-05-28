#ifndef GCPS3_LOG_H
#define GCPS3_LOG_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum Gcps3LogLevel {
    GCPS3_LOG_TRACE = 0,
    GCPS3_LOG_INFO,
    GCPS3_LOG_WARN,
    GCPS3_LOG_ERROR
} Gcps3LogLevel;

void gcps3_log_set_level(Gcps3LogLevel level);
Gcps3LogLevel gcps3_log_get_level(void);
void gcps3_log(Gcps3LogLevel level, const char *category, const char *fmt, ...);

#define GCPS3_LOG_TRACE(category, ...) gcps3_log(GCPS3_LOG_TRACE, category, __VA_ARGS__)
#define GCPS3_LOG_INFO(category, ...) gcps3_log(GCPS3_LOG_INFO, category, __VA_ARGS__)
#define GCPS3_LOG_WARN(category, ...) gcps3_log(GCPS3_LOG_WARN, category, __VA_ARGS__)
#define GCPS3_LOG_ERROR(category, ...) gcps3_log(GCPS3_LOG_ERROR, category, __VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif
