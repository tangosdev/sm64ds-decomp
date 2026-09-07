#include "types.h"
#pragma opt_strength_reduction off
void func_ov007_020c20b8(u16 *p, u16 *src)
{
    int flags[13];
    int i;
    u16 keys;
    s8 *b;
    int za;
    int zb;

    i = 0;
    do { flags[i] = 0; i = i + 1; } while (i < 0xc);
    p[1] = *p;
    if (src == 0) {
        *p = ((*(volatile u16 *)0x4000130 | *(volatile u16 *)0x27fffa8) ^ 0x2fff) & 0x2fff;
    } else { *p = *src; }
    if ((*(u32 *)(p + 0x32) & 1) != 0) {
        if (*p == 0) {
            u32 w = *(volatile u32 *)(p + 0x32);
            *(u32 *)(p + 0x32) = w & ~1u;
        } else { *p = 0; }
    }
    b = (s8 *)p;
    i = 0;
    b[0x69] = b[0x68]; b[0x6b] = b[0x6a]; b[0x6d] = b[0x6c];
    b[0x6c] = (s8)i; b[0x6a] = b[0x6c]; b[0x68] = b[0x6a];
    if ((*p & 0x40) != 0) { b[0x6a] = 1; flags[0] = 1; }
    if ((*p & 0x80) != 0) { b[0x6a] = -1; flags[1] = 1; }
    if ((*p & 0x10) != 0) { b[0x68] = 1; flags[2] = 1; }
    if ((*p & 0x20) != 0) { b[0x68] = -1; flags[3] = 1; }
    if ((*p & 0x200) != 0) { b[0x6c] = 1; flags[8] = 1; }
    if ((*p & 0x100) != 0) { b[0x6c] = -1; flags[9] = 1; }
    keys = *p; i = 0;
    flags[4] = keys & 1; flags[5] = keys & 2; flags[6] = keys & 0x400;
    flags[7] = keys & 0x800; flags[10] = keys & 8; flags[11] = keys & 4;
    za = i; zb = i;
    do {
        int off = i << 2;
        if (flags[i] != 0) {

            int *h;
            h = (int *)((int)p - (-off) + 4);
            off = (int)p - (-off);
            *h = *h + 1;
            *(int *)(off + 0x34) = zb;

        } else {
            char *row = (char *)p + off;
            int *r;
            *(int *)(row + 4) = za;
            r = (int *)(row + 0x34);
            *r = *r + 1;
        }
        i = i + 1;
    } while (i < 0xc);
}
