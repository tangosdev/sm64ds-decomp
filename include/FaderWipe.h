/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class FaderWipe: 5 matched functions, 4 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef FADERWIPE_H
#define FADERWIPE_H
#include "types.h"

/* fwd */
struct id;
struct FaderWipe {
    u8  pad_000[0x4];
    /* These two are Fader's, and include/Fader.h:34-35 already reconstructed them:
       Fix12i currInterp (0..0x1000, 20.12 fixed point) and Fix12i speed, whose sign
       selects the fade target. Fix12i is a typedef of s32 -- same bytes, real type
       named. Not renamed here: renaming cannot change codegen and belongs in its own
       commit. FaderWipe is one of 7 retyped classes the hierarchy pass cannot place,
       so "no ancestor declares this offset" was never checkable for it. */
    Fix12i mFadeAmount;         /* 0x004 -- Fader::currInterp */
    Fix12i unk_008;             /* 0x008 -- Fader::speed */
    u16 unk_00c;            /* 0x00c */
    u8  pad_00e[0x2];
    u8  unk_010;            /* 0x010 */
#ifdef __cplusplus
    /* methods */
    void AdvanceFade();
    void LoadAndSetFile(unsigned short id);
#endif
};

#endif
