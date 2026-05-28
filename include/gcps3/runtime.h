#ifndef GCPS3_RUNTIME_H
#define GCPS3_RUNTIME_H

#include "gcps3/config.h"

#ifdef __cplusplus
extern "C" {
#endif

int gcps3_runtime_init(const Gcps3Config *config);
void gcps3_runtime_shutdown(void);

#ifdef __cplusplus
}
#endif

#endif
