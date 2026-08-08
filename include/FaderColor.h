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
 * DERIVATION. _ZN9FaderWipeC1Ev (0x02017480) writes _ZTV5Fader, then
 * _ZTV15FaderBrightness, then _ZTV10FaderColor, then _ZTV9FaderWipe, in that
 * order -- so FaderColor sits between FaderBrightness and FaderWipe. The ROM's
 * own __si_class_type_info records agree: dFdColor_c's single base is
 * dFdBrightness_c.
 *
 * SIZE 0x10, and the constructor is what fixes it: after the FaderColor part is
 * initialised (`strh r2,[r4,#0xc]`), FaderWipe's own sub-object constructor is
 * handed `add r0, r4, #0x10`. The first byte past FaderColor is 0x10.
 *
 * VTABLE. _ZTV10FaderColor (0x0208eb2c) is ten slots and overrides exactly one,
 * slot 2 -- AdvanceFade. Slots 3..9 still point at FaderBrightness's functions.
 * AdvanceFade is NOT declared first here: the destructor is, so that ~FaderColor
 * is the key function and no TU defines it. An override takes its base's slot
 * whatever order it is declared in, so this costs nothing.
 */
#ifdef __cplusplus
struct FaderColor : FaderBrightness {
    /* 0x0c. Only its zero/non-zero-ness is observed: AdvanceFade picks a blend
       step of +0x10 when it is set and -0x10 when it is clear. Kept as unk_00c
       rather than renamed, because a rename cannot change codegen and belongs in
       its own commit -- and src/_ZN5Scene14StartSceneFadeEjjt.* names it. */
    u16 unk_00c;

    virtual ~FaderColor();          /* key function; see above */
    virtual void AdvanceFade();     /* slot 2 -- the only override */
};

typedef char FaderColor_size_must_be_0x10[sizeof(FaderColor) == 0x10 ? 1 : -1];
#else
/* Spelled for the C destructor translation unit, which cannot express the
   virtuals and so writes out the vptr the compiler would place. */
struct FaderColor {
    void*  vtable;      /* 0x00 */
    Fix12i currInterp;  /* 0x04 (from Fader) */
    Fix12i speed;       /* 0x08 (from Fader) */
    u16    unk_00c;     /* 0x0c */
};
#endif

#endif /* FADERCOLOR_H */
