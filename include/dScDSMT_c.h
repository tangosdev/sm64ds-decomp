#ifndef DSCDSMT_C_H
#define DSCDSMT_C_H
#include "dScene_c.h"
#include "dFdDummy_c.h"
#include "dGraph_c.h"

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
 * CONSTRUCTION. dScDSMT_c_classInit is the factory, `return new dScDSMT_c`:
 * it opens with `_ZN7fBase_cnwEm(0x64)` -- fBase_c::operator new(0x64), so
 * SIZE 0x64 (100 bytes) comes directly off the allocator call.
 *
 * The implicit constructor that `new` inlines writes the vtable chain
 * fBase_c -> (dBase_c/dScene_c inlined) -> _ZTV9dScDSMT_c, ORs two spawn-
 * flag bits at fBase_c's own 0x13 (unrelated to this class), then constructs
 * the two members: mGraphCallback at 0x50 gets its base-then-derived vptr
 * pair (_ZTVN8dGraph_c10callback_cE, then _ZTVN9dScDSMT_c15graphCallback_cE)
 * inline, and fader at 0x54 gets a call to _ZN10dFdDummy_cC1Ev. The
 * destructor (func_ov007_020cc028 / _020cc070) tears down the same range
 * with func_02017254(t + 0x54) before calling fBase_c's D2.
 *
 * That range (0x50..0x64, the entire 0x14 bytes dScDSMT_c adds over
 * dScene_c's own 0x50) opens with the nested RTTI class the ROM's type
 * graph records under this class, dScDSMT_c::graphCallback_c (base
 * dGraph_c::callback_c -- config/arm9/overlays/ov007/symbols.txt has
 * _ZTIN9dScDSMT_c15graphCallback_cE / _ZTSN9dScDSMT_c15graphCallback_cE
 * right next to dScDSMT_c's own RTTI). It overrides slots 0 and 2
 * (GraphCallback0/2) of dGraph_c::callback_c's table and adds no fields:
 * the vptr is the whole sub-object.
 *
 * IT IS NOT THE WHOLE RANGE, though this header used to say it was. The
 * constructor writes graphCallback_c's base-then-derived vptr pair at 0x50
 * and NOTHING ELSE below 0x54, and the 0x10 bytes from 0x54 are a
 * dFdDummy_c: `_ZN9dScDSMT_cD1Ev` calls `_ZN10dFdDummy_cD1Ev` at +0x54,
 * an arm_call relocation the ROM build checks (arm9 0x02017254; both D1 and
 * D0 carry the call). tools/dtor_members.py recovers it; D1 and not D2, so
 * it is a member and not an inlined base. include/dFdDummy_c.h reached the
 * same placement from the other side -- its own construction note already
 * said "an embedded member of dScDSMT_c (ov007) at offset 0x54" -- and
 * dScDSMT_c::Behavior's `SetFaders(c + 0x54)` takes its address. 0x54 +
 * 0x10 == 0x64 closes the class exactly.
 *
 * VTABLE ORDER follows dScene_c's; dScDSMT_c adds no new virtual. The
 * destructor pair is at slots 16/17 (the fBase_c/dScene_c convention),
 * confirmed directly by tools/rtti_vtables.py against the anchors
 * _ZN8dScene_cD1Ev / _ZN8dScene_cD0Ev.
 */
struct dScDSMT_c : dScene_c {

    /* The nested class the ROM's type graph records for this scene
       (_ZTIN9dScDSMT_c15graphCallback_cE, ov007). It is defined here, not
       after the class, because mGraphCallback below holds one by value. */
    class graphCallback_c : public dGraph_c::callback_c {
    public:
        virtual int GraphCallback0(); /* slot 0 */
        virtual int GraphCallback2(); /* slot 2 */
    };

    /* 0x050 -- the constructor writes its base-then-derived vptr pair here
       and touches nothing else below 0x54 (see the note above), and
       InitResources publishes its address as the active graph callback,
       data_0209d4a8. */
    graphCallback_c mGraphCallback;
    dFdDummy_c fader;  /* 0x054 -- member. The cartridge's own ~dScDSMT_c
                                    calls _ZN10dFdDummy_cD1Ev at +0x054
                                    (D0/D1), a relocation the ROM build
                                    checks; recovered by
                                    tools/dtor_members.py. D1 and not D2, so
                                    it is this type and not an inlined base.
                                    Behavior passes &fader to SetFaders. */

    /* Inline in the class body: the TU's key function is InitResources,
       and mwccarm emits D1 then D0 from here -- the cartridge's order. See
       the family convention discussed in dScene_c.h. */
    virtual ~dScDSMT_c() {}                            /* slots 16 (D1), 17 (D0) */

    /* --- overrides, in _ZTV8dScene_c/_ZTV7fBase_c order. --- */
    virtual s32  InitResources();                        /* slot  0 */
    virtual s32  CleanupResources();                     /* slot  3 */
    virtual s32  Behavior();                              /* slot  6 */
    virtual s32  Render();                                /* slot  9 */
    virtual void OnPendingDestroy();                      /* slot 12 */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dScDSMT_c_size_must_be_0x64[sizeof(dScDSMT_c) == 0x64 ? 1 : -1];
typedef char graphCallback_c_size_must_be_0x4[sizeof(dScDSMT_c::graphCallback_c) == 0x4 ? 1 : -1];
#endif

#endif
