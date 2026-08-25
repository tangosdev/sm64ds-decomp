#ifndef UKIKI_H
#define UKIKI_H
#include "types.h"
#include "dActor_c.h"
#include "ModelAnim.h"
#include "ShadowModel.h"
#include "dCcAc_c.h"
#include "dBgCh_Actr.h"
#include "PathPtr.h"

/* THREE WITNESSES:
 *
 *   UkikiThief_Spawn / UkikiStar_Spawn
 *       fBase_c::operator new(972 = 0x3cc), dActor_c::dActor_c(), stores
 *       _ZTV5Ukiki, then the six members below in this order.
 *   _ZN5UkikiD0Ev  five of the six destroyed in reverse (PathPtr is
 *       trivial, no dtor call), then ~dActor_c.
 *
 * SIZE 0x3cc is the factory's own literal; unk_3cb (1 byte, 0x3cb) closes
 * exactly on it under 4-byte alignment.
 *
 * Everything below 0x0d0 duplicated dActor_c's own fields under placeholder
 * names -- dActor_c ends at exactly 0x0d0, so pad_0d0 (unevidenced, 4 bytes)
 * is Ukiki's own first field. Consumers that used the old duplicated names
 * were repointed to the inherited dActor_c/fBase_c names: mActorID ->
 * actorID, unk_09c -> mVertAccel, unk_0a0 -> mTerminalVelocity, unk_0b0 ->
 * mFlags (mPosX/Y/Z, mScaleX/Y/Z, mAreaId already share dActor_c's names).
 *
 * mWithMeshClsn was mistyped `u8` at 0x194 in the generated header --
 * UkikiThief_Spawn/UkikiStar_Spawn call _ZN10dBgCh_ActrC1Ev at that
 * offset, so it is the real 0x1bc-byte member (0x194..0x350); the 0x30
 * bytes from 0x350..0x380 are genuinely unevidenced padding.
 *
 * THE VTABLE was diffed slot by slot against _ZTV8dActor_c. Ukiki overrides
 * slot 0 (InitResources), slot 3 (CleanupResources), slot 6 (Behavior),
 * slot 9 (Render), plus a leaf-only slot (OnPendingDestroy, an extern "C"
 * empty-body free function -- src/_ZN5Ukiki16OnPendingDestroyEv.c), 18
 * (OnYoshiTryEat) and 19 (OnTurnIntoEgg). Every other slot holds the base's
 * own word and is inherited, so it is deliberately not redeclared here.
 */
struct Ukiki : dActor_c {
    u8  pad_0d0[0x4];
    /* ModelAnim member, named by _ZN9ModelAnimD1Ev at +0xd4 -- a relocation the ROM build
       checks. */
    ModelAnim mModelAnim;            /* 0x0d4 */
    /* ShadowModel member, named by the class's own destructor calling
       ShadowModel's D1 at +0x138. [_ZN5UkikiD0Ev.c] */
    ShadowModel mShadowModel;            /* 0x138 */
    /* dCcAc_c member, named by the class's own destructor calling
       dCcAc_c's D1 at +0x160. [_ZN5UkikiD0Ev.c] */
    dCcAc_c mdCcAc_c;            /* 0x160 */
    /* dBgCh_Actr member, named by UkikiThief_Spawn/UkikiStar_Spawn's own
       C1 call and the class's own destructor's D1 call at +0x194.
       [UkikiThief_Spawn.c, _ZN5UkikiD0Ev.c] */
    dBgCh_Actr mWithMeshClsn;            /* 0x194 */
    u8  pad_350[0x30];
    /* Second position triple, also seeded from mPos in InitResources, but with
       0x64000 (100.0) added to Y immediately after -- a point a hundred units
       above the spawn. Nothing in a matched body reads it back, so what it is
       FOR is unevidenced and these keep unk_ names.
       [_ZN5Ukiki13InitResourcesEv.cpp] */
    s32 unk_380;            /* 0x380 */
    s32 unk_384;            /* 0x384 */
    s32 unk_388;            /* 0x388 */
    /* Exact copy of mPosX/Y/Z, taken once in InitResources and never written
       again. [_ZN5Ukiki13InitResourcesEv.cpp] */
    s32 mSpawnPosX;            /* 0x38c */
    s32 mSpawnPosY;            /* 0x390 */
    s32 mSpawnPosZ;            /* 0x394 */
    /* PathPtr member, named by UkikiThief_Spawn/UkikiStar_Spawn's own C1
       call at +0x398. Trivial (no dtor), so _ZN5UkikiD0Ev does not destroy
       it. */
    PathPtr mPathPtr;            /* 0x398 */
    u8  pad_3a0[0x8];
    s32 unk_3a8;            /* 0x3a8 */
    /* The cap-thief group. Only actor 0x10b (UkikiThief) runs it: it reads the
       closest player's fBase_c::param1 (the character number, guarded < 3) into
       mCapPlayerNo, spawns actor 0x10d -- Mario's lost cap -- with
       (mCapPlayerNo << 8) | 2 as its spawn parameter, keeps the spawned actor's
       fBase_c::uniqueID in mCapUniqueID, and latches mHasSpawnedCap so it never
       spawns a second one. Behavior runs the same block again from raw offsets.
       [_ZN5Ukiki13InitResourcesEv.cpp, _ZN5Ukiki8BehaviorEv.cpp] */
    s32 mCapUniqueID;            /* 0x3ac */
    u32 mCapPlayerNo;            /* 0x3b0 */
    u8  pad_3b4[0x14];
    u8  mHasSpawnedCap;            /* 0x3c8 */
    u8  pad_3c9[0x2];
    u8  unk_3cb;            /* 0x3cb */

    virtual ~Ukiki();            /* slots 16 (D1), 17 (D0) */

    virtual s32  InitResources();         /* slot  0 */
    virtual s32  CleanupResources();      /* slot  3 */
    virtual s32  Behavior();         /* slot  6 */
    virtual s32  Render();           /* slot  9 */
    virtual void OnPendingDestroy();      /* slot 12 */
    virtual s32  OnYoshiTryEat();         /* slot 18 */
    virtual int  OnTurnIntoEgg(Player &player); /* slot 19 */
};

typedef char Ukiki_size_must_be_0x3cc[sizeof(Ukiki) == 0x3cc ? 1 : -1];

#endif
