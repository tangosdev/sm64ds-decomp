#ifndef FADER_H
#define FADER_H

#include "types.h"

/* The screen-fade interpolator base at 0x020175e8..0x0201786c.
 *
 * Every claim here is read out of the ROM, not guessed:
 *
 * A NOTE ON THE VTABLE NAMES BELOW, because an earlier revision of this comment
 * got them wrong. Three of this family's four vtables have NO _ZTV symbol in
 * config/arm9/symbols.txt -- they are data_0208eafc (Fader), data_0208eacc
 * (FaderBrightness) and data_0208eb2c (FaderColor). Only _ZTV9FaderWipe is a
 * real name. Writing "_ZTV5Fader" reads like a symbol you could grep for and is
 * really an inference from what the table's entries resolve to. The inference is
 * sound; the spelling was not, so the addresses are used instead.
 *
 * LAYOUT. Fader is polymorphic -- the ROM carries its vtable at data_0208eafc,
 * and Fader::~Fader stores it into [this+0x0]. So the vptr is at 0x0 and the first
 * data member starts at 0x4. Fader::AdvanceInterp reads a Fix12i at 0x8 and
 * passes &[this+0x4] to the 20.12 approach helper at 0x0203ae58, which pins
 * currInterp=0x4 and speed=0x8, both 4 bytes. FaderWipe::FaderWipe writes
 * 0x1000 to [this+0x4] and 0 to [this+0x8] on the way up the chain, which is
 * the same two fields seen from the constructor side.
 *
 * VTABLE: TEN SLOTS, AND THIS HEADER USED TO CLAIM SEVEN. The old text derived
 * 0..6 from FaderBrightness::IsBetweenStartAndEnd calling slots 5 and 6, and
 * stopped there because nothing it had looked at reached higher. Slots 7, 8 and
 * 9 exist, and FaderBrightness declared their functions as ordinary non-virtual
 * members -- three functions the ROM dispatches through the vtable that no
 * header said were virtual.
 *
 * Read the vtable at data_0208eafc and the table is unambiguous, because eight of
 * its ten words are zero:
 *
 *     0208eafc  0201786c  _ZN5FaderD1Ev          slot 0
 *     0208eb00  02017848  _ZN5FaderD0Ev          slot 1
 *     0208eb04..0208eb20  00000000               slots 2..9, all null
 *
 * A null slot is a pure virtual, so Fader is ABSTRACT and declares eight of
 * them -- which is why nothing in the ROM ever instantiates one. The names and
 * order come from the concrete tables, where every slot resolves: data_0208eacc,
 * data_0208eb2c and _ZTV9FaderWipe (0x0208ea9c) are each ten entries long and
 * agree slot for slot.
 *
 * THE CHAIN is Fader -> FaderBrightness -> FaderColor -> FaderWipe, and
 * _ZN9FaderWipeC1Ev (0x02017480) is the single clearest statement of it: it
 * writes all four vtables into [this+0x0] in that exact order, 0x0208eafc then
 * 0x0208eacc then 0x0208eb2c then 0x0208ea9c, one per sub-object constructor.
 * The ROM's own type graph says the same -- tools/rtti_extract.py reads
 * __si_class_type_info records naming these classes dFader_c, dFdBrightness_c,
 * dFdColor_c and dFdWipe_c, each pointing at exactly one base, in that chain.
 * (dFdColor_c has two further children the tree has no name for at all,
 * dFdDummy_c and dWipe_c. dWipe_c is NOT dFdWipe_c; do not coin "Wipe".)
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
extern "C" void _ZN6Memory16operator_delete2EPv(void *);

struct Fader {
    Fix12i currInterp;  /* 0x04 -- current fade level, 0..0x1000 */
    Fix12i speed;       /* 0x08 -- per-frame delta; sign selects the target */

    /* Declared first so the destructor is the key function. It is only ever
       defined as an extern "C" free function, in _ZN5FaderD0Ev.c and
       _ZN5FaderD1Ev.c, so no translation unit defines it and CW 1.2 emits no
       vtable group to collide with the ROM's. */
    virtual ~Fader();                                /* slots 0 (D1), 1 (D0) */

    /* Every deleting destructor in this hierarchy ends at
       Memory::operator_delete2 (0x0203cbcc). Keeping that class delete path
       inline makes mwcc emit the ROM's direct call instead of global
       `_ZdlPv`; it adds neither object state nor a vtable slot. */
    void operator delete(void *ptr) { _ZN6Memory16operator_delete2EPv(ptr); }

    /* Pure, all eight: the corresponding words in data_0208eafc are null. */
    virtual void AdvanceFade() = 0;                  /* slot 2 */
    virtual int  SetBackwardTime(u32 frames) = 0;    /* slot 3 */
    virtual int  SetForwardTime(u32 frames) = 0;     /* slot 4 */
    virtual int  IsAtStart() = 0;                    /* slot 5 */
    virtual int  IsAtEnd() = 0;                      /* slot 6 */
    virtual int  IsBetweenStartAndEnd() = 0;         /* slot 7 */
    virtual void SetToEnd() = 0;                     /* slot 8 */
    virtual void SetToStart() = 0;                   /* slot 9 */

    /* Steps currInterp toward 1.0 or 0.0 depending on the sign of speed. */
    void AdvanceInterp();
};

typedef char Fader_size_must_be_0xc[sizeof(Fader) == 0xc ? 1 : -1];
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
