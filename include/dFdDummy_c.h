#ifndef DFDDUMMY_C_H
#define DFDDUMMY_C_H
#include "FaderColor.h"

/* A no-op fade variant: overrides five of FaderColor's ten slots with trivial
 * or pass-through bodies and adds no fields of its own -- "dummy" fits.
 *
 * BASE. dFdColor_c (== FaderColor), confirmed by the RTTI edge (dFdDummy_c
 * arm9:0x0208ea0c -> dFdColor_c arm9:0x0208ea00) and independently by the
 * vtable: data_0208ea6c (10 slots, matching FaderColor's own 10) overrides
 * exactly slots 0, 1, 2, 3, 4 against data_0208eb2c (FaderColor's table);
 * slots 5..9 are still whatever FaderColor/FaderBrightness holds there.
 * SetBackwardTime/SetForwardTime (slots 3/4) each write the interp-speed
 * field and then make a genuinely virtual call through their own `this` to
 * slot 5/6 respectively -- both land on FaderColor's own inherited
 * FaderBrightness bodies, i.e. dFdDummy_c contributes only the two field
 * writes and otherwise behaves exactly like its base at those slots.
 *
 * CONSTRUCTION. Not spawned on its own. It is an embedded member of
 * dScDSMT_c (ov007) at offset 0x54: func_ov007_020ccad0 (dScDSMT_c's
 * factory) calls `_ZN7fBase_cnwEj(0x64)`, builds the outer object, places a
 * dGraph_c::callback_c-derived sub-object at 0x50, then constructs this
 * class's sub-object at 0x54 via func_02017278 -- which writes the full
 * Fader -> FaderBrightness -> FaderColor -> dFdDummy_c vtable chain and the
 * inherited fields (speed, currInterp, FaderColor's own u16 at 0xc) but
 * touches nothing past offset 0xc. dScDSMT_c's own destructor
 * (func_ov007_020cc03c / _020cc084) calls this class's base-object
 * destructor (func_02017254) at the same offset.
 *
 * SIZE 0x10 -- identical to FaderColor's own sizeof, i.e. zero added fields.
 * Two independent witnesses agree exactly:
 *   - the field-span witness: the constructor helper (func_02017278) writes
 *     no field past FaderColor's own u16 at 0xc.
 *   - the structural witness: dScDSMT_c's own total size is 0x64
 *     (fBase_c::operator new(0x64) in its factory), the embedded
 *     dFdDummy_c sub-object starts at 0x54, and 0x64 - 0x54 == 0x10.
 *
 * VTABLE. data_0208ea6c and `_ZTV10dFdDummy_c` are two symbol views of the same
 * ROM-proven address point. The destructor pair, AdvanceFade, SetBackwardTime
 * and SetForwardTime are now real C++ member definitions, so mwcc owns all five
 * mangled names and emits the vtable relocation used by D0/D1. Enrollment keeps
 * the licensed function contribution, and the exact `_ZTV10dFdDummy_c` alias
 * binds that relocation back to data_0208ea6c.
 *
 * Dtor-pair slot index confirmed 0/1 (D1/D0), matching the Fader-family
 * convention (see dWipe_c.h), not Platform's 16/17.
 */
#ifdef __cplusplus
struct dFdDummy_c : FaderColor {
    virtual ~dFdDummy_c();                       /* slots 0 (D1), 1 (D0) */
    virtual void AdvanceFade();                  /* slot 2 */
    virtual int  SetBackwardTime(u32 frames);    /* slot 3 */
    virtual int  SetForwardTime(u32 frames);     /* slot 4 */
};

typedef char dFdDummy_c_size_must_be_0x10[sizeof(dFdDummy_c) == 0x10 ? 1 : -1];
#else
/* Spelled for remaining C consumers, which cannot express the virtuals and so
   write out the vptr the compiler would place. */
struct dFdDummy_c {
    void*  vtable;      /* 0x00 */
    Fix12i currInterp;  /* 0x04 (from Fader) */
    Fix12i speed;       /* 0x08 (from Fader) */
    u16    color;       /* 0x0c (from FaderColor) */
};
#endif

#endif
