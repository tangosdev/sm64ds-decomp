#include "types.h"
#pragma opt_common_subs off
extern int Sound_PlayIfNotActive(int a, int b, int c, int d);
extern int data_ov006_0212ec30[];

void func_ov006_021012cc(char* p, int i)
{
    int v1;
    int v2;
    u8 state;

    if (*(u16*)(p + 0x5200 + (i << 6) + 0x92) != 0) {
        (*(u16*)(p + 0x5292 + (i << 6)))--;
        return;
    }

    *(int*)(p + 0x5260 + (i << 6)) += *(int*)(p + 0x5000 + (i << 6) + 0x268);
    *(int*)(p + 0x5264 + (i << 6)) += *(int*)(p + 0x5000 + (i << 6) + 0x26c);
    v1 = *(int*)(p + 0x5000 + (i << 6) + 0x260) >> 12;
    state = *(u8*)(p + 0x5000 + (i << 6) + 0x29b);
    v2 = *(int*)(p + 0x5000 + (i << 6) + 0x264) >> 12;

    if (state == 0) {
        if (v1 >= 0xe0) {
            *(u16*)(p + 0x5200 + (i << 6) + 0x92) = 0x40;
            (*(u8*)(p + 0x529b + (i << 6)))++;
            *(int*)(p + 0x5000 + (i << 6) + 0x268) = 0;
            *(int*)(p + 0x5000 + (i << 6) + 0x26c) = 0;
            return;
        }
        if (v1 >= 0x80) {
            if (*(int*)(p + 0x5000 + (i << 6) + 0x268) >= 0x400)
                *(int*)(p + 0x5268 + (i << 6)) -= 8;
            if (i == 0) {
                if (*(int*)(p + 0x5000 + (i << 6) + 0x26c) <= 0xc00)
                    *(int*)(p + 0x526c + (i << 6)) += 0x10;
            } else {
                if (*(int*)(p + 0x5000 + (i << 6) + 0x26c) >= -0xc00)
                    *(int*)(p + 0x526c + (i << 6)) -= 0x10;
            }
        } else {
            if (*(int*)(p + 0x5000 + (i << 6) + 0x268) <= 0xc00)
                *(int*)(p + 0x5268 + (i << 6)) += 0x20;
            if (i == 0) {
                if (v2 != data_ov006_0212ec30[i]) {
                    if (*(int*)(p + 0x5000 + (i << 6) + 0x26c) >= -0xc00)
                        *(int*)(p + 0x526c + (i << 6)) -= 0x30;
                } else {
                    *(int*)(p + 0x5000 + (i << 6) + 0x26c) = 0;
                }
            } else {
                if (v2 != data_ov006_0212ec30[i]) {
                    if (*(int*)(p + 0x5000 + (i << 6) + 0x26c) <= 0xc00)
                        *(int*)(p + 0x526c + (i << 6)) += 0x30;
                } else {
                    *(int*)(p + 0x5000 + (i << 6) + 0x26c) = 0;
                }
            }
        }
    } else if (state == 1) {
        if (v1 <= 0x20) {
            *(u16*)(p + 0x5200 + (i << 6) + 0x92) = 0x40;
            *(u8*)(p + 0x5000 + (i << 6) + 0x29b) = 0;
            *(int*)(p + 0x5000 + (i << 6) + 0x268) = 0;
            *(int*)(p + 0x5000 + (i << 6) + 0x26c) = 0;
            return;
        }
        if (v1 <= 0x80) {
            if (*(int*)(p + 0x5000 + (i << 6) + 0x268) <= -0x400)
                *(int*)(p + 0x5268 + (i << 6)) += 8;
            if (i == 0) {
                if (*(int*)(p + 0x5000 + (i << 6) + 0x26c) <= 0xc00)
                    *(int*)(p + 0x526c + (i << 6)) += 0x10;
            } else {
                if (*(int*)(p + 0x5000 + (i << 6) + 0x26c) >= -0xc00)
                    *(int*)(p + 0x526c + (i << 6)) -= 0x10;
            }
        } else {
            if (*(int*)(p + 0x5000 + (i << 6) + 0x268) >= -0xc00)
                *(int*)(p + 0x5268 + (i << 6)) -= 0x20;
            if (i == 0) {
                if (v2 != data_ov006_0212ec30[i]) {
                    if (*(int*)(p + 0x5000 + (i << 6) + 0x26c) >= -0xc00)
                        *(int*)(p + 0x526c + (i << 6)) -= 0x30;
                } else {
                    *(int*)(p + 0x5000 + (i << 6) + 0x26c) = 0;
                }
            } else {
                if (v2 != data_ov006_0212ec30[i]) {
                    if (*(int*)(p + 0x5000 + (i << 6) + 0x26c) <= 0xc00)
                        *(int*)(p + 0x526c + (i << 6)) += 0x30;
                } else {
                    *(int*)(p + 0x5000 + (i << 6) + 0x26c) = 0;
                }
            }
        }
    }

    *(int*)(p + 0x5000 + (i << 6) + 0x28c) = Sound_PlayIfNotActive(*(int*)(p + 0x5000 + (i << 6) + 0x28c), 2, 0x198, 0);
}
