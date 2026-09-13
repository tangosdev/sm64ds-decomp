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
 *       _ZTV7daMky_c, then the five members below in this order.
 *   _ZN7daMky_cD0Ev  four of the five destroyed in reverse (PathPtr is
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
 * bytes from 0x350..0x380 hold the matrix copied by func_ov030_02112094
 * in src/actors/daMky_c.cpp. Its original member name is unknown.
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
       ShadowModel's D1 at +0x138.
       [src/actors/daMky_c.cpp: daMky_c::~daMky_c] */
    ShadowModel mShadowModel;            /* 0x138 */
    /* dCcAc_c member, named by the class's own destructor calling
       dCcAc_c's D1 at +0x160.
       [src/actors/daMky_c.cpp: daMky_c::~daMky_c] */
    dCcAc_c mdCcAc_c;            /* 0x160 */
    /* dBgCh_Actr member, named by daMky_c_classInit_MONKEY_THIEF/_MONKEY_STAR's own
       C1 call and the class's own destructor's D1 call at +0x194.
       [src/d_a_mky_monkey_thief.c,
        src/actors/daMky_c.cpp: daMky_c::~daMky_c] */
    dBgCh_Actr mWithMeshClsn;            /* 0x194 */
    /* 02112094 copies IDENTITY-rotated model matrix here and stores
       &mCapMtx on the spawned cap at +0xc8. */
    s32 mCapMtx[12];        /* 0x350 */
    /* Init copies mPos then adds 0x64000 to Y. 02111734 restores from here. */
    s32 mPerchPosX;         /* 0x380 */
    s32 mPerchPosY;         /* 0x384 */
    s32 mPerchPosZ;         /* 0x388 */
    /* Copy of mPosX/Y/Z seeded in InitResources. func_ov030_02111dd0 updates
       it after a ground check; func_ov030_02111734 also resets it.
       [src/actors/daMky_c.cpp: InitResources, func_ov030_02111dd0,
        func_ov030_02111734] */
    s32 mSpawnPosX;            /* 0x38c */
    s32 mSpawnPosY;            /* 0x390 */
    s32 mSpawnPosZ;            /* 0x394 */
    /* PathPtr member, named by daMky_c_classInit_MONKEY_THIEF/_MONKEY_STAR's own C1
       call at +0x398. Trivial (no dtor), so _ZN7daMky_cD0Ev does not destroy
       it. */
    PathPtr mPathPtr;            /* 0x398 */
    u32 mPathNode;          /* 0x3a0 -- 02111b20 GetNode index */
    /* 021141a8 stores &data_ov030_02115e0c[idx]; 02114170/02114134 call through. */
    void *mStateDesc;       /* 0x3a4 */
    void *unk_3a8;          /* 0x3a8 -- ClosestPlayer or pad_0d0 copy */
    /* The cap-thief group. Only actor 0x10b (the MONKEY_THIEF profile) runs it: it reads the
       closest player's fBase_c::param1 (the character number, guarded < 3) into
       mCapPlayerNo, spawns actor 0x10d -- Mario's lost cap -- with
       (mCapPlayerNo << 8) | 2 as its spawn parameter, keeps the spawned actor's
       fBase_c::uniqueID in mCapUniqueID, and latches mHasSpawnedCap so it never
       spawns a second one. Behavior runs the same block again from raw offsets.
       [src/actors/daMky_c.cpp: InitResources, Behavior] */
    s32 mCapUniqueID;            /* 0x3ac */
    u32 mCapPlayerNo;            /* 0x3b0 */
    s32 mState;             /* 0x3b4 -- EnterState0..10 each write 0..10 */
    s32 mPrevState;         /* 0x3b8 -- 02111bc4 snapshots mState; 021141a8(this, mPrevState) */
    s32 unk_3bc;            /* 0x3bc */
    u8  pad_3c0[6];
    u8  mActionTimer;       /* 0x3c6 -- EnterState10 arms 0x1e; DecIfAbove0_Byte */
    u8  unk_3c7;            /* 0x3c7 -- this TU switches on it; not padding */
    u8  mHasSpawnedCap;     /* 0x3c8 */
    u8  pad_3c9;
    u8  mAnimIdx;           /* 0x3ca -- 02111a00 indexes data_ov030_02115bc8 */
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
    virtual void OnTurnIntoEgg(Player &player); /* slot 19 */
};

typedef char daMky_c_size_must_be_0x3cc[sizeof(daMky_c) == 0x3cc ? 1 : -1];

#endif
