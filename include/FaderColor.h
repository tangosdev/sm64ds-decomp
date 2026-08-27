#ifndef FADERCOLOR_H
#define FADERCOLOR_H

#include "FaderBrightness.h"

/* Colour fade: same interpolator, driven into BLDY on both engines instead of
 * MASTER_BRIGHT, with one extra field selecting which direction to blend.
 *
 * This header used to be a flat generated struct with no base class, four
 * fields, and one non-virtual method. Three of those four fields were Fader's:
 * `unk_004` is currInterp and `pad_008` covered speed. Only the u16 at 0xc is
 * FaderColor's own.
 *
 * DERIVATION. _ZN9FaderWipeC1Ev (0x02017480) writes data_0208eafc, then
 * data_0208eacc, then data_0208eb2c, then _ZTV9FaderWipe, in that
 * order -- so FaderColor sits between FaderBrightness and FaderWipe. The ROM's
 * own __si_class_type_info records agree: dFdColor_c's single base is
 * dFdBrightness_c.
 *
 * SIZE 0x10, and the constructor is what fixes it: after the FaderColor part is
 * initialised (`strh r2,[r4,#0xc]`), FaderWipe's own sub-object constructor is
 * handed `add r0, r4, #0x10`. The first byte past FaderColor is 0x10.
 *
 * VTABLE. data_0208eb2c is ten slots and overrides exactly one,
 * slot 2 -- AdvanceFade. Slots 3..9 still point at FaderBrightness's functions.
 * AdvanceFade is NOT declared first here: the destructor is, so that ~FaderColor
 * is the key function. Its D0/D1/D2 sources now define the real destructor;
 * mwcc's `_ZTV10FaderColor` relocation binds to the same ROM-proven address
 * point as data_0208eb2c. The ROM RTTI name remains dFdColor_c. An override
 * takes its base's slot whatever order it is declared in, so this costs nothing.
 */
#ifdef __cplusplus
struct FaderColor : FaderBrightness {
    /* 0x0c. Only its zero/non-zero-ness is observed here: AdvanceFade picks a
       blend step of +0x10 when it is set and -0x10 when it is clear. What
       writes it is src/_ZN8dScene_c14StartSceneFadeEjjt.*, whose parameter is
       the fade colour -- which is also the name the upstream reference header
       gives this field, so it is named for that now. */
    u16 color;

    /* Inline, and it owns exactly one store. In _ZN9FaderWipeC1Ev the
       `strh r2, [r4,#0xc]` that zeroes this field comes after this class's own
       vtable store and before FaderWipe's, which is what places it in
       FaderColor's constructor rather than a neighbour's. Inline for the same
       reason as FaderBrightness's: the ROM has no out-of-line constructor for
       this class either. */
    FaderColor();

    virtual ~FaderColor();          /* key function; see above */
    virtual void AdvanceFade();     /* slot 2 -- the only override */
};

/* Defined out of line so the declaration inside the struct is a plain
   declaration -- tools/check_header_offsets.py cannot parse a member with an
   inline body and reports the whole header UNPARSED. `inline` keeps the
   emission identical: the body still goes wherever it is used, and the ROM
   has no out-of-line constructor for this class. */
inline FaderColor::FaderColor() { color = 0; }

typedef char FaderColor_size_must_be_0x10[sizeof(FaderColor) == 0x10 ? 1 : -1];
#else
/* Spelled for remaining C consumers, which cannot express the virtuals and so
   write out the vptr the compiler would place. */
struct FaderColor {
    void*  vtable;      /* 0x00 */
    Fix12i currInterp;  /* 0x04 (from Fader) */
    Fix12i speed;       /* 0x08 (from Fader) */
    u16    color;     /* 0x0c */
};
#endif

#endif /* FADERCOLOR_H */
