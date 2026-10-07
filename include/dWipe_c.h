#ifndef DWIPE_C_H
#define DWIPE_C_H
#include "dFdColor_c.h"

/* Hardware screen wipe. Unlike dFdColor_c, which just drives a blend register,
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
 * dWipe_c is NOT dFdWipe_c; they are unrelated classes.
 */
#ifdef __cplusplus
struct dWipe_c : dFdColor_c {
    /* 0x0e/0x0f reuse dFdColor_c's tail padding -- see notes/scene-provenance.md. */
    u8  unk_00e;         /* 0x0e */
    /* 0x0f -- capture is armed; the destructors tear it down. UNSIGNED: the
       only two reads of this field in the image, the D1 and D0 destructors at
       0x0202fc08/0x0202fbc8, both load it with `ldrb`. It was s8 here until
       those destructors became real C++ and the compiler was asked to emit the
       load itself. */
    u8  needsCleanup;
    s32 state;           /* 0x10 -- 0 idle, 1 opening, 2 open, 3 closing, 4 closed */
    s32 type;            /* 0x14 -- palette/blend path selector; 1 defers to the base */
    s32 unk_018;         /* 0x18 -- stashed type while a transition is pending */
    s32 wipeInterp;      /* 0x1c -- 20.12 ramp, independent of dFader_c::currInterp */
    s32 wipeSpeed;       /* 0x20 -- per-frame delta added to wipeInterp */
    s32 wipeAccel;       /* 0x24 -- per-frame delta added to wipeSpeed */
    u8  unk_028;         /* 0x28 -- a type change is pending (see unk_018) */

    /* Its own ctor starts the interpolator at 0, not the family default
       0x1000 -- 0x0202fc40's chain writes `currInterp = 0; speed = 0`
       inside the dFdBrightness_c sub-object span, so the definition passes
       dFdColor_c(0). */
    dWipe_c();

    virtual ~dWipe_c();                          /* slots 0 (D1), 1 (D0) */
    virtual void AdvanceFade();                  /* slot 2 */
    /* Slot 3 is declared one-argument so the vtable spells the ROM's
       `_ZN7dWipe_c15SetBackwardTimeEj`; the definition in the TU is a
       two-parameter extern "C" function under that same name -- the ROM's
       function forwards a second argument in r2 to the base call, which a
       real one-argument member cannot emit (measured, notes/scene-provenance.md). */
    virtual int  SetBackwardTime(u32 frames);    /* slot 3 */
    virtual int  SetForwardTime(u32 frames);     /* slot 4 */
    virtual int  IsAtStart();                    /* slot 5 */
    virtual int  IsAtEnd();                      /* slot 6 */
    virtual int  IsBetweenStartAndEnd();         /* slot 7 */
    virtual void SetToEnd();                     /* slot 8 */
    virtual void SetToStart();                   /* slot 9 */

    /* TU-local helpers, kept under their address names. */
    int  func_0202ec9c(int type);  /* request a wipe type; stash it mid-transition */
    void func_0202ed14();          /* reset every field to the idle state */
    void func_0202ee94();          /* build the circular-mask scanline table */
    void func_0202efa0();          /* build the ramp-segment scanline table */
    void func_0202f290();          /* rebuild the staged table for `type` */
    void func_0202f58c();          /* arm blend regs, prime both tables */
    void func_0202fb30();          /* drop the HBlank IRQ, restore pending type */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dWipe_c_size_must_be_0x2c[sizeof(dWipe_c) == 0x2c ? 1 : -1];
#endif
#endif

#endif
