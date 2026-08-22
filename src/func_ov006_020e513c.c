// NONMATCHING: 181 candidate words against 197 target words (0x314/4 = 194
// instructions + 3 pool words). The skeleton is the ROM's -- same two nested
// eleven-record loops, same three epilogues, same 64x64 multiply expansion
// (umull / mla / mla / adds / adc / lsr / orr) for the x26 separation step --
// and the residue is the ROM's extra address materialisations and stack
// reloads: the ROM spills SEVEN words (sub sp,#0x1c) including the constant
// 0x800 itself, where this candidate spills three.
//
// THE OBVIOUS LEVER IS A TRAP HERE AND THE MEASUREMENT IS WORTH KEEPING.
// Walking the eleven records with a POINTER instead of an index -- the lever
// that closed most of func_ov006_020e5450's gap -- narrows the word COUNT from
// 181 to 189, but it costs on the aligned diff the repository actually scores
// with: tools/nearmiss_db.py's difflib oracle reads the pointer version at 258
// divergences and this one at 192, because the pointer form makes mwcc spill
// `idx` and keep `m` in a register, which is the opposite of the ROM's frame
// (`mov fp,r1` and `str r1,[sp,#0x10]`). Re-deriving the idx address in the
// placement step instead of reusing the pointer moves nothing at all; mwcc
// common-subexpressions the address either way. A raw word count is not the
// score. Logic verified instruction-for-instruction against
// extracted/overlays/overlay_0006.bin at base 0x020bfec0. Counts as
// decompiled, not matched.
//
// AND AN EARLIER FAN-OUT ALREADY HAD A CLOSER CANDIDATE, which is worth more
// as a CROSS-CHECK than as a diff. nearmiss/db.jsonl carries a candidate for
// this address from before this lane (source: fable-subagent); re-scored with
// the repository's own difflib oracle it reads 175 divergences against this file's 192. Its logic and
// this transcription agree statement for statement -- same guards in the same
// order, same field offsets, same returns, same call at the end -- which is two
// independent derivations of the ROM agreeing, one of them made without seeing
// the other. The DB row is deliberately left alone so a later matching lane
// starts from the closer one; this file is the one whose every instruction was
// read off the image by hand, which is what the banner above claims.
//
// func_ov006_020e513c @ 0x020e513c, size 0x314. dScMgCurling2_c ("Shell Smash",
// actor id 0x177, scene 375). Reached from state slot 0 of table
// data_ov006_021419d8 (src/func_ov006_020e5e3c.c) on the frame the cue strikes.
//
// WHAT IT DOES: depenetration. The eleven 0x30-stride shell records live at
// object+0x4660 (x, Fix12), +0x4664 (y), +0x4689 (active). It scans them for
// the first ACTIVE record other than `idx` that is within 0x18 units of shell
// `idx`, and re-seats that shell exactly 26 units from `idx` along the line
// between them; then it does the same for the first shell that overlaps the one
// it just moved, and on that second hit it plays sound 0xe8 and spawns the
// contact effect through func_ov006_020e39e0(c, idx, i). One shove per call --
// every path out of the outer loop body returns.
//
// THE ANGLE AND THE TABLE. data_02082214 is the arm9 sin/cos table and the
// index idiom is src/func_ov006_020e59b0.c's: cos = tbl[(ang>>4)*2],
// sin = tbl[(ang>>4)*2 + 1]. THE x26 IS A 64-BIT CONSTANT in the ROM, not an
// int: the image expands it as umull + two mla (the second by the sign word),
// which is what `* (long long)26` produces and what `* 26` does not.
#include "types.h"

extern s16 data_02082214[];
extern int _ZN4cstd4sqrtEy(u64);
extern int _ZN4cstd5atan2E5Fix12IiES1_(int y, int x);
extern void func_02012718(int a, int b);
extern void func_ov006_020e39e0(char *c, int a, int b);

void func_ov006_020e513c(char *c, int idx)
{
    int m;
    int *px;
    int *py;
    int i, j;
    int dx, dy;
    int d;
    u16 ang;

    m = idx * 0x30;
    px = (int *)(c + m + 0x4660);
    py = (int *)(c + m + 0x4664);

    for (i = 0; i < 0xb; i++) {
        if (*(u8 *)(c + i * 0x30 + 0x4689) == 0) continue;
        if (idx == i) continue;
        dy = (*(int *)(c + i * 0x30 + 0x4664) - *py) >> 12;
        dx = (*(int *)(c + i * 0x30 + 0x4660) - *px) >> 12;
        d = _ZN4cstd4sqrtEy((u64)(s64)(dy * dy + dx * dx));
        ang = (u16)_ZN4cstd5atan2E5Fix12IiES1_(dy, dx);
        if (d > 0x18) continue;

        *(int *)(c + i * 0x30 + 0x4660) = *px +
            ((int)(((long long)data_02082214[(ang >> 4) * 2 + 1] * (long long)26 + 0x800) >> 12) << 12);
        *(int *)(c + i * 0x30 + 0x4664) = *py +
            ((int)(((long long)data_02082214[(ang >> 4) * 2] * (long long)26 + 0x800) >> 12) << 12);

        for (j = 0; j < 0xb; j++) {
            if (*(u8 *)(c + j * 0x30 + 0x4689) == 0) continue;
            if (i == j) continue;
            dy = (*(int *)(c + j * 0x30 + 0x4664) - *(int *)(c + i * 0x30 + 0x4664)) >> 12;
            dx = (*(int *)(c + j * 0x30 + 0x4660) - *(int *)(c + i * 0x30 + 0x4660)) >> 12;
            d = _ZN4cstd4sqrtEy((u64)(s64)(dy * dy + dx * dx));
            ang = (u16)_ZN4cstd5atan2E5Fix12IiES1_(dy, dx);
            if (d > 0x18) continue;

            *(int *)(c + j * 0x30 + 0x4660) = *(int *)(c + i * 0x30 + 0x4660) +
                ((int)(((long long)data_02082214[(ang >> 4) * 2 + 1] * (long long)26 + 0x800) >> 12) << 12);
            *(int *)(c + j * 0x30 + 0x4664) = *(int *)(c + i * 0x30 + 0x4664) +
                ((int)(((long long)data_02082214[(ang >> 4) * 2] * (long long)26 + 0x800) >> 12) << 12);

            func_02012718(0xe8, *px);
            func_ov006_020e39e0(c, idx, i);
            return;
        }
        return;
    }
}
