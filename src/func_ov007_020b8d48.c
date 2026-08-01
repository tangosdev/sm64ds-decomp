#include "types.h"
extern u32 data_ov007_02102dd8;
extern u32 data_ov007_02103464;
extern u8 data_ov007_02102df8[];
extern u16 data_ov007_02103458;
extern u32 data_ov007_02103468;
extern u32 *data_ov007_0210345c;
extern u32 *data_ov007_02103478[];
extern u32 data_ov007_020d7ac4[];
extern u32 data_ov007_020d7aa0[];

#pragma opt_loop_invariants off

void func_ov007_020b8d48(int a, int c)
{
    int w;
    u32 pal;
    int adj;
    int pos;
    int row, col;
    u16 tile;
    u16 t;
    int i;
    u32 *dst;
    u32 palmask;
    u32 *src;
    int rshift;
    u32 mask;
    int q, rem;
    int shift;

    pal = data_ov007_02102dd8;
    if (c == 0xf0 || c == 0xee)
        pal = 0xd;

    if (c == 0x4d) {
        data_ov007_02103464 += 4;
        return;
    }

    if (c == 0x4d)
        w = 4;
    else
        w = data_ov007_02102df8[c];

    adj = 0;
    if (c == 1)
        adj += 2;
    w -= adj;
    pos = (int)data_ov007_02103464 + adj;
    data_ov007_02103464 = pos;
    if (pos >= 0x100 - w)
        return;

    row = c >> 5;
    col = c - (row << 5);
    tile = (u16)(col + (row << 6));
    t = (u16)(data_ov007_02103458 + tile);

    q = pos >> 3;
    rem = pos - (q << 3);

    dst = data_ov007_02103478[a] + (q + data_ov007_02103468 * 32) * 8;
    src = data_ov007_0210345c + t * 8;

    shift = rem * 4;
    rshift = 0x20 - shift;
    palmask = data_ov007_020d7ac4[pal];
    mask = data_ov007_020d7aa0[rem];

    for (i = 0; i < 16; i++) {
        u32 s = *src++ & palmask;
        *dst = (*dst & mask) | (s << shift);
        if (rem != 0) {
            u32 *d2 = (u32 *)(int)(((long long)(int)(dst + 8)));
            *d2 = (*d2 & ~mask) | (s >> rshift);
        }
        dst++;
        if (i == 7) {
            dst += 0xf8;
            src += 0xf8;
        }
    }

    data_ov007_02103464 += w;
}
