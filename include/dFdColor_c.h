#ifndef DFDCOLOR_C_H
#define DFDCOLOR_C_H

#include "dFdBrightness_c.h"

/* Colour fade: same interpolator, driven into BLDY on both engines instead of
 * MASTER_BRIGHT, with one extra field selecting which direction to blend.
 *
 * This header used to be a flat generated struct with no base class, four
 * fields, and one non-virtual method. Three of those four fields were dFader_c's:
 * `unk_004` is currInterp and `pad_008` covered speed. Only the u16 at 0xc is
 * dFdColor_c's own.
 *
 * DERIVATION. _ZN9dFdWipe_cC1Ev (0x02017480) writes data_0208eafc, then
 * data_0208eacc, then data_0208eb2c, then _ZTV9dFdWipe_c, in that
 * order -- so dFdColor_c sits between dFdBrightness_c and dFdWipe_c. The ROM's
 * own __si_class_type_info records agree: dFdColor_c's single base is
 * dFdBrightness_c.
 *
 * SIZE 0x10, and the constructor is what fixes it: after the dFdColor_c part is
 * initialised (`strh r2,[r4,#0xc]`), dFdWipe_c's own sub-object constructor is
 * handed `add r0, r4, #0x10`. The first byte past dFdColor_c is 0x10.
 *
 * VTABLE. data_0208eb2c is ten slots and overrides exactly one,
 * slot 2 -- AdvanceFade. Slots 3..9 still point at dFdBrightness_c's functions.
 * AdvanceFade is NOT declared first here: the destructor is, so that ~dFdColor_c
 * is the key function. Its D0/D1/D2 sources now define the real destructor;
 * mwcc's `_ZTV10dFdColor_c` relocation binds to the same ROM-proven address
 * point as data_0208eb2c. The ROM RTTI name remains dFdColor_c. An override
 * takes its base's slot whatever order it is declared in, so this costs nothing.
 */
#ifdef __cplusplus
struct dFdColor_c : dFdBrightness_c {
    /* 0x0c. Only its zero/non-zero-ness is observed here: AdvanceFade picks a
       blend step of +0x10 when it is set and -0x10 when it is clear. What
       writes it is dScene_c::StartSceneFade in src/actors/dScene_c.cpp, whose parameter is
       the fade colour -- which is also the name the upstream reference header
       gives this field, so it is named for that now. */
    u16 color;

    /* Inline, and it owns exactly one store. In _ZN9dFdWipe_cC1Ev the
       `strh r2, [r4,#0xc]` that zeroes this field comes after this class's own
       vtable store and before dFdWipe_c's, which is what places it in
       dFdColor_c's constructor rather than a neighbour's. Inline for the same
       reason as dFdBrightness_c's: the ROM has no out-of-line constructor for
       this class either. The initial interpolator value is forwarded to
       dFdBrightness_c unchanged; dWipe_c passes 0 (see that header). */
    dFdColor_c(Fix12i initial = 0x1000);

    virtual ~dFdColor_c();          /* key function; see above */
    virtual void AdvanceFade();     /* slot 2 -- the only override */
};

/* Defined out of line so the declaration inside the struct is a plain
   declaration -- tools/check_header_offsets.py cannot parse a member with an
   inline body and reports the whole header UNPARSED. `inline` keeps the
   emission identical: the body still goes wherever it is used, and the ROM
   has no out-of-line constructor for this class. */
inline dFdColor_c::dFdColor_c(Fix12i initial) : dFdBrightness_c(initial) { color = 0; }

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dFdColor_c_size_must_be_0x10[sizeof(dFdColor_c) == 0x10 ? 1 : -1];
#endif
#else
/* Spelled for remaining C consumers, which cannot express the virtuals and so
   write out the vptr the compiler would place. */
struct dFdColor_c {
    void*  vtable;      /* 0x00 */
    Fix12i currInterp;  /* 0x04 (from dFader_c) */
    Fix12i speed;       /* 0x08 (from dFader_c) */
    u16    color;     /* 0x0c */
};
#endif

#endif /* DFDCOLOR_C_H */
