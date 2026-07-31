#include "types.h"
extern int RandomIntInternal(int *seed);
extern int data_0209d4b8;
extern void func_ov006_0210446c(char *o, int x, int y, int mode);
extern void SetBg2Offset(int x, int y);
extern void SetSubBg2Offset(int x, int y);

#pragma opt_common_subs off

void func_ov006_02104580(char *o)
{
    int x;
    int y;
    u32 r;
    u32 r2;

    if (*(u8 *)(o + 0x46a5) == 0) {
        *(u8 *)(((int)o + 0x46a6)) += 1;
        if (((*(u8 *)(o + 0x46a6) >> 1) & 1) != 0)
            *(int *)(o + 0x4694) = 0x2000;
        else
            *(int *)(o + 0x4694) = -0x2000;

        if (*(u8 *)(o + 0x46a7) != 0) {
            *(u8 *)(((int)o + 0x46a7)) -= 1;
        } else {
            r = ((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
            x = ((r * 7) >> 15) * 5;
            r = ((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff;
            y = ((r << 4) >> 15) + 0xb0;
            func_ov006_0210446c(o, x << 12, y << 12, 0);

            r = (u32)RandomIntInternal(&data_0209d4b8);
            r2 = (u32)RandomIntInternal(&data_0209d4b8);
            x = (((r >> 16 & 0x7fff) * 7) >> 15) * 5;
            y = ((((r2 >> 16) & 0x7fff) << 4) >> 15) + 0xb0;
            func_ov006_0210446c(o, (0x100 - x) << 12, y << 12, 0);
            *(u8 *)(o + 0x46a7) = 3;
        }

        if (*(u8 *)(o + 0x46a6) >= 0x3c) {
            *(u8 *)(((int)o + 0x46a5)) += 1;
            *(u8 *)(o + 0x46a6) = 0;
            *(u8 *)(o + 0x46a7) = 0;
            *(int *)(o + 0x4694) = 0;
            *(int *)(o + 0x46a0) = 0x1000;
        }
    } else if (*(int *)(o + 0x4698) < 0x200000) {
        *(int *)(((int)o + 0x4698)) += *(int *)(o + 0x46a0);
        *(int *)(((int)o + 0x46a0)) += 0x200;
        if (*(int *)(o + 0x4698) >= 0x200000)
            *(int *)(o + 0x4698) = 0x200000;

        if (*(u8 *)(o + 0x46a7) != 0) {
            *(u8 *)(((int)o + 0x46a7)) -= 1;
        } else {
            y = *(int *)(o + 0x4698) >> 12;
            if (y >= 0 && y <= 0xc0)
                y = (0xc0 - y) << 12;
            else if (y >= 0x100)
                y = (-0x20 - (y - 0x100)) << 12;

            x = (((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 7) >> 15) * 5;
            func_ov006_0210446c(o, x << 12, y, 1);
            x = (((((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff) * 7) >> 15) * 5;
            func_ov006_0210446c(o, (0x100 - x) << 12, y, 1);
            *(u8 *)(o + 0x46a7) = 2;
        }
    }

    x = *(int *)(o + 0x4694) >> 12;
    y = *(int *)(o + 0x4698) >> 12;
    SetBg2Offset(x, y);
    if (y >= 0x100)
        y = 0x100;
    SetSubBg2Offset(x, y);
}
