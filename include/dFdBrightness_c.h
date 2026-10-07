#ifndef DFDBRIGHTNESS_C_H
#define DFDBRIGHTNESS_C_H

#include "dFader_c.h"

/* Brightness fade: drives MASTER_BRIGHT on both engines from dFader_c's
 * interpolator. It adds no members of its own -- dFdBrightness_c::~dFdBrightness_c
 * writes the vptr and immediately tail-calls the dFader_c subobject destructor, so
 * the object is exactly a dFader_c with a different vtable.
 *
 * It is the only concrete implementation in the family: its vtable at
 * data_0208eacc fills all eight of the slots dFader_c leaves null, and both
 * data_0208eb2c (dFdColor_c's) and _ZTV9dFdWipe_c still point at these functions
 * for everything except AdvanceFade. `data_0208eacc` is the ROM-proven address
 * point of `_ZTV15dFdBrightness_c`, and the ROM's own RTTI record
 * (_ZTS15dFdBrightness_c) uses the same name.
 *
 * THREE OF THESE USED TO BE DECLARED NON-VIRTUAL -- IsBetweenStartAndEnd,
 * SetToEnd and SetToStart. They occupy slots 7, 8 and 9 of every concrete table
 * in the family, and the ROM calls them through the vtable: dScene_c::SetFaders
 * dispatches slot 9 at [vt+0x24] and slot 8 at [vt+0x20] on a fader it has only
 * as a base pointer, which a non-virtual member makes impossible to express.
 */
#ifdef __cplusplus
struct dFdBrightness_c : dFader_c {
    /* Inline, and this is where the interpolator's initial state is set. The
       ROM's evidence is the order inside _ZN9dFdWipe_cC1Ev (0x02017480), the
       chain's only surviving constructor: dFader_c's vtable, then THIS class's
       vtable, and only THEN `currInterp = 0x1000; speed = 0`. A field write
       that follows a sub-object's own vptr store belongs to that sub-object's
       constructor, so those two are dFdBrightness_c's, not dFader_c's. A fade
       therefore starts fully opaque and stationary. Inline because the ROM has
       no out-of-line constructor for this class: it is emitted into
       dFdWipe_c's.

       The initial value is a parameter: dWipe_c's complete-object ctor
       (0x0202fc40) runs the same vptr/store/vptr/store chain but writes
       `currInterp = 0`, so its constructor passes 0 where the fader leaves
       the default. */
    dFdBrightness_c(Fix12i initial = 0x1000);

    /* Declared first among the virtuals -- key function. The D0/D1/D2 sources
       now define the real destructor and isolate the requested variant from
       mwcc's emitted group. */
    virtual ~dFdBrightness_c();

    virtual void AdvanceFade();                 /* slot 2 */
    virtual int  SetBackwardTime(u32 frames);   /* slot 3 */
    virtual int  SetForwardTime(u32 frames);    /* slot 4 */
    virtual int  IsAtStart();                   /* slot 5 */
    virtual int  IsAtEnd();                     /* slot 6 */
    virtual int  IsBetweenStartAndEnd();        /* slot 7 */
    virtual void SetToEnd();                    /* slot 8 */
    virtual void SetToStart();                  /* slot 9 */
};

/* Defined out of line so the declaration inside the struct is a plain
   declaration -- tools/check_header_offsets.py cannot parse a member with an
   inline body and reports the whole header UNPARSED. `inline` keeps the
   emission identical: the body still goes wherever it is used, and the ROM
   has no out-of-line constructor for this class. */
inline dFdBrightness_c::dFdBrightness_c(Fix12i initial) { currInterp = initial; speed = 0; }

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dFdBrightness_c_size_must_be_0xc[sizeof(dFdBrightness_c) == 0xc ? 1 : -1];
#endif
#else
struct dFdBrightness_c {
    void*  vtable;      /* 0x00 */
    Fix12i currInterp;  /* 0x04 (from dFader_c) */
    Fix12i speed;       /* 0x08 (from dFader_c) */
};
#endif

#endif /* DFDBRIGHTNESS_C_H */
