#ifndef FADERBRIGHTNESS_H
#define FADERBRIGHTNESS_H

#include "Fader.h"

/* Brightness fade: drives MASTER_BRIGHT on both engines from Fader's
 * interpolator. It adds no members of its own -- FaderBrightness::~FaderBrightness
 * writes the vptr and immediately tail-calls the Fader subobject destructor, so
 * the object is exactly a Fader with a different vtable.
 */
#ifdef __cplusplus
struct FaderBrightness : Fader {
    virtual ~FaderBrightness();
    virtual void AdvanceFade();
    virtual int SetBackwardTime(u32 frames);
    virtual int SetForwardTime(u32 frames);
    virtual int IsAtStart();
    virtual int IsAtEnd();

    void SetToStart();
    void SetToEnd();
    int IsBetweenStartAndEnd();
};
#else
struct FaderBrightness {
    void*  vtable;      /* 0x00 */
    Fix12i currInterp;  /* 0x04 (from Fader) */
    Fix12i speed;       /* 0x08 (from Fader) */
};
#endif

#endif /* FADERBRIGHTNESS_H */
