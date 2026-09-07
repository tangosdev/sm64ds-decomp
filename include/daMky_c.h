#ifndef DAMKY_C_H
#define DAMKY_C_H
#include "types.h"
#include "dActor_c.h"
#include "ModelAnim.h"
#include "ShadowModel.h"
#include "dCcAc_c.h"
#include "dBgCh_Actr.h"
#include "PathPtr.h"

/* THREE WITNESSES:
 *
 *   daMky_c_classInit_MONKEY_THIEF / daMky_c_classInit_MONKEY_STAR
 *       fBase_c::operator new(972 = 0x3cc), dActor_c::dActor_c(), stores
 *       _ZTV7daMky_c, then the six members below in this order.
 *   _ZN7daMky_cD0Ev  five of the six destroyed in reverse (PathPtr is
 *       trivial, no dtor call), then ~dActor_c.
 *
 * SIZE 0x3cc is the factory's own literal; unk_3cb (1 byte, 0x3cb) closes
 * exactly on it under 4-byte alignment.
 *
 * Everything below 0x0d0 duplicated dActor_c's own fields under placeholder
 * names -- dActor_c ends at exactly 0x0d0, so pad_0d0 (unevidenced, 4 bytes)
 * is daMky_c's own first field. Consumers that used the old duplicated names
 * were repointed to the inherited dActor_c/fBase_c names: mActorID ->
 * actorID, unk_09c -> mVertAccel, unk_0a0 -> mTerminalVelocity, unk_0b0 ->
 * mFlags (mPosX/Y/Z, mScaleX/Y/Z, mAreaId already share dActor_c's names).
 *
 * mWithMeshClsn was mistyped `u8` at 0x194 in the generated header --
 * daMky_c_classInit_MONKEY_THIEF/_MONKEY_STAR call _ZN10dBgCh_ActrC1Ev at that
 * offset, so it is the real 0x1bc-byte member (0x194..0x350); the 0x30
 * bytes from 0x350..0x380 are genuinely unevidenced padding.
 *
 * THE VTABLE was diffed slot by slot against _ZTV8dActor_c. daMky_c overrides
 * slot 0 (InitResources), slot 3 (CleanupResources), slot 6 (Behavior),
 * slot 9 (Render), slot 12 (OnPendingDestroy, a leaf-only slot whose ROM body
 * is one `bx lr` -- the override exists only to occupy the slot), 18
 * (OnYoshiTryEat) and 19 (OnTurnIntoEgg). Every other slot holds the base's
 * own word and is inherited, so it is deliberately not redeclared here.
 * All seven are defined in src/actors/daMky_c.cpp.
 */
struct daMky_c : dActor_c {
    u8  pad_0d0[0x4];
    /* ModelAnim member, named by _ZN9ModelAnimD1Ev at +0xd4 -- a relocation the ROM build
       checks. */
    ModelAnim mModelAnim;            /* 0x0d4 */
    /* ShadowModel member, named by the class's own destructor calling
       ShadowModel's D1 at +0x138. [_ZN7daMky_cD0Ev.cpp] */
    ShadowModel mShadowModel;            /* 0x138 */
    /* dCcAc_c member, named by the class's own destructor calling
       dCcAc_c's D1 at +0x160. [_ZN7daMky_cD0Ev.cpp] */
    dCcAc_c mdCcAc_c;            /* 0x160 */
    /* dBgCh_Actr member, named by daMky_c_classInit_MONKEY_THIEF/_MONKEY_STAR's own
       C1 call and the class's own destructor's D1 call at +0x194.
       [d_a_mky_monkey_thief.c, _ZN7daMky_cD0Ev.cpp] */
    dBgCh_Actr mWithMeshClsn;            /* 0x194 */
    u8  pad_350[0x30];
    /* Second position triple, also seeded from mPos in InitResources, but with
       0x64000 (100.0) added to Y immediately after -- a point a hundred units
       above the spawn. Nothing in a matched body reads it back, so what it is
       FOR is unevidenced and these keep unk_ names.
       [_ZN7daMky_c13InitResourcesEv.cpp] */
    s32 unk_380;            /* 0x380 */
    s32 unk_384;            /* 0x384 */
    s32 unk_388;            /* 0x388 */
    /* Exact copy of mPosX/Y/Z, taken once in InitResources and never written
       again. [_ZN7daMky_c13InitResourcesEv.cpp] */
    s32 mSpawnPosX;            /* 0x38c */
    s32 mSpawnPosY;            /* 0x390 */
    s32 mSpawnPosZ;            /* 0x394 */
    /* PathPtr member, named by daMky_c_classInit_MONKEY_THIEF/_MONKEY_STAR's own C1
       call at +0x398. Trivial (no dtor), so _ZN7daMky_cD0Ev does not destroy
       it. */
    PathPtr mPathPtr;            /* 0x398 */
    u8  pad_3a0[0x8];
    s32 unk_3a8;            /* 0x3a8 */
    /* The cap-thief group. Only actor 0x10b (the MONKEY_THIEF profile) runs it: it reads the
       closest player's fBase_c::param1 (the character number, guarded < 3) into
       mCapPlayerNo, spawns actor 0x10d -- Mario's lost cap -- with
       (mCapPlayerNo << 8) | 2 as its spawn parameter, keeps the spawned actor's
       fBase_c::uniqueID in mCapUniqueID, and latches mHasSpawnedCap so it never
       spawns a second one. Behavior runs the same block again from raw offsets.
       [_ZN7daMky_c13InitResourcesEv.cpp, _ZN7daMky_c8BehaviorEv.cpp] */
    s32 mCapUniqueID;            /* 0x3ac */
    u32 mCapPlayerNo;            /* 0x3b0 */
    s32 mState;            /* 0x3b4 -- the state-machine id.  Proven: the
                             eleven EnterState* members below each write
                             one immediate here, 0..10 with none repeated
                             and none missing, and 0x02115e0c holds exactly
                             eleven 0x10-byte descriptors. */
    u8  pad_3b8[0x10];
    u8  mHasSpawnedCap;            /* 0x3c8 */
    u8  pad_3c9[0x2];
    u8  unk_3cb;            /* 0x3cb */

    /* The state machine.  Entered through func_ov030_021141a8(this, N),
       which installs descriptor N at +0x3a4 and calls its first
       pointer-to-member; each of these writes its own N to mState.  The
       class name and the numbers are the cartridge's; the word
       "EnterState" is coined.  See src/actors/daMky_c.cpp. */
    s32  EnterState0();        /* 0x02114124 -- PMF record 20 of 22 */
    s32  EnterState1();        /* 0x02113fd8 -- PMF record 5 of 22 */
    s32  EnterState2();        /* 0x02113be8 -- PMF record 7 of 22 */
    s32  EnterState3();        /* 0x02113a80 -- PMF record 16 of 22 */
    s32  EnterState4();        /* 0x0211360c -- PMF record 9 of 22 */
    s32  EnterState5();        /* 0x021132d4 -- PMF record 18 of 22 */
    s32  EnterState6();        /* 0x02112ff8 -- PMF record 3 of 22 */
    s32  EnterState7();        /* 0x02112c14 -- PMF record 4 of 22 */
    s32  EnterState8();        /* 0x02112a14 -- PMF record 0 of 22 */
    s32  EnterState9();        /* 0x02112560 -- PMF record 8 of 22 */
    s32  EnterState10();       /* 0x021123a4 -- PMF record 6 of 22 */

    virtual ~daMky_c();            /* slots 16 (D1), 17 (D0) */

    virtual s32  InitResources();         /* slot  0 */
    virtual s32  CleanupResources();      /* slot  3 */
    virtual s32  Behavior();         /* slot  6 */
    virtual s32  Render();           /* slot  9 */
    virtual void OnPendingDestroy();      /* slot 12 */
    virtual s32  OnYoshiTryEat();         /* slot 18 */
    virtual int  OnTurnIntoEgg(Player &player); /* slot 19 */
};

typedef char daMky_c_size_must_be_0x3cc[sizeof(daMky_c) == 0x3cc ? 1 : -1];

#endif
