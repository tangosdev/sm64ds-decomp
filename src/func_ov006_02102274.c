#include "types.h"
#pragma opt_common_subs off
extern int RandomIntInternal(void* seed);
extern int data_0209d4b8;
extern int func_020126e8(int a);
extern int func_02012468(int a, int b, int c, int d, int e, int f, int g, short h);

void func_ov006_02102274(char* p, int i)
{
    int v;

    if (*(u16*)(p + 0x5000 + (i << 6) + 0x292) != 0) {
        *(u16*)(p + 0x5292 + (i << 6)) -= 1;
        if (*(s16*)(p + 0x5000 + (i << 6) + 0x292) < 0)
            *(u16*)(p + 0x5000 + (i << 6) + 0x292) = 0;
        return;
    }

    *(u8*)(p + 0x5000 + (i << 6) + 0x296) = 1;

    v = ((u32)(((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 2) >> 15;
    if (i & 1)
        v = *(u8*)(p + 0x5298) ^ 1;
    *(u8*)(p + 0x5000 + (i << 6) + 0x298) = v;

    if (v != 0) {
        *(u8*)(p + 0x5000 + (i << 6) + 0x295) = 1;
        *(int*)(p + 0x5000 + (i << 6) + 0x260) = -0x10000;
        *(int*)(p + 0x5000 + (i << 6) + 0x268) = 0x200;
        if (*(int*)(p + 0x5668) == 1) {
            *(int*)(p + 0x5000 + (i << 6) + 0x268) = 0xf00;
            if (i != 0)
                *(u8*)(p + 0x5000 + (i << 6) + 0x29b) = *(u8*)(p + 0x529b);
            else
                *(u8*)(p + 0x5000 + (i << 6) + 0x29b) =
                    ((u32)(((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 2) >> 15;
        }
    } else {
        *(u8*)(p + 0x5000 + (i << 6) + 0x295) = 1;
        *(int*)(p + 0x5000 + (i << 6) + 0x260) = 0x110000;
        *(int*)(p + 0x5000 + (i << 6) + 0x268) = -0x200;
        if (*(int*)(p + 0x5668) == 1) {
            *(int*)(p + 0x5000 + (i << 6) + 0x268) = -0xf00;
            if (i != 0)
                *(u8*)(p + 0x5000 + (i << 6) + 0x29b) = *(u8*)(p + 0x529b);
            else
                *(u8*)(p + 0x5000 + (i << 6) + 0x29b) =
                    ((u32)(((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 2) >> 15;
        }
    }

    *(int*)(p + 0x5000 + (i << 6) + 0x264) = -0xf8000;
    *(u8*)(p + 0x5000 + (i << 6) + 0x299) = 0;
    *(int*)(p + 0x5000 + (i << 6) + 0x26c) = 0x2000;
    *(int*)(p + 0x5000 + (i << 6) + 0x28c) =
        func_02012468(*(int*)(p + 0x5000 + (i << 6) + 0x28c), 2, 0x198, 4, 0, 0,
                      func_020126e8(*(int*)(p + 0x5000 + (i << 6) + 0x260)), 0);
}
