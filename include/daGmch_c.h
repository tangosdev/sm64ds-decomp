#ifndef DAGMCH_C_H
#define DAGMCH_C_H

#include "types.h"
#include "dActor_c.h"
#include "ModelAnim.h"
#include "Model.h"
#include "ShadowModel.h"
#include "dCcAc_c.h"
#include "dBgCh_Actr.h"

/* TWO WITNESSES, and they close on each other:
 *
 *   Moneybag_Spawn  fBase_c::operator new(1012 = 0x3f4), dActor_c::dActor_c(), stores _ZTV8daGmch_c,
 *                   then the five members below in this order.
 *   ~daGmch_c       the same members destroyed in reverse, then ~dActor_c.
 *
 * SIZE 0x3f4 is the factory's own literal, and the trailing byte fields close exactly on it.
 *
 * THE VTABLE was diffed slot by slot against _ZTV8dActor_c (relocs.txt, ov081). Only the
 * slots declared below differ; every other slot holds the base's own word and is inherited,
 * so it is deliberately not redeclared here.
 *
 * SM64DS proves this class as daGmch_c through RTTI, allocation size and
 * vtable identity. The factory and profile spellings below are reconstructed
 * source-style names -- evidence-bounded proposals, not recovered SM64DS
 * symbols.
 *
 * daGmch_c_classInit at 0x02127adc (historical alias Moneybag_Spawn)
 * allocates 0x3f4 and installs this class's cartridge vtable. It backs the
 * GAMAGUCHI registry profile, whose descriptor at 0x02128be0 is
 * reconstructed as g_profile_GAMAGUCHI.
 */
struct daGmch_c : dActor_c {
    /* The actor whose position this one copies while mFlags bit 0x40000 (one
       of dActor_c's yoshi-mouth states) is set.  EnterState7 reads its
       mHorzSpeed, mAngleY and position, then clears the pointer.  A dActor_c
       because those are the only fields ever read through it; that it is in
       fact the player is not asserted here.
       [UpdateState6, EnterState7] */
    dActor_c *mHolder;                       /* 0x0d0 */
    ModelAnim mModelAnim;                    /* 0x0d4 */
    Model mModel;                            /* 0x138 */
    ShadowModel mShadowModel;                /* 0x188 */
    dCcAc_c mdCcAc_c;  /* 0x1b0 */
    dBgCh_Actr mWithMeshClsn;              /* 0x1e4 */
    /* InitResources assigns IDENTITY_MATRIX4X3 into it whole, and
       UpdateDrawMatrices writes the position, scaled by 8, into its
       translation row before handing it to DropShadowRadHeight.  The
       Matrix4x3 type reaches this header through Model.h.
       [InitResources, UpdateDrawMatrices] */
    Matrix4x3 mMatrix;      /* 0x3a0 */
    /* Copy of mPosX/Y/Z taken once in InitResources.
       [_ZN8daGmch_c13InitResourcesEv.cpp] */
    s32 mSpawnPosX;            /* 0x3d0 */
    s32 mSpawnPosY;            /* 0x3d4 */
    s32 mSpawnPosZ;            /* 0x3d8 */
    /* The current state's 16-byte pointer-to-member-function pair: SetState
       stores &mStateTable[n] here and the two invokers read it back.
       [SetState, CallStateEnter, CallStateUpdate] */
    void *mStatePmfPair;       /* 0x3dc */
    /* The state index itself, 0..8: every EnterStateN writes its own number
       here as its last act, and three members branch on the value.
       [EnterState0..EnterState8, ChooseNextState, CheckPlayerContact] */
    s32 mStateIndex;           /* 0x3e0 */
    /* The state ChooseNextState picked; the two animation-driven states pass it
       to SetState when their animation finishes.
       [ChooseNextState, UpdateState2, UpdateState4] */
    s32 mNextState;            /* 0x3e4 */
    u8  pad_3e8[0x4];
    /* The Y rotation UpdateDrawMatrices applies to mModel's matrix (the
       drawn-below-0x20 model, not the animated one); UpdateState0 advances it
       by 0xc00 every frame.
       [UpdateDrawMatrices, UpdateState0] */
    s16 mSpinAngleY;           /* 0x3ec */
    /* The heading UpdateState1 turns mAngleY toward, 0x2bc a frame; EnterState1
       draws it at random.
       [EnterState1, UpdateState1] */
    s16 mTargetAngleY;         /* 0x3ee */
    /* Set to 1 by InitResources. Render draws the ModelAnim only above 1 and
       the Model only at or below 0x1f, so the two overlap for 2..0x1f and the
       high values are a state in which neither is drawn.
       [_ZN8daGmch_c13InitResourcesEv.cpp, _ZN8daGmch_c6RenderEv.cpp] */
    /* UpdateState0 also reaches this offset as an `int *` through
       ApproachLinear2, so the four bytes 0x3f0..0x3f3 are read and written as
       one word there and as three separate bytes everywhere else.
       [UpdateState0, Render, InitResources] */
    u8  mState;            /* 0x3f0 */
    /* The phase within a state: the two multi-phase states switch on it and
       reset it to 0 on entry.
       [UpdateState0, UpdateState3, EnterState0, EnterState3] */
    u8  mPhase;            /* 0x3f1 */
    /* A frame countdown: two states set it and then end when
       DecIfAbove0_Byte drives it to zero.
       [EnterState1, EnterState8, UpdateState1, UpdateState8] */
    u8  mTimer;            /* 0x3f2 */
    u8  pad_3f3[0x1];

    virtual ~daGmch_c();            /* slots 16 (D1), 17 (D0) */

    virtual int   OnYoshiTryEat();               /* slot 18 */
    virtual int   OnTurnIntoEgg(Player &player); /* slot 19 */
    virtual int   OnAimedAtWithEgg();            /* slot 29 */


    /* ---------------------------------------------------------------------
     * The 26 members below are NOT cartridge names.  The ROM gives this class
     * ten mangled symbols and no more; these 26 are auto-named
     * `func_ov081_<address>` in config/arm9/overlays/ov081/symbols.txt and the
     * names here replace those.  Each is bounded by what the bytes prove and
     * nothing further -- for the state machine that is the state INDEX (the
     * literal each member writes to mStateIndex, or the literal SetState is
     * called with) and which HALF of the state's pointer-to-member pair the
     * member occupies, both of which are read straight off the 16-byte records
     * at 0x02128b38..0x02128bc8.  What each state MEANS is not proven and is
     * deliberately not asserted.
     * ------------------------------------------------------------------- */

    /* State machine: slot 0 of state N's pair is the enter half, slot 1 the
       update half.  Enter halves end by writing N to mStateIndex; update halves
       run one frame and return 1. */
    int  EnterState0();      /* 0x021276b0 */
    int  UpdateState0();     /* 0x02127558 */
    void EnterState1();      /* 0x021274c8 */
    int  UpdateState1();     /* 0x02127440 */
    int  EnterState2();      /* 0x021273e8 */
    int  UpdateState2();     /* 0x02127398 */
    int  EnterState3();      /* 0x02127314 */
    int  UpdateState3();     /* 0x02127240 */
    int  EnterState4();      /* 0x021271e8 */
    int  UpdateState4();     /* 0x02127188 */
    int  EnterState5();      /* 0x02127134 */
    int  UpdateState5();     /* 0x02127070 */
    int  EnterState6();      /* 0x02127044 */
    int  UpdateState6();     /* 0x02126fa4 */
    int  EnterState7();      /* 0x02126e28 */
    int  UpdateState7();     /* 0x02126d64 */
    int  EnterState8();      /* 0x02126c8c */
    int  UpdateState8();     /* 0x02126c20 */

    /* The three state-machine primitives, all proven by the bytes: SetState
       stores &table[n] in mStatePmfPair and tail-calls the enter half;
       CallStateEnter invokes pair slot 0 and CallStateUpdate pair slot 1. */
    void SetState(int n);    /* 0x0212777c */
    void CallStateEnter();   /* 0x02127744 */
    void CallStateUpdate();  /* 0x02127708 */

    /* Five file-local helpers.  ChooseNextState is proven by what it writes and
       tail-calls; the other four are named for the callees they are built out
       of, which is weaker -- see the notes on each definition. */
    void ChooseNextState();          /* 0x021265c8 */
    void SpawnCoinsAndDie();         /* 0x02126700 */
    void CheckPlayerContact();       /* 0x02126758 */
    int  ApplySlopeToVertSpeed(void *clsn); /* 0x02126950 */
    void UpdateDrawMatrices();       /* 0x02126a20 */

    int Behavior();
    int CleanupResources();                  /* slot  3 */
    int InitResources();
    void OnPendingDestroy();                 /* slot 12 -- empty body in the ROM */
    int Render();
};

typedef char daGmch_c_size_must_be_0x3f4[sizeof(daGmch_c) == 0x3f4 ? 1 : -1];

/* This class's vtable, for the factory's vptr store.  The declaration belongs
   here rather than in daGmch_c_classInit's body: before this promotion the
   shard d_a_gmch.c (written bare: this promotion deletes it) reached the same
   symbol through decl_common.h and carried no `extern` of its own, so a header
   is the declaration site this class already had.  A file-local `extern` would also be the one thing in
   this TU that langmode_audit counts as an unmodelled class. */
extern int _ZTV8daGmch_c[];

#endif /* DAGMCH_C_H */
