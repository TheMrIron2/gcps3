#ifndef GCPS3_BACKEND_H
#define GCPS3_BACKEND_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum Gcps3BackendKind {
    GCPS3_BACKEND_KIND_HOST = 0,
    GCPS3_BACKEND_KIND_PS3 = 1
} Gcps3BackendKind;

const char *gcps3_backend_name(void);
Gcps3BackendKind gcps3_backend_kind(void);

#ifdef __cplusplus
}
#endif

#endif
