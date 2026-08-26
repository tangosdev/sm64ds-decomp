#ifndef DSCSTARSEL_C_H
#define DSCSTARSEL_C_H

#include "dScene_c.h"
#include "Model.h"

/* One of dScene_c's ten direct children (see the census in dScene_c.h) --
 * the star-select screen shown before entering a course.
 *
 * BASE. dScene_c, confirmed by the RTTI edge (dScStarSel_c ov003:0x020b16bc
 * -> dScene_c arm9:0x020914d4, kind "si") and independently by the vtable:
 * _ZTV12dScStarSel_c (0x020b1704, ov003) is 18 slots, matching dScene_c's
 * own 18, and dScStarSel_c overrides exactly the same seven dScene_c gives
 * every other direct child -- 0, 3, 6, 9, 12, 16, 17 (rtti_vtables.py --own
 * dScStarSel_c). No new virtual is added.
 *
 * CONSTRUCTION. StarSelect_Spawn is the factory: it opens with
 * `_ZN7fBase_cnwEj(0x13c)`, i.e. fBase_c::operator new(0x13c). SIZE 0x13c
 * is read directly off the allocator call. The same factory then calls the
 * shared "construct N elements of size S" helper, func_020733a8(p+0x64, 2,
 * 0x50, Model::Model, Model::~Model) -- two Model objects (include/Model.h,
 * itself confirmed sizeof(Model) == 0x50) back to back at 0x064..0x104.
 * The matching destructor (func_ov003_020addfc / _020ade54) tears the same
 * range down with __destroy_arr(p+0x64, 2, 0x50, Model::~Model) before
 * calling fBase_c's D2 -- two independent witnesses agreeing on both count
 * and stride. InitResources (func_ov003_020af8a0) confirms the same two
 * offsets from the other side: it calls Model::LoadAndSetFile at arg0+0x64
 * and arg0+0xB4 (0x64 + 0x50), i.e. models[0] and models[1].
 *
 * MEMBERS BELOW 0x50 are all inherited (dScene_c/dBase_c add none of their
 * own -- see dScene_c.h). Everything from 0x050 up to the Model pair, and
 * everything after it, is read by Behavior/Render (func_ov003_020af038,
 * func_ov003_020ae6f4 -- both NONMATCHING-banner near-misses, not byte
 * gates) through dozens of scalar this-relative offsets (cursor index,
 * animation timers, per-character UI positions, fade state). Recovering
 * that scalar layout field-by-field is a separate, later pass; this header
 * only needs to hold the byte ranges it occupies, which it does as opaque
 * storage below, sized exactly by the constructor/destructor evidence
 * above (the Model pair) and the allocator literal (everything else).
 *
 * VTABLE ORDER follows dScene_c's (unchanged slot indices; dScStarSel_c
 * adds no new virtual). The destructor pair is at slots 16/17, same
 * convention as dScEntry_c and every other fBase_c-family class
 * (dScene_c.h/dScEntry_c.h): func_ov003_020addfc (D1) tears down the Model
 * pair then writes dScene_c's/dBase_c's vptrs before fBase_c::~fBase_c;
 * func_ov003_020ade54 (D0) does the same then calls
 * Memory::Deallocate(this, GAME_HEAP_PTR) -- dScStarSel_c declares no
 * operator delete of its own, so this finds dScene_c's inline copy on its
 * immediate base, same as dScEntry_c.
 */
struct dScStarSel_c : dScene_c {
    u8    unk_050[0x14];  /* 0x050 -- opaque; animation/fade scalars
                                        (Behavior/Render access as 32-bit
                                        fixed-point values at 0x50/0x54/0x58/
                                        0x5c/0x60) */
    Model models[2];                 /* 0x064 -- Model::LoadAndSetFile'd pair,
                                        see CONSTRUCTION above */
    u8    unk_104[0x38];  /* 0x104 -- opaque; cursor state, per-
                                        character UI positions and timers */

    /* Declared first -- key function; see the family convention discussed in
       dBase_c.h/dScene_c.h. The D1 source now defines this destructor as a
       genuine compiler-spelled class method. The D0 source remains a real
       destructor definition from which mwccarm emits the deleting variant.
       Those one-function objects contribute only their selected text; the
       ov003 ROM-gap data remains the production owner of this class's vtable,
       RTTI and type-name objects. This establishes the ABI shape without
       claiming the exact original EAD translation-unit spelling. */
    virtual ~dScStarSel_c();                             /* slots 16 (D1), 17 (D0) */

    /* --- overrides, in _ZTV8dScene_c/_ZTV7fBase_c order. --- */
    virtual s32  InitResources();                        /* slot  0 */
    virtual s32  CleanupResources();                     /* slot  3 */
    virtual s32  Behavior();                              /* slot  6 */
    virtual s32  Render();                                /* slot  9 */
    virtual void OnPendingDestroy();                      /* slot 12 */
};

typedef char dScStarSel_c_size_must_be_0x13c[sizeof(dScStarSel_c) == 0x13c ? 1 : -1];

#endif
