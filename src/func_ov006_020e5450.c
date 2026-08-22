// NONMATCHING: 346 candidate words against 344 target words (0x560/4 = 339
// instructions + 5 pool words). The prologue, both wall clamps, the loop
// guards, the distance test and all three epilogues are word-for-word the ROM's
// through 0x94; the residue is scheduling and stack-slot assignment across the
// twelve 64-bit fixed-point products in the middle -- the ROM spills 0x7c bytes
// and re-loads nearly every intermediate, this candidate spills 0x74 and keeps
// more of them live. Logic verified instruction-for-instruction against
// extracted/overlays/overlay_0006.bin at base 0x020bfec0. Counts as
// decompiled, not matched.
//
// func_ov006_020e5450 @ 0x020e5450, size 0x560. dScMgCurling2_c ("Shell Smash",
// actor id 0x177, scene 375). THE ROUND-END WORKHORSE: state slot 1 of table
// data_ov006_021419d8 (src/func_ov006_020e5b7c.c) calls it once per moving
// shell per frame, and slot 2 reaches it through the tail-call veneer
// src/func_ov006_020e5b70.c. The mg6 1200-frame run of scene 375 asked for it
// 6810 times with nothing behind it.
//
// WHAT IT DOES: the shell-vs-shell elastic collision. Records are 0x30 bytes at
// object+0x4660 { +0x00 x, +0x04 y, +0x08 speed, +0x26 angle (u16), +0x28
// state, +0x29 active, +0x2b loud-flag }. For the first ACTIVE, non-self record
// whose state is neither 0 nor 3 and whose distance is under 0x18 units, it
//   1. takes the contact normal a0 = atan2(dy, dx);
//   2. rotates BOTH velocities into that frame -- (p,q) for shell `idx`,
//      (r,u) for shell `i`, each a sin/cos of (own angle - a0) times own speed;
//   3. swaps the normal components and rotates back, giving (va,vb) for `idx`
//      and (vc,vd) for `i`;
//   4. writes each shell's new angle = atan2(., .) and speed = sqrt(.^2+.^2);
//   5. pushes `idx` back to 27 units (0x1b000 Fix12) from `i` along the normal;
//   6. clamps `idx` against the left, right and bottom walls, MOVING SHELL `i`
//      BY THE SAME DELTA so the pair stays separated;
//   7. marks both state 1, sets `i`'s loud flag when its new speed is >= 0x3800,
//      plays sound 0xe8 and spawns the effect via func_ov006_020e39e0.
//
// ONE ROM QUIRK IS REPRODUCED DELIBERATELY. The left-wall branch overwrites the
// same variable that holds the x tile index with the Fix12 push-back delta
// (`xi = *(int *)(c + 0x4660 + m) - 0xc000;`), so the right-wall test that
// follows sees the delta and not the index. That is what the image does --
// 0x020e5864 writes r0 and 0x020e5884 reads it back -- and the ROM's behaviour
// is the port's contract, so it is transcribed rather than corrected.
#include "types.h"

extern s16 data_02082214[];
extern int _ZN4cstd4sqrtEy(u64);
extern int _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
extern void func_02012718(int a, int b);
extern void func_ov006_020e39e0(char *c, int a, int b);

void func_ov006_020e5450(char *c, int idx)
{
    int m;
    int n;
    int i;
    int dx, dy;
    int d;
    int a0;
    int ia, ib;
    int sn0, cs0;
    int p, q, r, u;
    int va, vb, vc, vd;
    int xi, yi;
    char *e;

    m = idx * 0x30;

    e = c;
    for (i = 0; i < 0xb; i++, e += 0x30) {
        if (*(u8 *)(e + 0x4689) == 0) continue;
        if (idx == i) continue;
        if (*(u8 *)(e + 0x4688) == 0) continue;
        if (*(u8 *)(e + 0x4688) == 3) continue;

        dx = *(int *)(e + 0x4660) - *(int *)(c + m + 0x4660);
        dy = *(int *)(e + 0x4664) - *(int *)(c + m + 0x4664);
        d = _ZN4cstd4sqrtEy((u64)((s64)dy * dy + (s64)dx * dx));
        if ((d >> 12) >= 0x18) continue;

        n = i * 0x30;

        dy = *(int *)(c + 0x4664 + n) - *(int *)(c + 0x4664 + m);
        dx = *(int *)(c + 0x4660 + n) - *(int *)(c + 0x4660 + m);
        a0 = (u16)_ZN4cstd5atan2E5Fix12IiES1_(dy, dx);

        ia = ((u16)(*(u16 *)(c + 0x4686 + m) - a0) >> 4) * 2;
        p = (int)(((long long)data_02082214[ia + 1] * *(int *)(c + 0x4668 + m) + 0x800) >> 12);
        q = (int)(((long long)data_02082214[ia] * *(int *)(c + 0x4668 + m) + 0x800) >> 12);

        ib = ((u16)(*(u16 *)(c + 0x4686 + n) - a0) >> 4) * 2;
        r = (int)(((long long)data_02082214[ib + 1] * *(int *)(c + 0x4668 + n) + 0x800) >> 12);
        u = (int)(((long long)data_02082214[ib] * *(int *)(c + 0x4668 + n) + 0x800) >> 12);

        cs0 = data_02082214[(a0 >> 4) * 2];
        sn0 = data_02082214[(a0 >> 4) * 2 + 1];

        va = (int)(((long long)sn0 * r + 0x800) >> 12) - (int)(((long long)cs0 * q + 0x800) >> 12);
        vb = (int)(((long long)cs0 * r + 0x800) >> 12) + (int)(((long long)sn0 * q + 0x800) >> 12);
        vc = (int)(((long long)sn0 * p + 0x800) >> 12) - (int)(((long long)cs0 * u + 0x800) >> 12);
        vd = (int)(((long long)cs0 * p + 0x800) >> 12) + (int)(((long long)sn0 * u + 0x800) >> 12);

        *(u16 *)(c + 0x4686 + m) = (u16)_ZN4cstd5atan2E5Fix12IiES1_(vb, va);
        *(int *)(c + 0x4668 + m) = _ZN4cstd4sqrtEy((u64)((s64)vb * vb + (s64)va * va));

        *(int *)(c + 0x4660 + m) = *(int *)(c + 0x4660 + n)
            - (int)(((long long)sn0 * (long long)0x1b000 + 0x800) >> 12);
        *(int *)(c + 0x4664 + m) = *(int *)(c + 0x4664 + n)
            - (int)(((long long)cs0 * (long long)0x1b000 + 0x800) >> 12);

        yi = *(int *)(c + 0x4664 + m) >> 12;
        xi = *(int *)(c + 0x4660 + m) >> 12;
        if (xi - 0xc < 0) {
            xi = *(int *)(c + 0x4660 + m) - 0xc000;
            *(int *)(c + 0x4660 + n) = *(int *)(c + 0x4660 + n) + xi;
            *(int *)(c + 0x4660 + m) = 0xc000;
        }
        if (xi + 0xc > 0x100) {
            *(int *)(c + 0x4660 + n) = *(int *)(c + 0x4660 + n)
                + (*(int *)(c + 0x4660 + m) - 0xf4000);
            *(int *)(c + 0x4660 + m) = 0xf4000;
        }
        if (yi - 0xc < -0xe0) {
            *(int *)(c + 0x4664 + m) = -0xd4000;
            *(int *)(c + 0x4664 + n) = *(int *)(c + 0x4664 + m) + 0x18000;
        }

        *(u16 *)(c + 0x4686 + n) = (u16)_ZN4cstd5atan2E5Fix12IiES1_(vd, vc);
        *(int *)(c + 0x4668 + n) = _ZN4cstd4sqrtEy((u64)((s64)vd * vd + (s64)vc * vc));

        *(u8 *)(c + 0x4688 + m) = 1;
        *(u8 *)(c + 0x4688 + n) = 1;
        if (*(int *)(c + 0x4668 + n) >= 0x3800) {
            *(u8 *)(c + 0x468b + n) = 1;
        } else {
            *(u8 *)(c + 0x468b + n) = 0;
        }

        func_02012718(0xe8, *(int *)(c + 0x4660 + m));
        func_ov006_020e39e0(c, idx, i);
        return;
    }
}
