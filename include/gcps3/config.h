#ifndef GCPS3_CONFIG_H
#define GCPS3_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Gcps3Config {
    const char *app_name;
    const char *asset_root;
} Gcps3Config;

Gcps3Config gcps3_config_default(void);

#ifdef __cplusplus
}
#endif

#endif
