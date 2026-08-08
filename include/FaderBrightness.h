#ifndef FADERBRIGHTNESS_H
#define FADERBRIGHTNESS_H

#include "Fader.h"

/* Brightness fade: drives MASTER_BRIGHT on both engines from Fader's
 * interpolator. It adds no members of its own -- FaderBrightness::~FaderBrightness
 * writes the vptr and immediately tail-calls the Fader subobject destructor, so
 * the object is exactly a Fader with a different vtable.
 *
 * It is the only concrete implementation in the family: _ZTV15FaderBrightness
 * (0x0208eacc) fills all eight of the slots Fader leaves null, and both
 * _ZTV10FaderColor and _ZTV9FaderWipe still point at these functions for
 * everything except AdvanceFade.
 *
 * THREE OF THESE USED TO BE DECLARED NON-VIRTUAL -- IsBetweenStartAndEnd,
 * SetToEnd and SetToStart. They occupy slots 7, 8 and 9 of every concrete table
 * in the family, and the ROM calls them through the vtable: Scene::SetFaders
 * dispatches slot 9 at [vt+0x24] and slot 8 at [vt+0x20] on a fader it has only
 * as a base pointer, which a non-virtual member makes impossible to express.
 */
#ifdef __cplusplus
struct FaderBrightness : Fader {
    /* Declared first -- key function; only ever defined as an extern "C" free
       function, in D0Ev.c and D1Ev.c. */
    virtual ~FaderBrightness();

    virtual void AdvanceFade();                 /* slot 2 */
    virtual int  SetBackwardTime(u32 frames);   /* slot 3 */
    virtual int  SetForwardTime(u32 frames);    /* slot 4 */
    virtual int  IsAtStart();                   /* slot 5 */
    virtual int  IsAtEnd();                     /* slot 6 */
    virtual int  IsBetweenStartAndEnd();        /* slot 7 */
    virtual void SetToEnd();                    /* slot 8 */
    virtual void SetToStart();                  /* slot 9 */
};

typedef char FaderBrightness_size_must_be_0xc[sizeof(FaderBrightness) == 0xc ? 1 : -1];
#else
struct FaderBrightness {
    void*  vtable;      /* 0x00 */
    Fix12i currInterp;  /* 0x04 (from Fader) */
    Fix12i speed;       /* 0x08 (from Fader) */
};
#endif

#endif /* FADERBRIGHTNESS_H */
