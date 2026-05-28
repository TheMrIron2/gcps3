#include "gcps3/backend.h"

const char *gcps3_backend_name(void)
{
    return "host";
}

Gcps3BackendKind gcps3_backend_kind(void)
{
    return GCPS3_BACKEND_KIND_HOST;
}
