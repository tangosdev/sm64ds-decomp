#include "types.h"
extern int _Z15ApproachLinear2Riii(int *p, int a, int b);
extern int _Z14ApproachLinearRiii(int *p, int a, int b);
extern void func_ov006_020cf124(char *c);
extern int data_020a0db0;
extern s16 data_02082214[];

#define LA(p) ((int)(p))

void func_ov006_020cf820(char *c)
{
    int vx, vz;
    int negS;
    int dx, dz;
    u16 ang;
    int j, i;
    int target, step;
    int *src, *dst, *val;

    if ((data_020a0db0 & 1) != 0) {
        if (_Z15ApproachLinear2Riii((int *)(c + 0x329), 0, 1) != 0) {
            *(u8 *)(c + 0x328) = 0;
        }
    }

    {
        u16 *fieldPtr = (u16 *)LA(c + 0x320);
        *fieldPtr = *fieldPtr + *(u16 *)(c + 0x322);
        ang = *(u16 *)(c + 0x320);
    }

    vx = *(int *)(c + 0x14);
    vz = *(int *)(c + 0x18);

    negS = -(int)data_02082214[(ang >> 4) * 2];

    dx = (int)(((s64)vx * negS + 0x800) >> 12);
    dz = (int)(((s64)vz * negS + 0x800) >> 12);

    src = (int *)(c + 0x11c);
    dst = (int *)(c + 0x5c);
    val = (int *)(c + 0x29c);

    target = 0;
    step = 0x100;

    for (j = 0; j < 4; j++) {
        for (i = 0; i < 4; i++) {
            dst[0] = src[0];
            dst[1] = src[1];
            dst[2] = src[2];
            dst[0] = dst[0] + (int)(((s64)*val * dx + 0x800) >> 12);
            dst[1] = dst[1] + (int)(((s64)*val * dz + 0x800) >> 12);
            _Z14ApproachLinearRiii(val, target, step);
            src += 3;
            dst += 3;
            val += 1;
        }
    }

    func_ov006_020cf124(c);
}
