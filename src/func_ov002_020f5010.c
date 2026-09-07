#include "types.h"
// @symbol func_ov002_020f5010
/* recovered: the third of the three per-slot cursor/target updaters in this
 * family (func_ov002_020f43cc, func_ov002_020f4a2c, this one), same 0x4c-byte
 * record layout: hold timer +0x30, arrival flag +0x48, position +0/+4,
 * heading +0x2e, approach speed +8, target +0x1c/+0x20, orbit radius +0x10,
 * angular speed +0x42, orbit angle +0x24.
 *
 * Differences from func_ov002_020f4a2c, each visible in the bytes: the speed
 * clamp works from the +0x2000 side (`cmp #0x2000 / subge #0x120`), the
 * arrived slots orbit with the heading ADDED to the angular speed, the
 * re-arm loop reads data_ov002_021000d0 and has no mode==3 special case.
 * With fewer live values the compiler needs only r4-r8/sb and keeps c in r6.
 */
extern short _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
extern int func_ov002_020f5a94(char *c);
extern short data_02082214[];
extern unsigned short data_ov002_021000d0[];

void func_ov002_020f5010(char *c, int i)
{
    int off = i * 0x4c;
    int idx, dx, dy;
    unsigned short ang;
    unsigned short target;
    unsigned short v;

    v = *(unsigned short *)(c + 0x30 + off);
    if (v != 0) {
        *(unsigned short *)(c + 0x30 + off) = v - 1;
        return;
    }

    if (*(unsigned char *)(c + 0x48 + off) == 0) {
        idx = *(unsigned short *)(c + 0x2e + off) >> 4;
        {
            short tv = data_02082214[idx * 2 + 1];
            int spd = *(int *)(c + 8 + off);
            *(int *)(c + 0 + off) += (int)(((long long)tv * spd + 0x800) >> 12);
        }

        idx = *(unsigned short *)(c + 0x2e + off) >> 4;
        {
            short tv = data_02082214[idx * 2];
            int spd = *(int *)(c + 8 + off);
            *(int *)(c + 4 + off) += (int)(((long long)tv * spd + 0x800) >> 12);
        }

        if (*(int *)(c + 8 + off) >= 0x2000)
            *(int *)(c + 8 + off) = *(int *)(c + 8 + off) - 0x120;

        dx = (*(int *)(c + 0x1c + off) - *(int *)(c + 0 + off)) >> 12;
        dy = (*(int *)(c + 0x20 + off) - *(int *)(c + 4 + off)) >> 12;

        if (dx >= -2 && dx <= 2 && dy >= -2 && dy <= 2) {
            *(unsigned char *)(c + 0x48 + off) += 1;
            *(int *)(c + 0x24 + off) = 0;
            *(unsigned short *)(c + 0x2e + off) = 0xc000;
            return;
        }

        {
            int raw = _ZN4cstd5atan2E5Fix12IiES1_(dy, dx);
            ang = *(unsigned short *)(c + 0x2e + off);
            target = (unsigned short)raw;
        }
        if (target > ang) {
            unsigned short nv;
            *(unsigned short *)(c + 0x2e + off) += 0x100;
            nv = *(unsigned short *)(c + 0x2e + off);
            if (target <= nv)
                *(unsigned short *)(c + 0x2e + off) = target;
            return;
        }
        if (ang > target) {
            unsigned short nv2;
            *(unsigned short *)(c + 0x2e + off) -= 0x100;
            nv2 = *(unsigned short *)(c + 0x2e + off);
            if (nv2 <= target)
                *(unsigned short *)(c + 0x2e + off) = target;
            return;
        }
        return;
    }

    idx = *(unsigned short *)(c + 0x2e + off) >> 4;
    {
        short tv = data_02082214[idx * 2 + 1];
        int spd = *(int *)(c + 0x10 + off);
        *(int *)(c + 0 + off) = (int)(((long long)tv * spd + 0x800) >> 12) + 0x80000;
    }

    idx = *(unsigned short *)(c + 0x2e + off) >> 4;
    {
        short tv = data_02082214[idx * 2];
        int spd = *(int *)(c + 0x10 + off);
        *(int *)(c + 4 + off) = (int)(((long long)tv * spd + 0x800) >> 12) + 0x60000;
    }

    *(unsigned short *)(c + 0x2e + off) += *(unsigned short *)(c + 0x42 + off);
    *(int *)(c + 0x24 + off) += *(unsigned short *)(c + 0x42 + off);

    if (i != func_ov002_020f5a94(c) - 1)
        return;
    if ((unsigned int)*(int *)(c + 0x24) < 0x10000)
        return;

    {
        int j;
        char *p = c;
        for (j = 0; j < 4; j++) {
            if (*(unsigned char *)(p + 0x44) != 0) {
                *(unsigned char *)(p + 0x47) += 1;
                *(unsigned short *)(p + 0x3c) = 0;
                *(unsigned char *)(p + 0x48) = 0;
                *(int *)(p + 0x10) = 0x38000;
                *(unsigned short *)(p + 0x2e) = data_ov002_021000d0[j];
                *(int *)(p + 0x24) = 0;
            }
            p += 0x4c;
        }
    }
}
