#include "types.h"
extern int RandomIntInternal(int *seed);
extern int data_0209d4b8;

#pragma opt_common_subs off

void func_ov006_0210371c(char *c, int i)
{
    int old;
    int v;
    int w;

    *(int *)(c + 0x4664 + i * 0x40) += *(int *)(c + i * 0x40 + 0x4000 + 0x66c);
    old = *(int *)(c + i * 0x40 + 0x4000 + 0x66c);
    *(int *)(c + 0x466c + i * 0x40) += 0x200;

    if (old < 0 && *(int *)(c + i * 0x40 + 0x4000 + 0x66c) >= 0) {
        *(u16 *)(c + i * 0x40 + 0x4600 + 0x90) = 0;
        v = *(int *)(c + i * 0x40 + 0x4000 + 0x660) >> 12;
        if (v > 0x80) {
            *(u8 *)(c + i * 0x40 + 0x4000 + 0x69e) = 1;
            *(u8 *)(c + i * 0x40 + 0x4000 + 0x69c) = 5;
            *(int *)(c + i * 0x40 + 0x4000 + 0x668) = 0x1000;
        } else if (v < 0x80) {
            *(u8 *)(c + i * 0x40 + 0x4000 + 0x69e) = 0;
            *(u8 *)(c + i * 0x40 + 0x4000 + 0x69c) = 1;
            *(int *)(c + i * 0x40 + 0x4000 + 0x668) = -0x1000;
        } else {
            unsigned int r = ((unsigned int)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
            if (((r << 1) >> 15) != 0) {
                *(u8 *)(c + i * 0x40 + 0x4000 + 0x69e) = 1;
                *(u8 *)(c + i * 0x40 + 0x4000 + 0x69c) = 5;
                *(int *)(c + i * 0x40 + 0x4000 + 0x668) = 0x1000;
            } else {
                *(u8 *)(c + i * 0x40 + 0x4000 + 0x69e) = 0;
                *(u8 *)(c + i * 0x40 + 0x4000 + 0x69c) = 1;
                *(int *)(c + i * 0x40 + 0x4000 + 0x668) = -0x1000;
            }
        }
    }

    w = *(int *)(c + i * 0x40 + 0x4000 + 0x664) >> 12;
    if (w >= 0xa8) {
        *(int *)(c + i * 0x40 + 0x4000 + 0x664) = 0xa8000;
        *(u8 *)(c + i * 0x40 + 0x4000 + 0x699) = 5;
    }
}
