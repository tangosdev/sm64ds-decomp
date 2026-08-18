#ifndef DSCTITLE_C_H
#define DSCTITLE_C_H

#include "dScene_c.h"

/* One of dScene_c's ten direct children (see the census in dScene_c.h) --
 * the title screen.
 *
 * BASE. dScene_c, confirmed by the RTTI edge (dScTitle_c ov003:0x020b1498
 * -> dScene_c arm9:0x020914d4, kind "si") and independently by the vtable:
 * _ZTV10dScTitle_c (0x020b1650, ov003) is 18 slots, matching dScene_c's own
 * 18, and dScTitle_c overrides exactly the same seven dScene_c gives every
 * other direct child -- 0, 3, 6, 9, 12, 16, 17 (rtti_vtables.py --own
 * dScTitle_c). No new virtual is added.
 *
 * CONSTRUCTION. func_ov003_020adc10 is the factory: it opens with
 * `_ZN7fBase_cnwEj(0x54)`, i.e. fBase_c::operator new(0x54). SIZE 0x54 is
 * read directly off the allocator call. There is no base-object-constructor
 * sibling visible in this TU; only one caller of this factory exists.
 *
 * MEMBERS BELOW 0x50 are all inherited (dScene_c/dBase_c add none of their
 * own -- see dScene_c.h). The only own storage is a 2-byte repeat-delay
 * counter at 0x050, read/written by Behavior (func_ov003_020ad814) as
 * `*(u16*)(c+0x50)` -- both as a "still held" guard on the confirm/select
 * buttons and as the argument to DecIfAbove0_Short at the end of every
 * call. 0x54 - 0x50 = 4 bytes allocated; the u16 declared below leaves the
 * other two to natural 4-byte alignment (dScene_c's vtable pointer forces
 * the whole class to 4-byte alignment), which is why no second field is
 * declared.
 *
 * VTABLE ORDER follows dScene_c's (unchanged slot indices; dScTitle_c adds
 * no new virtual). The destructor pair is at slots 16/17, same convention as
 * dScEntry_c and every other fBase_c-family class (dScene_c.h/dScEntry_c.h),
 * confirmed here too: func_ov003_020ad660 (D1) writes _ZTV10dScTitle_c then
 * tail-calls into dScene_c's/dBase_c's vptr writes before fBase_c::~fBase_c;
 * func_ov003_020ad69c (D0) does the same then calls
 * Memory::Deallocate(this, GAME_HEAP_PTR) -- dScTitle_c declares no
 * operator delete of its own, so this finds dScene_c's inline copy on its
 * immediate base, same as dScEntry_c.
 */
struct dScTitle_c : dScene_c {
    u16 unk_050;                   /* 0x050 -- repeat-delay counter, Behavior */

    /* Declared first -- key function; see the family convention discussed in
       dBase_c.h/dScene_c.h. Never defined as a real method in any TU: both
       D1 and D0 are plain functions carrying their literal mangled name. */
    virtual ~dScTitle_c();                               /* slots 16 (D1), 17 (D0) */

    /* --- overrides, in _ZTV8dScene_c/_ZTV7fBase_c order. --- */
    virtual s32  InitResources();                        /* slot  0 */
    virtual s32  CleanupResources();                     /* slot  3 */
    virtual s32  Behavior();                              /* slot  6 */
    virtual s32  Render();                                /* slot  9 */
    virtual void OnPendingDestroy();                      /* slot 12 */
};

typedef char dScTitle_c_size_must_be_0x54[sizeof(dScTitle_c) == 0x54 ? 1 : -1];

#endif
