#include "types.h"
#pragma opt_common_subs off
extern int data_ov006_0212ec80[];

void func_ov006_021009b8(char *p, int i)
{
    int v;
    u8 state;

    if (*(u16 *)(p + 0x5200 + (i << 6) + 0x92) != 0) {
        (*(u16 *)(p + 0x5292 + (i << 6)))--;
        if (*(short *)(p + 0x5200 + (i << 6) + 0x92) > 0)
            return;
        *(int *)(p + 0x5000 + (i << 6) + 0x26c) = 0;
        *(int *)(p + 0x5000 + (i << 6) + 0x268) = data_ov006_0212ec80[i];
        *(u8 *)(p + 0x5000 + (i << 6) + 0x298) = i;
        return;
    }

    *(int *)(p + 0x5260 + (i << 6)) += *(int *)(p + 0x5000 + (i << 6) + 0x268);
    *(int *)(p + 0x5264 + (i << 6)) += *(int *)(p + 0x5000 + (i << 6) + 0x26c);
    *(int *)(p + 0x526c + (i << 6)) -= 0x20;
    state = *(u8 *)(p + 0x5000 + (i << 6) + 0x298);
    if (state == 0) {
        if (*(int *)(p + 0x5000 + (i << 6) + 0x268) <= 0xc00)
            *(int *)(p + 0x5268 + (i << 6)) += 0x80;
    } else if (state == 1) {
        if (*(int *)(p + 0x5000 + (i << 6) + 0x268) >= -0xc00)
            *(int *)(p + 0x5268 + (i << 6)) -= 0x80;
    }

    v = *(int *)(p + 0x5000 + (i << 6) + 0x264) >> 12;
    if (v <= -0x120) {
        *(u8 *)(p + 0x5000 + (i << 6) + 0x294) = 0;
        *(u8 *)(p + 0x5000 + (i << 6) + 0x295) = 0;
    }
}
