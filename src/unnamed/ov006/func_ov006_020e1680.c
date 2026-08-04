#include "types.h"
#pragma opt_strength_reduction off

extern int _ZN4cstd4sqrtEy(u64);
extern u8 data_ov006_0212e458[];

#define M(p) ((int *)(int)((long long)(int)(p)))

void func_ov006_020e1680(char *o)
{
    int slot = 0;
    int i, j;
    char *e;
    u8 *p;

    for (j = 0; j < 5; j++) {
        if (*(u8 *)(o + j * 16 + 0x4748) == 0) {
            slot = j;
            break;
        }
    }

    p = &data_ov006_0212e458[slot];
    e = o;
    for (i = 0; i < 5; i++, e += 0x2c) {
        int dx, dz, d, t;

        if (*(u8 *)(e + 0x4689) == 0)
            continue;
        dx = *(int *)(e + 0x4660) - *(int *)(o + 0x4e94);
        dz = *(int *)(e + 0x4664) - *(int *)(o + 0x4e98);
        {
            int ax = dx >> 12;
            int az = dz >> 12;
            d = _ZN4cstd4sqrtEy((u64)(s64)(ax * ax + az * az));
        }
        *(u8 *)(o + slot * 16 + 0x4748) = 1;
        *(int *)(o + slot * 16 + 0x473c) = *(int *)(e + 0x4660);
        {
        int *py = M(o + slot * 16 + 0x4740);
        *py = *(int *)(e + 0x4664) + 0x1000;
        *(short *)(o + slot * 16 + 0x4746) = *p;
        if (d <= 8)
            *(short *)(o + slot * 16 + 0x4744) = 1000;
        else if (d <= 0x18)
            *(short *)(o + slot * 16 + 0x4744) = 500;
        else if (d <= 0x28)
            *(short *)(o + slot * 16 + 0x4744) = 300;
        else if (d <= 0x38)
            *(short *)(o + slot * 16 + 0x4744) = 100;
        else
            *(short *)(o + slot * 16 + 0x4744) = 0;
        t = *py >> 12;
        if (t >= -32 && t <= 8)
            *py = -0x28000;
        }
        p++;
        slot++;
    }
}
