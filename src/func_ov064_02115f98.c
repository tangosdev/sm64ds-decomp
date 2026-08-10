#include "types.h"
extern s16 data_02082214[];
extern void Vec3_Sub(int* out, int* a, int* b);
extern int Vec3_HorzLen(int* v);
extern int _ZN4cstd5atan2E5Fix12IiES1_(int a, int b);
extern void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(unsigned effect, int x, int y, int z);

void func_ov064_02115f98(char* a0, char* a1)
{
    int scratch[3];
    int pos0[3];
    int pos1[3];
    int diff[3];
    int scale;
    int angH;
    int angV;
    int idxV, idxH;
    s16 sinV, cosV, sinH, cosH;
    int lenfix, ycomp, t0, t1;
    int y0;
    int* p1;
    int tmpy;

    p1 = (int*)(a1 + 0x5c);
    pos0[0] = *(int*)(a0 + 0x5c);
    y0 = *(int*)(a0 + 0x60);
    pos0[1] = y0;
    pos0[2] = *(int*)(a0 + 0x64);
    pos1[0] = *p1;
    tmpy = p1[1];
    pos1[1] = tmpy;
    pos1[2] = p1[2];
    tmpy = tmpy + 0x46000;

    scale = *(int*)(*(int*)(a0 + 0x330) + 0x14);
    pos0[1] = y0 + *(int*)(a0 + 0x3ec);
    pos1[1] = tmpy;

    Vec3_Sub(diff, pos1, pos0);

    scratch[0] = diff[0];
    scratch[1] = diff[1];
    scratch[2] = diff[2];
    angH = _ZN4cstd5atan2E5Fix12IiES1_(scratch[0], scratch[2]);
    {
        int hl = Vec3_HorzLen(scratch);
        angV = _ZN4cstd5atan2E5Fix12IiES1_(scratch[1], hl);
    }

    idxV = (int)((u16)angV >> 4);
    idxH = (int)((u16)angH >> 4);

    sinV = data_02082214[idxV * 2 + 1];
    cosV = data_02082214[idxV * 2];
    sinH = data_02082214[idxH * 2 + 1];
    cosH = data_02082214[idxH * 2];

    {
        s64 p = (s64)scale * sinV;
        lenfix = (int)((p + 0x800) >> 12);
    }
    {
        s64 p = (s64)scale * cosV;
        ycomp = (int)((p + 0x800) >> 12);
    }
    {
        s64 p = (s64)lenfix * cosH;
        t0 = (int)((p + 0x800) >> 12);
    }
    {
        s64 p = (s64)lenfix * sinH;
        t1 = (int)((p + 0x800) >> 12);
    }

    pos0[0] = pos0[0] + t0;
    pos0[1] = pos0[1] + ycomp;
    pos0[2] = pos0[2] + t1;

    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0xf6, pos0[0], pos0[1], pos0[2]);
}
