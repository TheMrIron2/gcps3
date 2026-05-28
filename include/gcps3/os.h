#ifndef GCPS3_OS_H
#define GCPS3_OS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint64_t OSTime;

void OSInit(void);
void OSShutdown(void);
OSTime OSGetTime(void);
void OSReport(const char *fmt, ...);

#ifdef __cplusplus
}
#endif

#endif
