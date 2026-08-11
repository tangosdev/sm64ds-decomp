/* Banner claims tools/deepen_rtti.py generated this file. That tool has never
 * existed in this repo (git log --all --diff-filter=A -- '*deepen_rtti*' is
 * empty; see notes/runbook-type-reconstruction.md section 2) -- so treat every
 * field below except the one this comment documents as an unverified
 * placeholder, not as machine-checked evidence.
 *
 * class dScMgBase_c, recovered from ROM RTTI + vtable slot identity.
 * Offsets/widths are observed. Names are placeholders.
 *
 * 0x0f4 IS hand-verified, from _ZN11dScMgBase_cD2Ev (src/_ZN11dScMgBase_cD2Ev.cpp):
 * it calls func_ov004_020b929c(c + 0xf4), which is
 *     __destroy_arr(p, count=8, elem_size=0x24, func_ov004_020b9280)
 * (0x0207328c is __destroy_arr / __cxa_vec_cleanup, config/arm9/symbols.txt:3050-3051).
 * The per-element destructor func_ov004_020b9280 writes two vtables into [r0+0]
 * back to back with no further calls -- 0x020bca7c then 0x020ad494 -- which
 * build/rtti.json identifies as _ZTVN10dMgPsOpt_c11TouchIcon_cE (ov004:0x020bca68,
 * "dMgPsOpt_c::TouchIcon_c") and _ZTV9dThIcon_c (ov001:0x020ad478, "dThIcon_c",
 * a root class) respectively -- i.e. TouchIcon_c : dThIcon_c, single inheritance,
 * offset 0, and both destructors are trivial enough to fully inline into vtable
 * writes. So 0x0f4 is 8 contiguous dMgPsOpt_c::TouchIcon_c, each 0x24 bytes,
 * spanning 0x0f4..0x214. Neither TouchIcon_c nor dThIcon_c has a header yet, so
 * the array is left as raw bytes rather than an unverified struct type.
 * 0x214..0x21c (8 bytes) has no matched access yet and stays padding. */
#ifndef DSCMGBASE_C_H
#define DSCMGBASE_C_H
#include "types.h"

struct dScMgBase_c {
    u8  pad_000[0x5c];
    s32 unk_05c;            /* 0x05c */
    u8  pad_060[0x44];
    s32 unk_0a4;            /* 0x0a4 */
    s32 unk_0a8;            /* 0x0a8 */
    u8  pad_0ac[0x8];
    s32 unk_0b4;            /* 0x0b4 */
    s32 unk_0b8;            /* 0x0b8 */
    u8  pad_0bc[0x7];
    u8  unk_0c3;            /* 0x0c3 */
    u8  pad_0c4[0x4];
    s32 unk_0c8;            /* 0x0c8 */
    u8  pad_0cc[0x24];
    s32 unk_0f0;            /* 0x0f0 */
    u8  touchIcon_0f4[8][0x24]; /* 0x0f4 -- 8x dMgPsOpt_c::TouchIcon_c : dThIcon_c,
                                    destroyed via __destroy_arr in dScMgBase_c's D2;
                                    see the file banner. */
    u8  pad_214[0x8];       /* 0x214 -- unaccounted for, no matched access yet */
    u32 unk_21c;            /* 0x21c */
    u32 unk_220;            /* 0x220 */
    u32 unk_224;            /* 0x224 */
    u8  pad_228[0x4400];
    s32 unk_4628;           /* 0x4628 */
    s32 unk_462c;           /* 0x462c */
    s32 unk_4630;           /* 0x4630 */
    s16 unk_4634;           /* 0x4634 */
    s16 unk_4636;           /* 0x4636 */
    s16 unk_4638;           /* 0x4638 */
    s16 unk_463a;           /* 0x463a */
    s16 unk_463c;           /* 0x463c */
    s16 unk_463e;           /* 0x463e */
    s32 unk_4640;           /* 0x4640 */
    u8  pad_4644[0x2];
    s16 unk_4646;           /* 0x4646 */
    u8  pad_4648[0x14];
    u8  unk_465c;           /* 0x465c */
};

#endif
