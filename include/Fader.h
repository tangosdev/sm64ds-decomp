#ifndef FADER_H
#define FADER_H

#include "types.h"

/* The screen-fade interpolator base at 0x020175e8..0x0201786c.
 *
 * Every claim here is read out of the ROM, not guessed:
 *
 * LAYOUT. Fader is polymorphic -- the ROM carries _ZTV5Fader, and
 * Fader::~Fader stores it into [this+0x0]. So the vptr is at 0x0 and the first
 * data member starts at 0x4. Fader::AdvanceInterp reads a Fix12i at 0x8 and
 * passes &[this+0x4] to the 20.12 approach helper at 0x0203ae58, which pins
 * currInterp=0x4 and speed=0x8, both 4 bytes.
 *
 * VTABLE ORDER. FaderBrightness::IsBetweenStartAndEnd calls two virtuals
 * through slots 5 and 6 and returns true only when both are 0 -- "not at the
 * start and not at the end". With the Itanium ABI's two destructor slots (D1,
 * D0) at 0 and 1, that puts IsAtStart at 5 and IsAtEnd at 6 and leaves 2..4 for
 * AdvanceFade, SetBackwardTime and SetForwardTime. That exact signature list is
 * what src/_ZN15FaderBrightness14SetForwardTimeEj used to reproduce the ROM
 * before this header existed, so the ordering is verified, not merely plausible.
 *
 * FIXED POINT. currInterp runs 0..0x1000 -- 0.0..1.0 in 20.12. SetToEnd writes
 * 0x1000 and SetToStart writes 0; SetForwardTime derives speed as 1.0/frames
 * via cstd::fdiv and SetBackwardTime the same with the sign flipped, which is
 * why AdvanceInterp picks its target from the sign of speed.
 *
 * Field NAMES are inferred from behaviour and cannot change codegen, so they are
 * safe to improve. Offsets, widths and vtable slots are pinned by the bytes.
 */
#ifdef __cplusplus
struct Fader {
    Fix12i currInterp;  /* 0x04 -- current fade level, 0..0x1000 */
    Fix12i speed;       /* 0x08 -- per-frame delta; sign selects the target */

    virtual ~Fader();                          /* vtable slots 0 (D1), 1 (D0) */
    virtual void AdvanceFade();                /* slot 2 */
    virtual int SetBackwardTime(u32 frames);   /* slot 3 */
    virtual int SetForwardTime(u32 frames);    /* slot 4 */
    virtual int IsAtStart();                   /* slot 5 */
    virtual int IsAtEnd();                     /* slot 6 */

    /* Steps currInterp toward 1.0 or 0.0 depending on the sign of speed. */
    void AdvanceInterp();
};
#else
/* Same object, spelled for the C destructor translation units: C cannot express
   the virtual functions, so the vptr the compiler would place is explicit. */
struct Fader {
    void*  vtable;      /* 0x00 */
    Fix12i currInterp;  /* 0x04 */
    Fix12i speed;       /* 0x08 */
};
#endif

#endif /* FADER_H */
