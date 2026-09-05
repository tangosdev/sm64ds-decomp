#include "types.h"
// @symbol func_ov002_020f43cc
/* recovered: per-slot update of a 4-entry cursor/target table (0x4c-byte
 * records at c). Slot i first burns its hold timer (+0x30); while its arrival
 * flag (+0x48) is clear it steps position (+0/+4) along heading (+0x2e) at
 * speed (+8), decelerates at the -0x2000 floor, and either snaps onto the
 * target (+0x1c/+0x20) within 2 units (flag++, timer reset, heading 0xc000)
 * or turns the heading toward atan2(dy,dx) in 0x100 steps. Arrived slots
 * orbit at radius +0x10 around (0x80000,0x60000) with angular speed +0x42.
 * When the last slot (func_ov002_020f5a94(c)-1) has orbited past 0x10000,
 * every active row (+0x44) is re-armed: +0x47 bumped, +0x3c/+0x48/+0x24
 * cleared, radius 0x38000, heading from data_ov002_021000b8[j] (0 for j==2
 * when the mode call returns 3).
 *
 * Codegen notes: the function is void (the ROM overwrites r0 in place after
 * both calls); func_ov002_020f5a94 takes c (r0 stays live to the bl). The
 * dx/dy block needs TWO named bases: px = c + off defined before the clamp
 * (colours to sb) and s = c + idx * 0x4c at the block start (shares the
 * laundered product's inner mul but not its value number, so it is the r2
 * base the tx/ty loads and the +0x24 store share).
 */
extern int _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
extern int func_ov002_020f5a94(void *a);
extern short data_02082214[];
extern unsigned short data_ov002_021000b8[];

void func_ov002_020f43cc(char *c, int i) {
    int idx = i;
    char *px;
    char *s;
    int off = (int)(((long long)(idx * 0x4c)) & 0xFFFFFFFFFFFFFFFFLL);
    volatile int t1; (void)&t1;
    {
        unsigned short v = *(unsigned short *)(c + 0x30 + off);
        if (v != 0) {
            *(unsigned short *)(c + 0x30 + off) = v - 1;
            return;
        }
    }
    if (*(unsigned char *)(c + 0x48 + off) == 0) {
        int idxa;
        short tv;
        int spd;
        idxa = *(unsigned short *)(c + 0x2e + off) >> 4;
        {
            tv = data_02082214[idxa * 2 + 1];
            spd = *(int *)(c + 8 + off);
            *(int *)(c + off) += (int)(((long long)tv * spd + 0x800) >> 12);
        }
        idxa = *(unsigned short *)(c + 0x2e + off) >> 4;
        {
            tv = data_02082214[idxa * 2];
            spd = *(int *)(c + 8 + off);
            *(int *)(c + 4 + off) += (int)(((long long)tv * spd + 0x800) >> 12);
        }
        px = c + off;
        if (*(int *)(c + 8 + off) <= -0x2000)
            *(int *)(c + 8 + off) = *(int *)(c + 8 + off) + 0x120;
        {
            s = c + idx * 0x4c;
            int dx = (*(int *)(s + 0x1c) - *(int *)px) >> 12;
            int dy = (*(int *)(s + 0x20) - *(int *)(c + 4 + off)) >> 12;
            if (dx >= -2 && dx <= 2 && dy >= -2 && dy <= 2) {
                (*(unsigned char *)(c + 0x48 + off))++;
                *(int *)(s + 0x24) = 0;
                *(unsigned short *)(c + 0x2e + off) = 0xc000;
                return;
            }
            {
                unsigned short a = (unsigned short)_ZN4cstd5atan2E5Fix12IiES1_(dy, dx);
                unsigned short cur = *(unsigned short *)(c + 0x2e + off);
                if (a > cur) {
                    unsigned short nv;
                    *(unsigned short *)(c + 0x2e + off) += 0x100;
                    nv = *(unsigned short *)(c + 0x2e + off);
                    if (a <= nv)
                        *(unsigned short *)(c + 0x2e + off) = a;
                    return;
                }
                if (cur > a) {
                    unsigned short nv2;
                    *(unsigned short *)(c + 0x2e + off) -= 0x100;
                    nv2 = *(unsigned short *)(c + 0x2e + off);
                    if (nv2 <= a)
                        *(unsigned short *)(c + 0x2e + off) = a;
                    return;
                }
                return;
            }
        }
    }
    {
        int idxa;
        short tv;
        int spd;
        idxa = *(unsigned short *)(c + 0x2e + off) >> 4;
        {
            tv = data_02082214[idxa * 2 + 1];
            spd = *(int *)(c + 0x10 + off);
            *(int *)(c + off) = (int)(((long long)tv * spd + 0x800) >> 12) + 0x80000;
        }
        idxa = *(unsigned short *)(c + 0x2e + off) >> 4;
        {
            tv = data_02082214[idxa * 2];
            spd = *(int *)(c + 0x10 + off);
            *(int *)(c + 4 + off) = (int)(((long long)tv * spd + 0x800) >> 12) + 0x60000;
        }
        *(unsigned short *)(c + 0x2e + off) -= *(unsigned short *)(c + 0x42 + off);
        *(int *)(c + 0x24 + off) += *(unsigned short *)(c + 0x42 + off);
    }
    if (idx != func_ov002_020f5a94(c) - 1)
        return;
    if ((unsigned int)*(int *)(c + 0x24) < 0x10000U)
        return;
    {
        int j;
        char *p = c;
        for (j = 0; j < 4; j++) {
            if (*(unsigned char *)(p + 0x44) != 0) {
                (*(unsigned char *)(p + 0x47))++;
                *(unsigned short *)(p + 0x3c) = 0;
                *(unsigned char *)(p + 0x48) = 0;
                *(int *)(p + 0x10) = 0x38000;
                *(unsigned short *)(p + 0x2e) = data_ov002_021000b8[j];
                if (func_ov002_020f5a94(c) == 3) {
                    if (j == 2) *(unsigned short *)(p + 0x2e) = 0;
                }
                *(int *)(p + 0x24) = 0;
            }
            p += 0x4c;
        }
    }
}
