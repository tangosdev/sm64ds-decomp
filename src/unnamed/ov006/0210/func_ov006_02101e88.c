#include "types.h"
#pragma opt_common_subs off
extern int RandomIntInternal(int* seed);
extern int Sound_PlayIfNotActive(int a, int b, int c, int d);
extern int* data_ov006_0213db84[];
extern int data_0209d4b8;

void func_ov006_02101e88(char* p, int i)
{
    int v;
    u8 flag;

    *(int*)(p + 0x5260 + (i << 6)) += *(int*)(p + 0x5000 + (i << 6) + 0x268);
    flag = *(u8*)(p + 0x5000 + (i << 6) + 0x298);
    v = *(int*)(p + 0x5000 + (i << 6) + 0x260) >> 12;

    if (flag != 0) {
        if (v >= 0x80) {
            if (*(int*)(p + 0x5000 + (i << 6) + 0x268) >= 0x400)
                *(int*)(p + 0x5268 + (i << 6)) -= 8;
        } else {
            int idx = *(int*)(p + 0x5668);
            if (*(int*)(p + 0x5000 + (i << 6) + 0x268) <= data_ov006_0213db84[idx][i])
                *(int*)(p + 0x5268 + (i << 6)) += 0x80;
        }
        if (v >= 0xe0) {
            u32 a;
            *(int*)(p + 0x5000 + (i << 6) + 0x260) = 0xe0000;
            *(int*)(p + 0x5000 + (i << 6) + 0x268) = 0;
            *(u8*)(p + 0x5298 + (i << 6)) ^= 1;
            a = ((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
            *(u16*)(p + (i << 6) + 0x5292) = (u16)(((a << 3) >> 15) * 3 + 0x40);
            *(u8*)(p + 0x5000 + (i << 6) + 0x296) = 7;
        }
    } else {
        if (v <= 0x80) {
            if (*(int*)(p + 0x5000 + (i << 6) + 0x268) <= -0x400)
                *(int*)(p + 0x5268 + (i << 6)) += 8;
        } else {
            int idx = *(int*)(p + 0x5668);
            if (*(int*)(p + 0x5000 + (i << 6) + 0x268) >= -data_ov006_0213db84[idx][i])
                *(int*)(p + 0x5268 + (i << 6)) -= 0x80;
        }
        if (v <= 0x20) {
            u32 a;
            *(int*)(p + 0x5000 + (i << 6) + 0x260) = 0x20000;
            *(int*)(p + 0x5000 + (i << 6) + 0x268) = 0;
            *(u8*)(p + 0x5298 + (i << 6)) ^= 1;
            a = ((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
            *(u16*)(p + (i << 6) + 0x5292) = (u16)(((a << 3) >> 15) * 3 + 0x40);
            *(u8*)(p + 0x5000 + (i << 6) + 0x296) = 7;
        }
    }

    *(int*)(p + 0x5000 + (i << 6) + 0x28c) = Sound_PlayIfNotActive(*(int*)(p + 0x5000 + (i << 6) + 0x28c), 2, 0x198, 0);
}
