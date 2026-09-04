#ifndef DSCMB_C_H
#define DSCMB_C_H

/* RECONSTRUCTED NAMES USED IN THIS HEADER. SM64DS RTTI names the
 * implementation(s) below; the registry profile object and the factory
 * spelling are Tier B reconstructions -- evidence-bounded proposals, not
 * recovered SM64DS symbols. Exact original spellings are not preserved.
 *
 *   dScMB_c -- dScMB_c_classInit (was func_020352b4), g_profile_MULTIBOOT (was data_0209435c)
 */
#include "dScene_c.h"
#include "FaderColor.h"
#include "dGraph_c.h"

/* Multi-Boot scene: one of dScene_c's ten direct children (see the census in
 * dScene_c.h). Its own destructor and other matched code already carry the
 * informal coinage "MultiBootScene" -- the boot/demo path used when the DS
 * downloads the game over multiplayer.
 *
 * BASE. dScene_c, confirmed by the RTTI edge (dScMB_c arm9:0x02094370 ->
 * dScene_c arm9:0x020914d4) and independently by the vtable: data_020943c4
 * (18 slots, matching dScene_c's own 18) overrides exactly six --
 * 0, 3, 6, 9, 16, 17 -- and every other slot is still whatever dScene_c's
 * own table holds there.
 *
 * CONSTRUCTION. A real Spawn-style factory, dScMB_c_classInit: it opens with
 * `_ZN7fBase_cnwEj(0x68)`, i.e. fBase_c::operator new(0x68). SIZE 0x68 is
 * read directly off the allocator call.
 *
 * The same constructor writes the vtable chain fBase_c -> (dBase_c/dScene_c
 * inlined) -> data_020943c4 (dScMB_c's own vtable), sets the usual two
 * spawn-flag bits at fBase_c's own 0x13, then placement-constructs a
 * FaderColor member at 0x50 (writes the Fader -> FaderBrightness ->
 * FaderColor vtable chain and FaderColor's own fields, touching nothing
 * past 0x60). dScMB_c::~dScMB_c (func_02034a78/func_02034ac0) destroys the
 * same FaderColor member before calling fBase_c's own D2 -- the standard
 * "most-derived vptr write, member dtor, base-subobject vptr writes, lowest
 * base dtor" sequence.
 *
 * InitResources (func_0203506c) separately registers a GLOBAL
 * dScMB_c::graphCallback_c object (data_020a0c68, constructed by
 * func_02034b1c(&data_020a0c68, this)) with `this` as its context --
 * NOT an embedded member of dScMB_c. Recovering that nested class's own
 * layout is a separate, later pass; it occupies no space in this header.
 *
 * MEMBERS. FaderColor's own dsize is 0x10 (0x50..0x60); InitResources writes
 * its own u16 at 0x5c (FaderColor's own field, via
 * _ZN8dScene_c9SetFadersEP15FaderBrightness(this+0x50), not a new
 * dScMB_c field). Two int fields at 0x60 and 0x64 are dScMB_c's own --
 * InitResources zeroes both; Behavior uses 0x60 as a state-machine step
 * counter and 0x64 as a frame-timeout counter. 0x64 + sizeof(int) == 0x68,
 * exactly the operator-new size: field-span and allocator witnesses agree.
 *
 * VTABLE ORDER follows dScene_c's (unchanged slot indices; dScMB_c adds no
 * new virtual). The destructor pair is at slots 16/17, the fBase_c/actor-
 * family convention, not the Fader family's 0/1 -- consistent with every
 * other dScene_c child recovered so far.
 */
struct dScMB_c : dScene_c {

    class graphCallback_c;
    FaderColor fader;      /* 0x50 -- placement-constructed by InitResources,
                               destroyed by ~dScMB_c; see derivation above */
    s32 unk_060;            /* 0x60 -- Behavior state-machine step */
    s32 unk_064;            /* 0x064 -- Behavior frame-timeout counter */

    /* Declared first -- key function; see the family convention discussed in
       dBase_c.h/dScene_c.h. Never defined as a real method in any TU: both
       D1 and D0 are plain functions carrying their literal mangled name
       (src/_ZN7dScMB_cD1Ev.c, src/_ZN7dScMB_cD0Ev.c). */
    virtual ~dScMB_c();                                 /* slots 16 (D1), 17 (D0) */

    /* --- overrides, in _ZTV8dScene_c/_ZTV7fBase_c order. --- */
    virtual s32 InitResources();                        /* slot  0 */
    virtual s32 CleanupResources();                      /* slot  3 */
    virtual s32 Behavior();                               /* slot  6 */
    virtual s32 Render();                                 /* slot  9 */
};

/* The GLOBAL callback object InitResources registers (data_020a0c68), not a
   member of dScMB_c -- it occupies no space in this class; see the note above.
   Derives from dGraph_c::callback_c, the ROM-proven base whose _ZTI edge every
   scene's nested graphCallback_c points at (include/dGraph_c.h).

   The slots are declared virtual, matching dGraph_c::callback_c's own
   declarations (include/dGraph_c.h): the base supplies the vptr at +0x0,
   so mOwner falls at the +0x4 GraphCallback3 loads, and strict object
   isolation discards the vtable/RTTI passengers the function range does
   not own. */
class dScMB_c::graphCallback_c : public dGraph_c::callback_c {
public:
    void *mOwner;                                   /* 0x04 -- polled by slot 3 */
    s32   mSpinAngle;                               /* 0x08 -- 20.12 sprite spin,
                                advanced by the global frame step and zeroed
                                while the still frame is up */
    u8    mStillTimer;                              /* 0x0c -- nonzero: draw the
                                single unrotated sprite and count down */
    u8    mEnabled;                                 /* 0x0d -- nothing draws while 0 */
    u8    mAdvancing;                               /* 0x0e -- gates the spin advance */

    virtual int GraphCallback0();                           /* slot 0 */
    virtual int GraphCallback2();                           /* slot 2 */
    virtual int GraphCallback3();                           /* slot 3 */
};

typedef char dScMB_c_size_must_be_0x68[sizeof(dScMB_c) == 0x68 ? 1 : -1];

#endif
