#ifndef DAMKY_C_H
#define DAMKY_C_H
#include "types.h"
#include "dActor_c.h"
#include "dExtAnmModel_c.h"
#include "dExtShadowModel_c.h"
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
 * SIZE 0x3cc is the factory's own literal; mResetTimer (1 byte, 0x3cb) closes
 * exactly on it under 4-byte alignment.
 *
 * Everything below 0x0d4 duplicated dActor_c's own fields under placeholder
 * names -- dActor_c ends at exactly 0x0d4. Its last word, mEatingPlayer, is
 * the daPly_c pointer written by the mouth/grab machinery and read by
 * HandleContact when the mouth-state flag is set. Consumers that used the old duplicated names
 * were repointed to the inherited dActor_c/fBase_c names: mActorID ->
 * actorID, unk_09c -> mVertAccel, unk_0a0 -> mTerminalVelocity, unk_0b0 ->
 * mFlags (mPos, mScale, mAreaId already share dActor_c's names).
 *
 * mWithMeshClsn was mistyped `u8` at 0x194 in the generated header --
 * daMky_c_classInit_MONKEY_THIEF/_MONKEY_STAR call _ZN10dBgCh_ActrC1Ev at that
 * offset, so it is the real 0x1bc-byte member (0x194..0x350); the 0x30
 * bytes from 0x350..0x380 hold the matrix copied by UpdateModelAndCap
 * in src/game/actors/d_a_mky.cpp. Its original member name is unknown.
 *
 * THE VTABLE was diffed slot by slot against _ZTV8dActor_c. daMky_c overrides
 * slot 0 (InitResources), slot 3 (CleanupResources), slot 6 (Behavior),
 * slot 9 (Render), slot 12 (OnPendingDestroy, a leaf-only slot whose ROM body
 * is one `bx lr` -- the override exists only to occupy the slot), 18
 * (OnYoshiTryEat) and 19 (OnTurnIntoEgg). Every other slot holds the base's
 * own word and is inherited, so it is deliberately not redeclared here.
 * All seven are defined in src/game/actors/d_a_mky.cpp.
 */
struct daMky_c : dActor_c {
    /* Both halves target int-returning methods; their results are discarded.
       The C static initializer installs eleven 16-byte pairs in .bss. */
    typedef int (daMky_c::*StateFunction)();
    struct StateFunctions {
        StateFunction enter;
        StateFunction update;
    };

    /* Member names below are inferred from use, not recovered identifiers. */
    /* dExtAnmModel_c member, named by _ZN14dExtAnmModel_cD1Ev at +0xd4 -- a relocation the ROM build
       checks. */
    dExtAnmModel_c mAnmModel; /* 0x0d4 */
    /* dExtShadowModel_c member, named by the class's own destructor calling
       dExtShadowModel_c's D1 at +0x138.
       [src/game/actors/d_a_mky.cpp: daMky_c::~daMky_c] */
    dExtShadowModel_c mShadowModel;            /* 0x138 */
    /* dCcAc_c member, named by the class's own destructor calling
       dCcAc_c's D1 at +0x160.
       [src/game/actors/d_a_mky.cpp: daMky_c::~daMky_c] */
    dCcAc_c mdCcAc_c;            /* 0x160 */
    /* dBgCh_Actr member, named by daMky_c_classInit_MONKEY_THIEF/_MONKEY_STAR's own
       C1 call and the class's own destructor's D1 call at +0x194.
       [src/game/actors/d_a_mky.cpp: daMky_c_classInit_MONKEY_THIEF,
        daMky_c_classInit_MONKEY_STAR, daMky_c::~daMky_c] */
    dBgCh_Actr mWithMeshClsn;            /* 0x194 */
    /* 02112094 builds the cap attachment matrix here and stores
       &mCapMtx on the spawned cap at +0xc8. */
    Matrix4x3 mCapMtx; /* 0x350 */
    /* Init copies mPos then adds 0x64000 to Y. 02111734 restores from here. */
    Vector3 mPerchPos; /* 0x380 */
    /* Copy of mPos seeded in InitResources. UpdateSafePos updates
       it after a ground check; ReturnToPerch also resets it.
       [src/game/actors/d_a_mky.cpp: InitResources, UpdateSafePos,
        ReturnToPerch] */
    Vector3 mSpawnPos; /* 0x38c */
    /* PathPtr member, named by daMky_c_classInit_MONKEY_THIEF/_MONKEY_STAR's own C1
       call at +0x398. Trivial (no dtor), so _ZN7daMky_cD0Ev does not destroy
       it. */
    PathPtr mPathPtr;            /* 0x398 */
    s32 mPathNode;               /* 0x3a0 -- 02111b20 GetNode index */
    /* 021141a8 stores &data_ov030_02115e0c[idx]; 02114170/02114134 call through. */
    StateFunctions *mStateDesc; /* 0x3a4 */
    daPly_c *mPlayer;           /* 0x3a8 -- closest/contact player or mEatingPlayer copy */
    /* The cap-thief group. Only actor 0x10b (the MONKEY_THIEF profile) runs it: it reads the
       closest player's fBase_c::param1 (the character number, guarded < 3) into
       mCapPlayerNo, spawns actor 0x10d -- Mario's lost cap -- with
       (mCapPlayerNo << 8) | 2 as its spawn parameter, keeps the spawned actor's
       fBase_c::uniqueID in mCapUniqueID, and latches mHasSpawnedCap to suppress
       repeated spawning until the state machine clears it. Behavior repeats
       this check when it can respawn the cap.
       [src/game/actors/d_a_mky.cpp: InitResources, Behavior] */
    s32 mCapUniqueID;            /* 0x3ac */
    u32 mCapPlayerNo;            /* 0x3b0 */
    s32 mState;             /* 0x3b4 -- EnterState0..10 each write 0..10 */
    s32 mPrevState;         /* 0x3b8 -- 02111bc4 snapshots mState; 021141a8(this, mPrevState) */
    s32 mCagePushProgress;  /* 0x3bc */
    u8  pad_3c0[6];
    u8  mActionTimer;       /* 0x3c6 -- EnterState10 arms 0x1e; DecIfAbove0_Byte */
    u8 mSubState;           /* 0x3c7 -- this TU switches on it; not padding */
    u8  mHasSpawnedCap;     /* 0x3c8 */
    u8 mPlayerWasOwnCharacter; /* 0x3c9 -- St_ThiefGrabbed_Main: daPly_c mCharacter == param1 */
    u8  mAnimIdx;           /* 0x3ca -- 02111a00 indexes data_ov030_02115bc8 */
    u8 mResetTimer;         /* 0x3cb -- counts down from 0x96 before perch-reset checks */

    /* The state machine.  Entered through ChangeState(this, N),
       which installs descriptor N at +0x3a4 and calls its first
       pointer-to-member; each of these writes its own N to mState.  The
       class name and the numbers are the cartridge's; the word
       "EnterState" is coined.  See src/game/actors/d_a_mky.cpp. */
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

    /* The helpers the ticks below call. r0 is this Ukiki; the cartridge does
     * not spell any of these names. */
    void ReturnToPerch();
    void PlayRunFootstepSound();
    void PlayIdleSounds();
    int CycleIdleAnimation();
    int FollowPath();
    int HandleContact();
    int UpdateSafePos();
    int RevertOffLedge();
    void UpdateFloorAndWalls(dBgCh_Actr *w);
    void UpdateModelAndCap();
    /* The eleven tick halves: descriptor N's second pointer-to-member, the one
       Behavior reaches through CallStateUpdate. The PMF record index is the
       cartridge's own proof of member-ness; the state number comes from the table
       at the bottom of src/game/actors/d_a_mky.cpp, whose matched __sinit copy
       order is what pairs a tick with the EnterState above that writes that
       number. The words are coined from what each body does.

           state  0  St_Follow_Main        0x02113ff0  PMF record 19 of 22
           state  1  St_KeepAway_Main      0x02113d20  PMF record 17 of 22
           state  2  St_Jump_Main          0x02113b38  PMF record 15 of 22
           state  3  St_ThiefGrabbed_Main  0x021136b0  PMF record 10 of 22
           state  4  St_StarGrabbed_Main   0x02113324  PMF record  2 of 22
           state  5  St_ThiefEaten_Main    0x02113094  PMF record 11 of 22
           state  6  St_StarEaten_Main     0x02112da0  PMF record 12 of 22
           state  7  St_LeaveMouth_Main    0x02112a84  PMF record 13 of 22
           state  8  St_PushCage_Main      0x02112578  PMF record  1 of 22
           state  9  St_Wait_Main          0x02112400  PMF record 21 of 22
           state 10  St_Flee_Main          0x021122b0  PMF record 14 of 22 */
    int St_Flee_Main();
    int St_Wait_Main();
    int St_PushCage_Main();
    int St_LeaveMouth_Main();
    int St_StarEaten_Main();
    int St_ThiefEaten_Main();
    int St_StarGrabbed_Main();
    int St_ThiefGrabbed_Main();
    int St_Jump_Main();
    int St_KeepAway_Main();
    int St_Follow_Main();
    void CallStateUpdate();
    void CallStateEnter();
    void ChangeState(int idx);

    virtual ~daMky_c();            /* slots 16 (D1), 17 (D0) */

    virtual s32  InitResources();         /* slot  0 */
    virtual s32  CleanupResources();      /* slot  3 */
    virtual s32  Behavior();         /* slot  6 */
    virtual s32  Render();           /* slot  9 */
    virtual void OnPendingDestroy();      /* slot 12 */
    virtual s32  OnYoshiTryEat();         /* slot 18 */
    virtual void OnTurnIntoEgg(daPly_c &player); /* slot 19 */
};

typedef char
    daMky_c_StateFunctions_size_must_be_0x10[sizeof(daMky_c::StateFunctions) == 0x10 ? 1 : -1];
typedef char daMky_c_size_must_be_0x3cc[sizeof(daMky_c) == 0x3cc ? 1 : -1];

#endif
