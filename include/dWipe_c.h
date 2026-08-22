#ifndef DWIPE_C_H
#define DWIPE_C_H
#include "FaderColor.h"

/* Hardware screen wipe: a FaderColor concrete implementation that drives the
 * capture/DMA-based wipe transition (CP15 cache flush + GX palette load +
 * IRQ-driven per-scanline capture), instead of a flat blend register
 * (FaderColor's own path, still used when type == 1) or a 3D model
 * (FaderWipe's path -- a different, unrelated class; see include/Fader.h).
 *
 * DERIVATION. The ROM's __si_class_type_info for dWipe_c names its single
 * base dFdColor_c (== FaderColor), confirmed independently by
 * dWipe_c::AdvanceFade (0x0202f428), which forwards to
 * _ZN10FaderColor11AdvanceFadeEv when unk_014 (type) == 1.
 *
 * CONSTRUCTION. dWipe_c is not spawned; it is a single global static object,
 * data_0209f61c, placement-constructed by __sinit_02074f80 (func_0202fc40)
 * and registered for atexit teardown via func_020731dc(obj, D1, dso-handle).
 * There is no `operator new` call to read a size literal from.
 *
 * SIZE. Two independent witnesses agree on 0x2c (44 bytes):
 *   - the bss layout gap: data_0209f61c to the next symbol data_0209f648
 *     is exactly 0x2c bytes.
 *   - the field span: the highest field written is a u8 at 0x28
 *     (func_0202ed14, dWipe_c's own field-init helper called from the
 *     constructor), and 0x29 rounds up to the 4-byte-aligned 0x2c.
 *
 * MEMBERS. FaderColor's own dsize (vptr+currInterp+speed+color) is 0xe;
 * its sizeof of 0x10 leaves a 2-byte tail-padding gap at 0xe..0xf that the
 * Itanium ABI lets a derived class reuse, and dWipe_c's constructor helper
 * (func_0202ed14) does: it writes single bytes at both 0xe and 0xf before
 * touching anything past 0x10. Real, newly-added storage starts at 0x10.
 * Names are kept as unk_NNN (matching the original flat header's convention
 * for the fields it already knew about) because behavioural names are a
 * separate, later commit -- this pass is base/size/vtable only.
 *   0x0e  unk_00e -- set 1 by func_0202f58c when hardware capture starts
 *   0x0f  unk_00f -- "cleanup pending"; ~dWipe_c and the D0 both test this
 *                    before calling func_0202fb30 to tear the capture down
 *   0x10  unk_010 -- wipe state machine: 0 idle, 1 opening, 2 open,
 *                    3 closing, 4 closed
 *   0x14  unk_014 -- 0/1/2 selects which hardware palette table + blend
 *                    path SetForwardTime/SetBackwardTime program; ==1 is the
 *                    "act like plain FaderColor" escape hatch used by every
 *                    overridden method here
 *   0x18  unk_018 -- savedState; func_0202fb30 restores unk_010 from this and
 *                    clears unk_028 when a capture is cancelled mid-flight
 *   0x1c  unk_01c -- 20.12 fixed-point interpolation position, independent
 *                    of FaderColor::currInterp (the hardware path drives its
 *                    own ramp instead of the base class's)
 *   0x20  unk_020 )
 *   0x24  unk_024 )  per-frame deltas for unk_01c, same +/-0x200000 scheme
 *                     as Fader::speed but derived from `unk_014` and `frames`
 *   0x28  unk_028 -- pendingSwap companion flag to unk_018; see func_0202fb30
 *
 * VTABLE. data_020926f0 (unnamed -- like the rest of this family, only
 * FaderWipe's table has a real _ZTV symbol in config/arm9/symbols.txt) is
 * ten slots and overrides ALL of them, even the six (SetToEnd/SetToStart/
 * IsBetweenStartAndEnd/IsAtStart/IsAtEnd, slots 5..9 minus AdvanceFade) that
 * FaderColor itself still inherits unmodified from FaderBrightness: every
 * dWipe_c word differs from FaderColor's own vtable at the same slot. Two of
 * the ten (SetToEnd, SetToStart) are pure tail-call veneers to
 * FaderBrightness's implementation with no dWipe_c-specific behaviour; the
 * rest branch on `type == 1` to the same FaderBrightness/FaderColor bodies
 * and otherwise run the hardware-capture path.
 */
#ifdef __cplusplus
struct dWipe_c : FaderColor {
    u8  unk_00e;      /* 0x0e -- FaderColor tail-padding, reused (see above) */
    s8  unk_00f;      /* 0x0f -- FaderColor tail-padding, reused (see above) */
    s32 unk_010;      /* 0x10 -- wipe state machine, 0..4 */
    s32 unk_014;      /* 0x14 -- type selector; ==1 delegates to FaderColor/FaderBrightness */
    s32 unk_018;      /* 0x18 -- savedState, restored by func_0202fb30 */
    s32 unk_01c;      /* 0x1c -- interpVal, independent 20.12 ramp position */
    s32 unk_020;      /* 0x20 -- speed1 */
    s32 unk_024;      /* 0x24 -- speed2 */
    u8  unk_028;      /* 0x28 -- pendingSwap companion flag to unk_018 */

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
    s8     unk_00f;     /* 0x0f */
    s32    unk_010;     /* 0x10 */
    s32    unk_014;     /* 0x14 */
    s32    unk_018;     /* 0x18 */
    s32    unk_01c;     /* 0x1c */
    s32    unk_020;     /* 0x20 */
    s32    unk_024;     /* 0x24 */
    u8     unk_028;     /* 0x28 */
};
#endif

#endif
