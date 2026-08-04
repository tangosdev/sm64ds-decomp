#include "types.h"
#pragma opt_common_subs off
extern int Sound_PlayIfNotActive(int a, int b, int c, int d);

void func_ov006_021016ec(char* p, int i)
{
    int v;
    u8 flag;

    if (*(u16*)(p + (i << 6) + 0x5292) != 0) {
        *(u16*)(p + 0x5292 + (i << 6)) -= 1;
        return;
    }

    *(int*)(p + 0x5260 + (i << 6)) += *(int*)(p + 0x5000 + (i << 6) + 0x268);
    *(int*)(p + 0x5264 + (i << 6)) += *(int*)(p + 0x5000 + (i << 6) + 0x26c);
    flag = *(u8*)(p + 0x5000 + (i << 6) + 0x298);
    v = *(int*)(p + 0x5000 + (i << 6) + 0x260) >> 12;

    if (flag != 0) {
        if (v >= 0x80) {
            if (*(int*)(p + 0x5000 + (i << 6) + 0x268) >= 0x400)
                *(int*)(p + 0x5268 + (i << 6)) -= 8;
        } else {
            if (*(int*)(p + 0x5000 + (i << 6) + 0x268) <= 0x1000)
                *(int*)(p + 0x5268 + (i << 6)) += 0x80;
            if (*(u8*)(p + 0x5000 + (i << 6) + 0x29b) == 0) {
                if (*(int*)(p + 0x5000 + (i << 6) + 0x26c) <= 0x1000)
                    *(int*)(p + 0x526c + (i << 6)) += 0xc;
            } else {
                if (*(int*)(p + 0x5000 + (i << 6) + 0x26c) >= -0x1000)
                    *(int*)(p + 0x526c + (i << 6)) -= 0xc;
            }
        }
        if (v >= 0xe0) {
            *(int*)(p + 0x5000 + (i << 6) + 0x260) = 0xe0000;
            *(int*)(p + 0x5000 + (i << 6) + 0x268) = 0;
            *(int*)(p + 0x5000 + (i << 6) + 0x26c) = 0;
            *(u8*)(p + 0x5298 + (i << 6)) ^= 1;
            *(u8*)(p + 0x529b + (i << 6)) ^= 1;
            *(u16*)(p + (i << 6) + 0x5292) = 0x40;
        }
    } else {
        if (v <= 0x80) {
            if (*(int*)(p + 0x5000 + (i << 6) + 0x268) <= -0x400)
                *(int*)(p + 0x5268 + (i << 6)) += 8;
        } else {
            if (*(int*)(p + 0x5000 + (i << 6) + 0x268) >= -0x1000)
                *(int*)(p + 0x5268 + (i << 6)) -= 0x80;
            if (*(u8*)(p + 0x5000 + (i << 6) + 0x29b) == 0) {
                if (*(int*)(p + 0x5000 + (i << 6) + 0x26c) <= 0x1000)
                    *(int*)(p + 0x526c + (i << 6)) += 0xc;
            } else {
                if (*(int*)(p + 0x5000 + (i << 6) + 0x26c) >= -0x1000)
                    *(int*)(p + 0x526c + (i << 6)) -= 0xc;
            }
        }
        if (v <= 0x20) {
            *(int*)(p + 0x5000 + (i << 6) + 0x260) = 0x20000;
            *(int*)(p + 0x5000 + (i << 6) + 0x268) = 0;
            *(int*)(p + 0x5000 + (i << 6) + 0x26c) = 0;
            *(u8*)(p + 0x5298 + (i << 6)) ^= 1;
            *(u8*)(p + 0x529b + (i << 6)) ^= 1;
            *(u16*)(p + (i << 6) + 0x5292) = 0x40;
        }
    }

    *(int*)(p + 0x5000 + (i << 6) + 0x28c) = Sound_PlayIfNotActive(*(int*)(p + 0x5000 + (i << 6) + 0x28c), 2, 0x198, 0);
}
