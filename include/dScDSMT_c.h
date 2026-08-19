#ifndef DSCDSMT_C_H
#define DSCDSMT_C_H
#include "dScene_c.h"

/* One of dScene_c's ten direct children (see the census in dScene_c.h) --
 * the DS Multi-Play scene, per its own coinage (DSMT = "DS MulTi"). It had
 * NO header under any name before this pass -- every one of its seven
 * resolvable vtable slots was blocked on it.
 *
 * BASE. dScene_c, confirmed independently two ways:
 *   - RTTI: dScDSMT_c's __si_class_type_info (ov007:0x02103278, name
 *     "dScDSMT_c" at 0x0210326c) names its single base dScene_c
 *     (build/rtti.json, tools/rtti_extract.py).
 *   - Vtable: _ZTV9dScDSMT_c (ov007:0x021032e8) is 18 slots, matching
 *     dScene_c's own 18 (tools/rtti_vtables.py --own dScDSMT_c). It
 *     overrides exactly seven of them -- 0, 3, 6, 9, 12, 16, 17 -- every
 *     other slot is still whatever dScene_c's own table holds there. This
 *     is the fBase_c/dScene_c convention: no slot 1 (BeforeInitResources)
 *     override, matching dScMiniGm_c and unlike dScEntry_c.
 *
 * CONSTRUCTION. func_ov007_020ccad0 is the factory: it opens with
 * `_ZN7fBase_cnwEj(0x64)` -- fBase_c::operator new(0x64). SIZE 0x64 (100
 * bytes) therefore comes directly off the allocator call.
 *
 * The same function writes the vtable chain fBase_c -> (dBase_c/dScene_c
 * inlined) -> data_ov007_021032e8 (dScDSMT_c's own vtable), ORs two spawn-
 * flag bits at fBase_c's own 0x13 (unrelated to this class), then at 0x50
 * writes a nested object's vtable word twice (data_0208ee14, then
 * data_ov007_021032b0 -- a base-then-derived vptr-store pair) and calls the
 * shared helper func_02017278(p + 0x54) to finish constructing it. The
 * destructor (func_ov007_020cc028 / _020cc070) tears down the same range
 * with func_02017254(t + 0x54) before calling fBase_c's D2.
 *
 * That range (0x50..0x64, the entire 0x14 bytes dScDSMT_c adds over
 * dScene_c's own 0x50) lines up with the nested RTTI class the ROM's type
 * graph records under this class, dScDSMT_c::graphCallback_c (base
 * dGraph_c::callback_c -- config/arm9/overlays/ov007/symbols.txt has
 * _ZTIN9dScDSMT_c15graphCallback_cE / _ZTSN9dScDSMT_c15graphCallback_cE
 * right next to dScDSMT_c's own RTTI). Two of its slots are independently
 * resolvable (GraphCallback0/2 at slots 0/2 of dGraph_c::callback_c's own
 * table) but recovering graphCallback_c's own internal layout is a
 * separate, later pass -- this header only needs to hold the byte range it
 * occupies, which it does as opaque storage below. (dScDSMT_c::Behavior
 * calls dScene_c::SetFaders with an argument built from this same range,
 * which is consistent with a Fader-shaped member living here without
 * pinning its exact sub-offset.)
 *
 * VTABLE ORDER follows dScene_c's; dScDSMT_c adds no new virtual. The
 * destructor pair is at slots 16/17 (the fBase_c/dScene_c convention),
 * confirmed directly by tools/rtti_vtables.py against the anchors
 * _ZN8dScene_cD1Ev / _ZN8dScene_cD0Ev.
 */
struct dScDSMT_c : dScene_c {
    u8  unk_050[0x14]; /* 0x050 -- opaque; holds the graphCallback_c
                                    member constructed by func_02017278 and
                                    torn down by func_02017254 -- see the
                                    derivation note above */

    /* Declared first -- key function; see the family convention discussed
       in dScene_c.h. Never defined as a real method in any TU: both D1 and
       D0 are plain functions carrying their literal mangled name
       (src/_ZN9dScDSMT_cD1Ev.cpp, src/_ZN9dScDSMT_cD0Ev.c). */
    virtual ~dScDSMT_c();                                /* slots 16 (D1), 17 (D0) */

    /* --- overrides, in _ZTV8dScene_c/_ZTV7fBase_c order. --- */
    virtual s32  InitResources();                        /* slot  0 */
    virtual s32  CleanupResources();                     /* slot  3 */
    virtual s32  Behavior();                              /* slot  6 */
    virtual s32  Render();                                /* slot  9 */
    virtual void OnPendingDestroy();                      /* slot 12 */
};

typedef char dScDSMT_c_size_must_be_0x64[sizeof(dScDSMT_c) == 0x64 ? 1 : -1];

#endif
