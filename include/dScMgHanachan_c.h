#ifndef DSCMGHANACHAN_C_H
#define DSCMGHANACHAN_C_H
#include "dScMgBase_c.h"

/* __destroy_arr / func_ov006_020ea324: the same __destroy_arr(p, count,
   elem_size, dtor) idiom dScMgBase_c's own D1/D0 use for touchIcon_0f4 (see
   dScMgBase_c.h's file banner and src/_ZN11dScMgBase_cD1Ev.cpp) -- declared
   here, not per-destructor-file, matching dScMgBase_c.h's own placement of
   func_ov004_020b929c. */
extern "C" void __destroy_arr(void *arr, int count, int elemSize, void *dtor);
extern "C" void func_ov006_020ea324(void);

/* dScMgHanachan_c : dScMgBase_c, confirmed leaf via tools/rtti_extract.py
   (no RTTI record names it as a base). Own vtable slots: 0 (InitResources),
   3 (CleanupResources -- the ActorBase/Scene-level virtual, see
   include/ActorBase.h; NOT one of dScMgBase_c's usual six overrides), 6
   (Behavior), 9 (Render), 16 (D1), 17 (D0), 18 (own new slot, not yet
   named -- stays a raw extern "C" helper (src/func_ov006_020eda48.c), not a
   declared method, same precedent as every other dScMgBase_c leaf's slot 18).

   THE DESTRUCTOR IS NON-TRIVIAL, unlike most siblings: this class owns an
   array of 15 elements * 0x98 bytes at its own 0x4678, explicitly destroyed
   via __destroy_arr in BOTH D1 and D0 (src/_ZN15dScMgHanachan_cD1Ev.cpp and
   .../_D0Ev.cpp carry an identical body, same shape dScMgBase_c's own D1/D0
   pair uses) -- the base-D2 call and own-vtable-write are compiler
   generated, and D0's extra Memory::Deallocate comes for free from
   dScMgBase_c's own operator delete (its immediate base), no per-class copy
   needed. Neither the per-element type at 0x4678 nor the type behind the
   function-pointer-shaped value at 0x4660 has been recovered, so both stay
   raw bytes rather than invented types.

   Fields below dScMgBase_c's own 0x465d are INHERITED, not this class's
   own -- accessed via raw offsets on a char* cast (0xa8/0xac by
   InitResources through a local vtable-shim struct that calls slot 18
   polymorphically, since slot 18 stays unmigrated; 0xbc by the slot 18
   helper itself, same precedent dScMgPachinko2_c.h documents for the same
   offset). Neither is dScMgBase_c's own NAMED field -- 0xac is inside its
   pad_0ac and 0xbc is inside its pad_0bc. */
struct dScMgHanachan_c : dScMgBase_c {
    virtual ~dScMgHanachan_c();
    virtual s32 InitResources();      /* slot  0 */
    virtual s32 CleanupResources();   /* slot  3 */
    virtual s32 Behavior();           /* slot  6 */
    virtual s32 Render();             /* slot  9 */

    u8  pad_465d[0x3];
    u32 unk_4660;            /* 0x4660 -- Behavior calls through it as a
                                 pointer-to-member-function; Render reads it
                                 as a raw {int,int} pair together with
                                 unk_4664 */
    u32 unk_4664;            /* 0x4664 */
    u8  pad_4668[0x4];
    s32 unk_466c;            /* 0x466c */
    s32 unk_4670;            /* 0x4670 */
    u8  pad_4674[0x4];
    u8  unk_4678[0xf][0x98]; /* 0x4678 -- 15x own-type array, destroyed via
                                 __destroy_arr in D1/D0; see the class
                                 banner above. */
    void *unk_4f60;          /* 0x4f60 */
    u8  unk_4f64;            /* 0x4f64 */
};

#endif
