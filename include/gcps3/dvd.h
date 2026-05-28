#ifndef GCPS3_DVD_H
#define GCPS3_DVD_H

#ifdef __cplusplus
extern "C" {
#endif

void DVDInit(void);
void DVDShutdown(void);
void DVDSetRoot(const char *root_path);
const char *DVDGetRoot(void);

#ifdef __cplusplus
}
#endif

#endif
