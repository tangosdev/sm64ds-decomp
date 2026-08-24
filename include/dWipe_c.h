#ifndef DWIPE_C_H
#define DWIPE_C_H
#include "FaderColor.h"

/* Hardware screen wipe. Unlike FaderColor, which just drives a blend register,
 * this drives the capture/DMA path: CP15 cache flush, GX palette load, and an
 * IRQ-driven per-scanline capture. `type == 1` is the escape hatch -- every
 * override below hands that case straight back to the base class.
 *
 * A single global static (data_0209f61c), never spawned. Derivation, size and
 * vtable evidence: notes/scene-provenance.md.
 *
 * dWipe_c is NOT dFdWipe_c/FaderWipe; they are unrelated classes.
 */
#ifdef __cplusplus
struct dWipe_c : FaderColor {
    /* 0x0e/0x0f reuse FaderColor's tail padding -- see notes/scene-provenance.md. */
    u8  unk_00e;         /* 0x0e */
    s8  needsCleanup;    /* 0x0f -- capture is armed; the destructors tear it down */
    s32 state;           /* 0x10 -- 0 idle, 1 opening, 2 open, 3 closing, 4 closed */
    s32 type;            /* 0x14 -- palette/blend path selector; 1 defers to the base */
    s32 unk_018;         /* 0x18 */
    s32 wipeInterp;      /* 0x1c -- 20.12 ramp, independent of Fader::currInterp */
    s32 wipeSpeed;       /* 0x20 -- per-frame delta added to wipeInterp */
    s32 wipeAccel;       /* 0x24 -- per-frame delta added to wipeSpeed */
    u8  unk_028;         /* 0x28 */

    virtual ~dWipe_c();                          /* slots 0 (D1), 1 (D0) */
    virtual void AdvanceFade();                  /* slot 2 */
    virtual int  SetBackwardTime(u32 frames);    /* slot 3 */
    virtual int  SetForwardTime(u32 frames);     /* slot 4 */
    virtual int  IsAtStart();                    /* slot 5 */
    virtual int  IsAtEnd();                      /* slot 6 */
    virtual int  IsBetweenStartAndEnd();         /* slot 7 */
    virtual void SetToEnd();                     /* slot 8 */
    virtual void SetToStart();                   /* slot 9 */
};

typedef char dWipe_c_size_must_be_0x2c[sizeof(dWipe_c) == 0x2c ? 1 : -1];
#else
/* Spelled for the C destructor/leaf-method translation units, which cannot
   express the virtuals and so write out the vptr the compiler would place. */
struct dWipe_c {
    void*  vtable;      /* 0x00 */
    Fix12i currInterp;  /* 0x04 (from Fader) */
    Fix12i speed;       /* 0x08 (from Fader) */
    u16    color;       /* 0x0c (from FaderColor) */
    u8     unk_00e;     /* 0x0e */
    s8     needsCleanup;/* 0x0f */
    s32    state;       /* 0x10 */
    s32    type;        /* 0x14 */
    s32    unk_018;     /* 0x18 */
    s32    wipeInterp;  /* 0x1c */
    s32    wipeSpeed;   /* 0x20 */
    s32    wipeAccel;   /* 0x24 */
    u8     unk_028;     /* 0x28 */
};
#endif

#endif
