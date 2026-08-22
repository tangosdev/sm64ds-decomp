// NONMATCHING: register allocation + instruction scheduling. Logic verified
// correct vs ROM instruction for instruction; not byte-matchable from C at
// mwccarm 1.2/sp2p3 (see notes/matching-style.md). Counts as decompiled, not
// matched.
//
// CLOSEST APPROACH: 188 of the ROM's 319 words byte-identical on the best
// word alignment, and 195 counting the seven relocation slots that hold a
// placeholder in the object and would carry the ROM's word once linked
// (0x020d86a0, _87d4, _8814, _8844, _8884 and the two pool loads at _88ec and
// _8900; the reloc destinations themselves are checked, so those seven are a
// real ceiling and not a wildcard being waved through). Candidate 3
// instructions long, 322 versus 319.
//
// COUNT IT ON THE RAW WORDS. An earlier revision of this banner said 176,
// which was measured with the alignment keyed on REGISTER-NORMALISED
// mnemonics -- the right key for reading the shape of the residue, the wrong
// one for this number, because it throws away every byte-identical word that
// falls outside a shape-aligned block. Align on the words themselves.
//
// The whole control-flow skeleton -- the timer arm, the thirteen-step ladder,
// all three toggle blocks, the loop preheader's ten hoisted constants and the
// entity initialisation -- reproduces exactly; what will not close is which
// register holds `lvl` and `step` (the ROM swaps sb/r8 against every
// candidate), three rematerialisations of `c + i * 0x40`, and one shared zero
// where the ROM spends two `mov`s on `mode = 0; step = 0;`.
//
// WHAT WAS TRIED, so the next lane does not repeat it: all 25 installed
// mwccarm builds. DO NOT GO BACK HOPEFULLY TO 1.2/sp3 AND 1.2/sp4 -- they are
// the two that look promising and are not. They land at 320 words, delta +1,
// the closest SIZE any build reaches, but they score 184 identical against
// 1.2/sp2p3's 188, so they are further away where it counts and the "none is
// closer" conclusion holds. 1.2/base, 1.2/sp2, 1.2/sp2p3 and 2004/b56 all sit
// together at 322 words and 188 identical; 2.0/base collapses to 271 words and
// 59. Also tried: the opt_common_subs / opt_loop_invariants /
// opt_moveinvariantsinaddressexpr / opt_strength_reduction pragmas in every
// combination; 61 declaration orders of the eight locals (the pret regalloc
// lever -- flat); pointer-local versus inline spelling of the two entity
// read-modify-writes; `i * 0x40` versus `(i << 6)`; `* 2 >> 15` versus
// `<< 1 >> 15` on the random range; accum initialised before the loop, first
// in the for-init and second in it; and -O1/-O2/-O3/-O4 in p and s variants.
//
// THE ONE RESULT WORTH REVISITING IS -O2,p: 318 words, delta -1, and 192
// identical -- better than -O4,p on BOTH counts, and the best any flag set
// reaches here. It still does not close, and -O4,p is what tools/match.py
// compiles with, so this file is written for -O4,p; but a lane that is allowed
// to move the flag set has 4 more words there than this banner's headline.
// (-O4,s and -O2 both collapse to 308 words and 117 identical.)
//
// WHAT DID MOVE IT, and these are the load-bearing parts of the source below:
// the u64 no-op mask launder L() on the five read-modify-write sites, which is
// what reproduces the ROM's split between pool-materialised RMW addresses and
// folded single-use accesses, and the two pragmas. Without L() the head alone
// was 5 instructions short and folded three separate loads into one.

typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;

extern int RandomIntInternal(int *seed);
extern int data_0209d4b8;
extern unsigned short data_ov006_0212e2e8[];
extern void func_ov006_020d66c4(char *base, int idx);

#define RND (((u32)RandomIntInternal(&data_0209d4b8) >> 16) & 0x7fff)

/* Every read-modify-write in this body materializes its address through a pool
   constant in the ROM while every single-use access folds onto the 12-bit
   offset; the no-op 64-bit mask is what holds that split (notes/matching-style.md,
   "RMW base materialization"). */
#define L(a) (((int)(a)) & 0xFFFFFFFFFFFFFFFFULL)

#pragma opt_common_subs off
#pragma opt_loop_invariants off

void func_ov006_020d8408(char *c)
{
    int lvl;
    int count;
    int mode;
    int step;
    int accum;
    int k;
    int i;
    int n;

    if (*(u16 *)(c + 0x6200 + 0xe2) != 0) {
        u16 *t = (u16 *)L(c + 0x62e2);
        *t = (u16)(*t - 1);
        if (*(s16 *)(c + 0x6200 + 0xe2) < 0)
            *(s16 *)(c + 0x6200 + 0xe2) = 0;
        return;
    }

    n = *(int *)(c + 0x6000 + 0x2d8);
    lvl = 0;
    if (n >= 0x12c) lvl = 12;
    else if (n >= 0xc6) lvl = 11;
    else if (n >= 0x9f) lvl = 10;
    else if (n >= 0x84) lvl = 9;
    else if (n >= 0x5c) lvl = 8;
    else if (n >= 0x39) lvl = 7;
    else if (n >= 0x21) lvl = 6;
    else if (n >= 0x1b) lvl = 5;
    else if (n >= 0x15) lvl = 4;
    else if (n >= 0xf) lvl = 3;
    else if (n >= 9) lvl = 2;
    else if (n >= 3) lvl = 1;

    count = 1;
    if (lvl >= 5) count = 2;
    if (lvl == 9) count = 1;
    mode = 0;
    step = 0;
    if (lvl == 7) {
        u8 *tg = (u8 *)L(c + 0x62fc);
        count = 2;
        mode = (*(u8 *)(c + 0x6000 + 0x2fc) & 1) + 1;
        *tg = *tg ^ 1;
        step = 0x3000;
    }
    if (lvl == 8) {
        u8 *tg = (u8 *)L(c + 0x62fc);
        int q = *(u8 *)(c + 0x6000 + 0x2fc) & 1;
        count = q + 2;
        *tg = *tg ^ 1;
        mode = q + 1;
        step = count == 2 ? 0x3000 : 0x1800;
    }
    if (lvl == 10) {
        count = 4;
        step = 0x3000;
    }
    if (lvl == 11) {
        u8 *tg = (u8 *)L(c + 0x62fc);
        count = 3;
        mode = (*(u8 *)(c + 0x6000 + 0x2fc) & 1) + 1;
        *tg = *tg ^ 1;
        step = 0x1800;
    }
    if (lvl >= 12) {
        count = 6;
        step = 0x1800;
    }

    for (k = 0, accum = 0; k < count; k++) {
        for (i = 0; i < 0x70; i++) {
            u8 *live = (u8 *)L(c + i * 0x40 + 0x4698);
            if (*live != 0) continue;
            *live = 1;
            *(u8 *)(c + i * 0x40 + 0x4697) = 0;
            *(u8 *)(c + i * 0x40 + 0x469b) = 0;
            *(u8 *)(c + i * 0x40 + 0x469c) = 0;
            *(u8 *)(c + i * 0x40 + 0x469d) = 0;
            *(int *)(c + i * 0x40 + 0x4660) = 0x80000;
            *(u16 *)(c + i * 0x40 + 0x4690) = 4;
            *(u16 *)(c + i * 0x40 + 0x4692) = 0x200;
            *(u8 *)L(c + i * 0x40 + 0x4696) = (u8)(RND * 2 >> 15);
            *(int *)(c + i * 0x40 + 0x4670) = 0x999;
            *(int *)(c + i * 0x40 + 0x4688) = 0;
            if (lvl == 0) {
                *(u8 *)L(c + i * 0x40 + 0x4696) = 1;
                func_ov006_020d66c4(c, 0);
            }
            if (lvl <= 1) {
                *(u16 *)(c + i * 0x40 + 0x468c) = (u16)((RND * 4 >> 15) * 0x1000 + 0x2000);
                *(int *)(c + i * 0x40 + 0x4664) = 0;
            } else if (mode == 0) {
                if (*(int *)(c + 0x6000 + 0x2d8) & 1) {
                    if (step != 0) {
                        *(u16 *)(c + i * 0x40 + 0x468c) = (u16)(step * (k >> 1) + 0x2800);
                    } else {
                        *(u16 *)(c + i * 0x40 + 0x468c) = (u16)((RND * 4 >> 15) * 0x1000 + 0x2000);
                    }
                    *(int *)(c + i * 0x40 + 0x4664) = 0;
                    func_ov006_020d66c4(c, 0);
                } else {
                    if (step != 0) {
                        *(u16 *)(c + i * 0x40 + 0x468c) = (u16)(step * (k >> 1) + 0xa800);
                    } else {
                        *(u16 *)(c + i * 0x40 + 0x468c) = (u16)((RND * 4 >> 15) * 0x1000 + 0xa000);
                    }
                    *(int *)(c + i * 0x40 + 0x4664) = 0xb8000;
                    func_ov006_020d66c4(c, 1);
                }
            } else if (mode == 1) {
                *(u16 *)(c + i * 0x40 + 0x468c) = (u16)(accum + 0x2000);
                *(int *)(c + i * 0x40 + 0x4664) = 0;
                func_ov006_020d66c4(c, 0);
            } else {
                *(u16 *)(c + i * 0x40 + 0x468c) = (u16)(accum + 0xa000);
                *(int *)(c + i * 0x40 + 0x4664) = 0xb8000;
                func_ov006_020d66c4(c, 1);
            }
            *(int *)L(c + 0x62d8) = *(int *)L(c + 0x62d8) + 1;
            break;
        }
        accum += step;
    }

    *(u16 *)(c + 0x6200 + 0xe2) = data_ov006_0212e2e8[lvl];
}
