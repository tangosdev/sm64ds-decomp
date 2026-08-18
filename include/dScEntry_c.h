#ifndef DSCENTRY_C_H
#define DSCENTRY_C_H

#include "dScene_c.h"

/* One of dScene_c's ten direct children (see the census in dScene_c.h) --
 * the "entry" scene: whichever course/minigame is currently loaded, keyed
 * off the actor ID it was spawned with (fBase_c::actorID, inherited at
 * 0x0c -- see the note on InitResources below).
 *
 * BASE. dScene_c, confirmed by the RTTI edge (dScEntry_c ov075:0x0211c8f8
 * -> dScene_c arm9:0x020914d4) and independently by the vtable: reading
 * data_ov075_0211d304 (18 slots, matching dScene_c's own 18) against
 * _ZTV8dScene_c slot for slot, dScEntry_c overrides exactly eight --
 * 0, 1, 3, 6, 9, 12, 16, 17 -- and every other slot is still whatever
 * dScene_c's own table holds there (either dScene_c's own override or,
 * further up, fBase_c's).
 *
 * CONSTRUCTION. A real Spawn-style factory: func_ov075_0211a740 (the
 * complete-object constructor) and func_ov075_0211a854 (an identical
 * base-object variant -- the usual C1/C2 pair) both open with
 * `_ZN7fBase_cnwEj(0x288)`, i.e. fBase_c::operator new(0x288). SIZE 0x288
 * is therefore read directly off the allocator call, not inferred from
 * field span.
 *
 * The same constructor writes the vtable chain fBase_c -> (dBase_c/dScene_c
 * inlined, per dScene_c.h) -> data_ov075_0211d304 (dScEntry_c's own vtable),
 * sets two spawn-flag bits at fBase_c's own 0x13 (unrelated to this class),
 * copies an 8-byte template struct into three slots 8 bytes apart at
 * 0x54/0x5c/0x64, then calls a shared "construct N elements of size S"
 * helper (func_020733a8) twice:
 *     func_020733a8(p+0x70,  9, 0x24, ...)   -- 9 elements of 0x24 bytes,
 *                                                spanning 0x70..0x1b4
 *     func_020733a8(p+0x1b4, 4, 0x2c, ...)   -- 4 elements of 0x2c bytes,
 *                                                spanning 0x1b4..0x264
 * These line up with the two nested RTTI classes the ROM's type graph
 * records under this class -- dScEntry_c::icon_c (base dThIcon_c) and
 * dScEntry_c::graphCallback_c (base dGraph_c::callback_c) -- and the
 * destructor (func_ov075_02115ab8 / _02115b28) tears down the same two
 * ranges with a matching __destroy_arr(ptr, count, stride, dtor) before
 * calling fBase_c's D2. Recovering those two nested classes' own layouts
 * is a separate, later pass; this header only needs to hold the byte range
 * they occupy, which it does as opaque storage below.
 *
 * MEMBERS BELOW 0x50 are all inherited (dBase_c/dScene_c add none of their
 * own; fBase_c's own fields run to 0x50 -- see fBase_c.h). Nothing here
 * redeclares them; dScEntry_c::InitResources reads fBase_c::actorID
 * directly through the real inheritance chain instead of a shadow field,
 * which is what this header replaces (the flat header this superseded had
 * an "unk_00c" that was actually fBase_c::actorID misread as this class's
 * own, because a flat struct has no base to attribute it to).
 *
 * FIELDS that source evidence has actually named so far -- everything else
 * in 0x50..0x288 is left as opaque storage, sized exactly by the
 * constructor/destructor evidence above, pending a members-focused pass:
 *   0x080  unk_080  -- s32; func_ov075_0211b1cc (a non-virtual helper,
 *                      OnYoshiTryEat) indexes an array of 0x18-byte records
 *                      starting here, count given by unk_0a4
 *   0x0a0  unk_0a0  -- s16; the same helper adds 0x1111 to it each call
 *   0x0a4  unk_0a4  -- u16; element count for the unk_080 array
 *   0x283  unk_283  -- InitResources zeroes it; also read/written by Behavior
 *   0x284  unk_284  -- InitResources zeroes it; incremented by Behavior
 *   0x285  unk_285  -- InitResources zeroes it; gates a Behavior branch
 *   0x287  unk_287  -- InitResources zeroes it
 * (func_ov075_0211a2b8, dScEntry_c::Behavior, independently confirms
 * f283/f284/f285 at the same three offsets through its own local struct.
 * These four plus func_ov075_0211a410/InitResources are the only two files
 * that #include this header directly -- everything else in the family uses
 * its own local offset-based struct, matching convention.)
 *
 * VTABLE ORDER follows dScene_c's (unchanged slot indices; dScEntry_c adds
 * no new virtual). The destructor pair is at slots 16/17, NOT 0/1 -- this
 * is the fBase_c/actor-family convention (fBase_c.h), not the Fader
 * family's.
 */
struct dScEntry_c : dScene_c {
    u8  unk_050[0x30];  /* 0x050 -- opaque; holds the icon_c[9]
                                     array (0x070..0x1b4) and part of the
                                     graphCallback_c[4] array (0x1b4..) --
                                     see the derivation note above */
    s32 unk_080;                  /* 0x080 */
    u8  unk_084[0x1c];  /* 0x084 -- opaque; rest of graphCallback_c[4] */
    s16 unk_0a0;                  /* 0x0a0 */
    u8  unk_0a2[0x2];  /* 0x0a2 */
    u16 unk_0a4;                  /* 0x0a4 */
    u8  unk_0a6[0x1dd];  /* 0x0a6 -- opaque */
    u8  unk_283;                   /* 0x283 */
    u8  unk_284;                   /* 0x284 */
    u8  unk_285;                   /* 0x285 */
    u8  pad_286;                   /* 0x286 */
    u8  unk_287;                   /* 0x287 */

    /* Declared first -- key function; see the family convention discussed in
       dBase_c.h/dScene_c.h. Never defined as a real method in any TU: both
       D1 and D0 are plain functions carrying their literal mangled name
       (src/_ZN10dScEntry_cD1Ev.cpp, src/_ZN10dScEntry_cD0Ev.c). */
    virtual ~dScEntry_c();                              /* slots 16 (D1), 17 (D0) */

    /* --- overrides, in _ZTV8dScene_c/_ZTV7fBase_c order. --- */
    virtual s32  InitResources();                       /* slot  0 */
    virtual bool BeforeInitResources();                 /* slot  1 */
    virtual s32  CleanupResources();                    /* slot  3 */
    virtual s32  Behavior();                             /* slot  6 */
    virtual s32  Render();                               /* slot  9 */
    virtual void OnPendingDestroy();                     /* slot 12 */
};

typedef char dScEntry_c_size_must_be_0x288[sizeof(dScEntry_c) == 0x288 ? 1 : -1];

#endif
