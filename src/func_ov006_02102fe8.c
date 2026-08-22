// NONMATCHING: register allocation only. Logic verified instruction-for-
// instruction against extracted/overlays/overlay_0006.bin at base 0x020bfec0.
// Counts as decompiled, not matched.
//
// dScMgPachinko2_c (actor id 0x171, scene 369), state 1 of
// data_ov006_021426f4 -- the 0x30-entry loop at +0x4698. It is the touch
// driver: while the stylus is down it moves the cursor by the touch delta,
// clamps it to the 8..0xf8 by 8..0xb8 box, re-seats the grab offset and plays
// one of two drag sounds by how far the cursor moved; on release it turns the
// cursor's distance from (0x80, 0x20) into a launch angle and speed, writes
// the velocity pair through the sine table at data_02082214, hands the state
// index to 2 and plays the launch sound. Under 0x10 of travel it re-centres
// and goes back to state 0 instead.
//
// COMPILER: mwccarm 2004/b56, not the 1.2/sp2p3 the three sibling states in
// this class match at. Measured, not assumed: a full 25-version sweep with
// tools/match.py --all puts every 1.2 and 2.0 build at the wrong SIZE and
// 2004/b56 at exactly 0x378, and the -O sweep puts -O4 ahead of -O3 (50) and
// -O2 (59).
//
// 222 words = 212 instructions (0x02102fe8..0x02103338) + 10 pool words
// (0x02103338..0x0210335c) = 0x378/4. The candidate is the same 0x378 and the
// same 212 instructions; 18 words differ and NOT ONE of them is
// a different operation, a missing instruction or an extra one:
//
//   0x02103018..0x02103030   the same twelve instructions in a different
//                            order. The ROM hoists the data_020a0deb pool
//                            load above the first store and fills the
//                            load-use slot with the second touch byte; the
//                            candidate fills it with the pool load instead.
//   0x0210317c..0x02103190   r1 and r2 exchanged on two loads and two shifts.
//   0x02103274..0x021032bc   r3/r4 and r4/r6 exchanged around the two
//                            sine-table multiplies.
//
// The full diff, ROM on the left and candidate on the right:
//
//   02103018 | ldr      r3, [pc, #0x324]                | add      r0, r0, #0x4000
//   0210301C | add      r0, r0, #0x4000                 | ldrb     r6, [r1, r7]
//   02103020 | ldrb     r6, [r1, r7]                    | ldr      r3, [r0, #0x670]
//   02103024 | ldr      r1, [r0, #0x670]                | ldr      r1, [pc, #0x318]
//   02103028 | ldrb     r3, [r3, r7]                    | add      r3, r3, r6, lsl #12
//   0210302C | add      r1, r1, r6, lsl #12             | str      r3, [r0, #0x660]
//   02103030 | str      r1, [r0, #0x660]                | ldrb     r3, [r1, r7]
//   0210317C | ldr      r1, [r0, #0x664]                | ldr      r2, [r0, #0x664]
//   02103180 | ldr      r2, [r0, #0x660]                | ldr      r1, [r0, #0x660]
//   02103184 | asr      r0, r1, #0xc                    | asr      r0, r2, #0xc
//   02103190 | asr      r1, r2, #0xc                    | asr      r1, r1, #0xc
//   02103274 | ldr      r3, [r1, #0x680]                | ldr      r4, [r1, #0x680]
//   02103288 | ldrsh    r4, [r7, r0]                    | ldrsh    r3, [r7, r0]
//   021032A8 | ldrh     r6, [r2, #0x94]                 | ldrh     r4, [r2, #0x94]
//   021032AC | ldr      r4, [r1, #0x680]                | ldr      r6, [r1, #0x680]
//   021032B4 | asr      r6, r6, #4                      | asr      r4, r4, #4
//   021032B8 | lsl      r6, r6, #2                      | lsl      r4, r4, #2
//   021032BC | ldrsh    r6, [r7, r6]                    | ldrsh    r4, [r7, r4]
//
// Levers already spent on it, all recorded in nearmiss/db.jsonl: the whole
// opt_* pragma sweep (only opt_common_subs off helps, and it is kept),
// hoisting each statement's value into a local (that is what closed the
// +0x670/+0x674 pair and the sqrt argument), declaration-order permutation of
// every local, const-qualifying the three touch arrays, and both spellings of
// the touch index. None moves the remaining 18.

#include "types.h"
#pragma opt_common_subs off
extern u8 data_020a0e40;
extern u8 data_020a0de8[];
extern u8 data_020a0dea[];
extern u8 data_020a0deb[];
extern short data_02082214[];
extern Fix12i _ZN4cstd4sqrtEy(u64 val);
extern s16 _ZN4cstd5atan2E5Fix12IiES1_(Fix12i y, Fix12i x);
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int n);
extern int Sound_PlayIfNotActive(int a, int b, int c, int d);
extern void func_ov006_02102864(char* p);

void func_ov006_02102fe8(char* p, int i)
{
    u32 idx = data_020a0e40;
    int x, y, tx, ty, old;
    u64 sq;
    int d, dx, dy;

    if (data_020a0de8[idx * 4] != 0) {
        tx = data_020a0dea[idx * 4];
        ty = data_020a0deb[idx * 4];
        *(int*)(p + 0x4000 + (i << 6) + 0x660) =
            *(int*)(p + 0x4000 + (i << 6) + 0x670) + (tx << 12);
        *(int*)(p + 0x4000 + (i << 6) + 0x664) =
            *(int*)(p + 0x4000 + (i << 6) + 0x674) + (ty << 12);

        x = *(int*)(p + 0x4000 + (i << 6) + 0x660) >> 12;
        y = *(int*)(p + 0x4000 + (i << 6) + 0x664) >> 12;
        if (x >= 0xf8) *(int*)(p + 0x4000 + (i << 6) + 0x660) = 0xf8000;
        if (x <= 8)    *(int*)(p + 0x4000 + (i << 6) + 0x660) = 0x8000;
        if (y >= 0xb8) *(int*)(p + 0x4000 + (i << 6) + 0x664) = 0xb8000;
        if (y <= 8)    *(int*)(p + 0x4000 + (i << 6) + 0x664) = 0x8000;

        tx = data_020a0dea[idx * 4];
        ty = data_020a0deb[idx * 4];
        x = (*(int*)(p + 0x4000 + (i << 6) + 0x660) >> 12) - tx;
        y = (*(int*)(p + 0x4000 + (i << 6) + 0x664) >> 12) - ty;
        *(int*)(p + 0x4000 + (i << 6) + 0x670) = x << 12;
        *(int*)(p + 0x4000 + (i << 6) + 0x674) = y << 12;

        x = *(int*)(p + 0x4000 + (i << 6) + 0x660) >> 12;
        y = *(int*)(p + 0x4000 + (i << 6) + 0x664) >> 12;
        dx = 0x80 - x;
        dy = 0x20 - y;
        d = _ZN4cstd4sqrtEy((u64)(dx * dx + dy * dy));

        old = *(int*)(p + 0x4000 + (i << 6) + 0x68c);
        *(int*)(p + 0x4000 + (i << 6) + 0x68c) = d;
        if (d > old) {
            if (d > old + 10)
                *(int*)(p + 0x4000 + (i << 6) + 0x688) =
                    Sound_PlayIfNotActive(*(int*)(p + 0x4000 + (i << 6) + 0x688), 2, 0x17b, 0);
            else
                *(int*)(p + 0x4000 + (i << 6) + 0x688) =
                    Sound_PlayIfNotActive(*(int*)(p + 0x4000 + (i << 6) + 0x688), 2, 0x17c, 0);
        }
    } else {
        *(u8*)(p + 0x4000 + (i << 6) + 0x699) = 2;

        dy = 0x20 - (*(int*)(p + 0x4000 + (i << 6) + 0x664) >> 12);
        dx = 0x80 - (*(int*)(p + 0x4000 + (i << 6) + 0x660) >> 12);
        sq = (u64)(dx * dx + dy * dy);
        d = _ZN4cstd4sqrtEy(sq);

        if (d >= 0x10) {
            *(u16*)(p + 0x4600 + (i << 6) + 0x94) = _ZN4cstd5atan2E5Fix12IiES1_(dy, dx);
            *(int*)(p + 0x4000 + (i << 6) + 0x680) = _ZN4cstd4sqrtEy(sq) << 8;
            *(int*)(p + 0x4680 + (i << 6)) += *(int*)(p + 0x4000 + (i << 6) + 0x680) >> 3;
            *(int*)(p + 0x4680 + (i << 6)) += 0x1800;
            if (*(int*)(p + 0x4000 + (i << 6) + 0x680) >= 0x9400)
                *(int*)(p + 0x4000 + (i << 6) + 0x680) = 0x9400;

            *(u16*)(p + 0x5000 + 0x66c) = *(int*)(p + 0x4000 + (i << 6) + 0x680) >> 11;
            *(u16*)(p + 0x566c) += *(u16*)(p + 0x5000 + 0x66c) >> 1;
            if (*(u16*)(p + 0x5000 + 0x66c) == 0) *(u16*)(p + 0x5000 + 0x66c) = 1;

            *(int*)(p + 0x4000 + (i << 6) + 0x668) = (int)
                (((s64)data_02082214[((*(u16*)(p + 0x4600 + (i << 6) + 0x94) >> 4) << 1) + 1] *
                  *(int*)(p + 0x4000 + (i << 6) + 0x680) + 0x800) >> 12);
            *(int*)(p + 0x4000 + (i << 6) + 0x66c) = (int)
                (((s64)data_02082214[(*(u16*)(p + 0x4600 + (i << 6) + 0x94) >> 4) * 2] *
                  *(int*)(p + 0x4000 + (i << 6) + 0x680) + 0x800) >> 12);

            *(u16*)(p + 0x4600 + (i << 6) + 0x96) = 0;
            *(u16*)(p + 0x4600 + (i << 6) + 0x92) =
                *(int*)(p + 0x4000 + (i << 6) + 0x668) >> 2;

            if (d >= 0x40) _ZN5Sound12PlayBank2_2DEj(0x17e);
            else           _ZN5Sound12PlayBank2_2DEj(0x17d);
        } else {
            *(int*)(p + 0x4000 + (i << 6) + 0x660) = 0x80000;
            *(int*)(p + 0x4000 + (i << 6) + 0x664) = 0x28000;
            *(u8*)(p + 0x4000 + (i << 6) + 0x699) = 0;
        }
    }

    func_ov006_02102864(p);
}
