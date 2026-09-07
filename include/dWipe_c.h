#ifndef DWIPE_C_H
#define DWIPE_C_H
#include "FaderColor.h"

/* Hardware screen wipe. Unlike FaderColor, which just drives a blend register,
 * this drives the capture/DMA path: CP15 cache flush, GX palette load, and an
 * IRQ-driven per-scanline capture. `type == 1` is the escape hatch -- every
 * override below hands that case straight back to the base class.
 *
 * A single global static (data_0209f61c), never spawned. Derivation, size and
 * vtable evidence: notes/scene-provenance.md. Its vtable storage at 0x020926f0
 * is named _ZTV7dWipe_c in config/arm9/symbols.txt, taken from the ROM's own
 * __si_class_type_info record for dWipe_c, so the destructors below can be real
 * C++ and still resolve at the link.
 *
 * dWipe_c is NOT dFdWipe_c/FaderWipe; they are unrelated classes.
 */
#ifdef __cplusplus
struct dWipe_c : FaderColor {
    /* 0x0e/0x0f reuse FaderColor's tail padding -- see notes/scene-provenance.md. */
    u8  unk_00e;         /* 0x0e */
    /* 0x0f -- capture is armed; the destructors tear it down. UNSIGNED: the
       only two reads of this field in the image, the D1 and D0 destructors at
       0x0202fc08/0x0202fbc8, both load it with `ldrb`. It was s8 here until
       those destructors became real C++ and the compiler was asked to emit the
       load itself. */
    u8  needsCleanup;
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
    u8     needsCleanup;/* 0x0f -- unsigned; see the C++ branch above */
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
