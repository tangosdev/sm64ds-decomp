// NONMATCHING: register colouring only. SIZE IS EXACT (192/192 words = 0x300/4:
// 186 instructions + 6 pool words) and the schedule is the ROM's; 72 words differ
// and every one of them is the same permutation -- the ROM colours the receiver
// sb and this candidate colours it r4, which walks the other five callee-saved
// registers one seat over. mwccarm 1.2/sp2p3; tools/match.py --all finds no
// version that closes it, and four declaration-order permutations move the count
// by at most one. Logic verified instruction-for-instruction against
// extracted/overlays/overlay_0006.bin at base 0x020bfec0. Counts as decompiled,
// not matched. See notes/mwccarm-codegen.md 6aa (the coloring wall).
//
// func_ov006_020e4bd4 @ 0x020e4bd4, size 0x300. dScMgCurling2_c ("Shell Smash",
// actor id 0x177, scene 375), slot 1 of state table data_ov006_02141978 -- the
// LIVE state port/slice_s75.txt and port/unmatched/MgCurling2_StateDispatch.cpp
// reported as a floor. THE STYLUS AIM STATE: it reads the touch sample the arm9
// keeps at data_020a0de8[i*4] (valid) / data_020a0dea[i*4] (x) /
// data_020a0deb[i*4] (y) with i = data_020a0e40[0], moves the cue to it, clamps
// it into the play field, and derives the flick's angle and power.
#include "types.h"

#define M(p) ((int *)(int)((long long)(int)(p)))

extern u8 data_020a0de8[];
extern u8 data_020a0dea[];
extern u8 data_020a0deb[];
extern u8 data_020a0e40[];

extern int _ZN4cstd4sqrtEy(u64);
extern int _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
extern void func_02012718(int a, int b);

void func_ov006_020e4bd4(char *c)
{
    int i;
    int i4;
    int ox, oy;
    int dx, dy;
    int d;
    int t;
    u16 oa;
    int v;

    i = data_020a0e40[0];
    i4 = i * 4;
    if (data_020a0de8[i * 4] != 0) {
    ox = *(int *)(c + 0x5584);
    oy = *(int *)(c + 0x5588);
    *(int *)(c + 0x5584) = *(int *)(c + 0x5594) + (data_020a0dea[i4] << 12);
    *(int *)(c + 0x5588) = *(int *)(c + 0x5598) + (data_020a0deb[i4] << 12);

    if (*(int *)(c + 0x5588) <= 0x94000) *(int *)(c + 0x5588) = 0x94000;
    if (*(int *)(c + 0x5584) <= 0x20000) *(int *)(c + 0x5584) = 0x20000;
    if (*(int *)(c + 0x5584) >= 0xe0000) *(int *)(c + 0x5584) = 0xe0000;
    if (*(int *)(c + 0x5588) >= 0xb8000) *(int *)(c + 0x5588) = 0xb8000;

    dy = (*(int *)(c + 0x5588) - oy) >> 12;
    dx = (*(int *)(c + 0x5584) - ox) >> 12;
    d = _ZN4cstd4sqrtEy((u64)(s64)(dy * dy + dx * dx));
    if (d <= 1) {
        *(int *)(c + 0x5584) = ox;
        *(int *)(c + 0x5588) = oy;
        return;
    }

    t = (*(int *)(c + 0x5588) - *(int *)(c + 0x5590)) >> 12;
    if (*(u8 *)(c + 0x55be) == 0) {
        func_02012718(0x1d6, *(int *)(c + 0x5584));
        *(u8 *)(c + 0x55be) = 2;
        *(int *)(c + 0x55a8) = (*(int *)(c + 0x5588) - *(int *)(c + 0x5590)) >> 12;
        *(int *)(c + 0x5590) = *(int *)(c + 0x5588);
    } else if (*(u8 *)(c + 0x55be) == 1) {
        if (*(int *)(c + 0x55a8) * t > 0) {
            if (t < 0) t = -t;
            if (t >= 10) *(u8 *)(c + 0x55be) = 0;
        } else {
            *(int *)(c + 0x55a8) = t;
            *(int *)(c + 0x5590) = *(int *)(c + 0x5588);
        }
    } else {
        if (*(int *)(c + 0x55a8) * t < 0) *(u8 *)(c + 0x55be) = 1;
        *(int *)(c + 0x55a8) = (*(int *)(c + 0x5588) - *(int *)(c + 0x5590)) >> 12;
        *(int *)(c + 0x5590) = *(int *)(c + 0x5588);
    }

    oa = *(u16 *)(c + 0x55b2);
    *(u16 *)(c + 0x55b2) = _ZN4cstd5atan2E5Fix12IiES1_(dy, dx >> 1);
    if (*(u16 *)(c + 0x55b2) <= 0x8000 && *(u16 *)(c + 0x55b2) >= 0x4000)
        *(u16 *)(c + 0x55b2) = 0x8000;
    else if (*(u16 *)(c + 0x55b2) <= 0x4000)
        *(u16 *)(c + 0x55b2) = 0;
    *(u16 *)(c + 0x55b2) = (*(u16 *)(c + 0x55b2) + oa) >> 1;

    d = _ZN4cstd4sqrtEy((u64)(s64)((dx >> 1) * (dx >> 1) + dy * dy));
    v = ((d * 9) << 12) >> 4;
    if (v >= 0xc000) v = 0xc000;
    if (v > *(int *)(c + 0x559c)) *(int *)(c + 0x559c) = v;
    if (*(int *)(c + 0x559c) > v) {
        int *pv = M(c + 0x559c);
        *pv = *pv - ((*(int *)(c + 0x559c) - v) >> 1);
    }

    i = data_020a0e40[0];
    *(int *)(c + 0x5594) = ((*(int *)(c + 0x5584) >> 12) - data_020a0dea[i * 4]) << 12;
    *(int *)(c + 0x5598) = ((*(int *)(c + 0x5588) >> 12) - data_020a0deb[i * 4]) << 12;
    return;
    }
    *(u8 *)(c + 0x55b8) = 0;
    *(u8 *)(c + 0x55b9) = 1;
}
