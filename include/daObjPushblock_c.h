#ifndef DAOBJPUSHBLOCK_C_H
#define DAOBJPUSHBLOCK_C_H

/* Push Block (PUSH_BLOCK 192).
 *
 * RTTI ov002:0x0210966c names the class daObjPushblock_c; overlay_actors
 * and the debug table name the profile PUSH_BLOCK. Base is dBgActor_c
 * (RTTI edge ov002:0x021089ec). The factory spelling daObjPushblock_c_classInit
 * (historical alias PushBlock_Spawn) and g_profile_PUSHBLOCK (historical
 * alias PushBlock_SpawnInfo) are Tier B reconstructions -- evidence-bounded
 * proposals, not recovered SM64DS symbols.
 *
 * SIZE 0x4f4, the literal daObjPushblock_c_classInit passes to operator new
 * (1268 decimal). dBgActor_c ends at 0x320; this class adds one dBgCh_Actr
 * (0x1bc) plus six trailing scalars, landing on 0x4f4 with no padding.
 */

#include "types.h"
#include "dBgActor_c.h"
#include "dBgCh_Actr.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

struct dActor_c;
struct Player;

struct daObjPushblock_c : dBgActor_c {
    dBgCh_Actr mWithMeshClsn;       /* 0x320 */
    s32 mHomePosX;                  /* 0x4dc */
    s32 mHomePosY;                  /* 0x4e0 */
    s32 mHomePosZ;                  /* 0x4e4 */
    s32 mGroundY;                   /* 0x4e8 */
    u32 mSlideSound;                /* 0x4ec */
    dActor_c *mLinkedActor;         /* 0x4f0 */

    /* INLINE IS LOAD-BEARING. Out of line, mwccarm emits D0 before D1
       (cartridge is 0x020b8bf0 D1 then 0x020b8c3c D0) plus a D2 with no ROM
       home. Empty body: mWithMeshClsn teardown, the vptr store and
       dBgActor_c's two member teardowns are synthesised. Key function is
       InitResources, the first declared non-inline virtual. */
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

    static void *operator new(unsigned long size) {
        return _ZN7fBase_cnwEj((unsigned)size);
    }
};

typedef char PushBlock_size_must_be_0x4f4[sizeof(daObjPushblock_c) == 0x4f4 ? 1 : -1];

#endif /* DAOBJPUSHBLOCK_C_H */
