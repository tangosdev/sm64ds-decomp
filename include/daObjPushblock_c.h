#ifndef DAOBJPUSHBLOCK_C_H
#define DAOBJPUSHBLOCK_C_H

/* RECONSTRUCTED NAMES USED IN THIS HEADER. SM64DS RTTI names the
 * implementation(s) below; the registry profile object and the factory
 * spelling are Tier B reconstructions -- evidence-bounded proposals, not
 * recovered SM64DS symbols. Exact original spellings are not preserved.
 *
 *   daObjPushblock_c -- daObjPushblock_c_classInit (was PushBlock_Spawn), g_profile_PUSHBLOCK (was PushBlock_SpawnInfo)
 */

#include "types.h"

/* Derives from dBgActor_c: the destructor stores this class's vtable, then
 * dBgActor_c's -- inlined -- then destroys the dBgW_KcMbg at 0x124 and
 * the Model at 0xd4 before chaining to dActor_c. All three belong to dBgActor_c.
 * Everything this header used to restate below 0x320 was dActor_c's and
 * dBgActor_c's, and is inherited now.
 *
 * SIZE IS 0x4f4, THE LITERAL d_a_obj_pushblock.cpp passes to fBase_c::operator
 * new (1268 decimal) -- not merely the observed field span, though here they
 * agree: dBgActor_c ends at 0x320 and this class adds exactly one dBgCh_Actr
 * (0x1bc) plus six trailing scalars, landing on 0x4f4 with no padding.
 */

#include "dBgActor_c.h"
#include "dBgCh_Actr.h"

struct dActor_c;
struct Player;

struct daObjPushblock_c : dBgActor_c {
    dBgCh_Actr mWithMeshClsn;       /* 0x320 */
    s32 mHomePosX;                      /* 0x4dc */
    s32 mHomePosY;                      /* 0x4e0 */
    s32 mHomePosZ;                      /* 0x4e4 */
    s32 mGroundY;                      /* 0x4e8 */
    u32 mSlideSound;                      /* 0x4ec */
    dActor_c *mLinkedActor;                 /* 0x4f0 */

    /* --- vtable --- */
    virtual ~daObjPushblock_c() {}          /* slots 16 (D1), 17 (D0) */

    int InitResources();
    int CleanupResources();
    int Behavior();
    int Render();

    /* Slot 25, dActor_c's own OnPushed(dActor_c&) (include/dActor_c.h). Attributed
       by the vtable: _ZTV16daObjPushblock_c (ov002 0x021096b0) carries 0x020b8d3c at
       slot 25 -- vtable + 0x64 -- where _ZTV10dBgActor_c carries dActor_c's generic
       0x02010138 at the same slot. Confirmed with tools/mangle.py, not
       hand-mangled: _ZN16daObjPushblock_c8OnPushedER8dActor_c, NOT ...ERS_ -- the
       Itanium substitution only fires when the parameter type has already
       appeared earlier in the same mangled name, which happens inside
       dActor_c's own methods but not here. */
    int OnPushed(dActor_c &other);         /* slot 25 */

    /* Slot 27, dActor_c's own OnHitByMegaChar(Player&). Attributed the same way:
       _ZTV16daObjPushblock_c carries 0x020b8d14 at slot 27 -- vtable + 0x6c -- where
       _ZTV10dBgActor_c carries dActor_c's generic 0x02010124. Confirmed with
       tools/mangle.py: _ZN16daObjPushblock_c15OnHitByMegaCharER6Player. */
    void OnHitByMegaChar(Player &player);  /* slot 27 */

    /* Slot 31, dBgActor_c's own new virtual (include/dBgActor_c.h). Attributed
       the same way: _ZTV16daObjPushblock_c carries 0x020b8c9c at slot 31 -- vtable +
       0x7c -- where _ZTV10dBgActor_c carries _ZN10dBgActor_c4KillEv. Confirmed
       with tools/mangle.py: _ZN16daObjPushblock_c4KillEv. */
    void Kill();                           /* slot 31 */
};

typedef char PushBlock_size_must_be_0x4f4[sizeof(daObjPushblock_c) == 0x4f4 ? 1 : -1];

/* Retail symbols.txt exposes _ZTV at the public address point rather than the
 * compiler object's two-word ABI preamble. Actor factories that must retain
 * the exact allocator/constructor seam use this declaration for that one
 * measured address-point store. */
extern void *_ZTV16daObjPushblock_c[];

#endif /* DAOBJPUSHBLOCK_C_H */
