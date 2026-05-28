#ifndef GCPS3_PAD_H
#define GCPS3_PAD_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PADStatus {
    uint16_t button;
    int8_t stick_x;
    int8_t stick_y;
    int8_t substick_x;
    int8_t substick_y;
    uint8_t trigger_left;
    uint8_t trigger_right;
} PADStatus;

void PADInit(void);
void PADShutdown(void);
int PADRead(PADStatus *status, int count);

#ifdef __cplusplus
}
#endif

#endif
