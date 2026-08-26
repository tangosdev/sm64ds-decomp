#ifndef PRINCESSPEACH_H
#define PRINCESSPEACH_H

#include "dActor_c.h"
#include "ModelAnim.h"
#include "ShadowModel.h"
#include "dCcAc_c.h"
#include "dBgCh_Actr.h"

struct Player;

/* The cartridge RTTI names this class daPeach_c. PrincessPeach is the
 * readable compatibility spelling already carried by every known virtual.
 * Its __si_class_type_info record points directly at dActor_c at offset zero,
 * and its vtable has the same 31 slots as that base's table.
 *
 * The five state pairs below are not inferred from address adjacency. Ten
 * eight-byte ROM PMF constants at 0x0212ff34..0x0212ff83 are copied into
 * data_ov085_0213055c. SetState selects one pair and calls its first member;
 * Behavior calls the second member. The descriptive helper names are useful
 * source names, not names authenticated by the cartridge.
 */
struct PrincessPeach : dActor_c {
    u8 mPad0d0[0x4];                    /* 0x0d0 */
    ModelAnim mModelAnim;               /* 0x0d4 */
    ShadowModel mShadowModel;           /* 0x0138 */
    dCcAc_c mCylinder;                  /* 0x0160 */
    dBgCh_Actr mWithMeshClsn;           /* 0x0194 */

    typedef int (PrincessPeach::*StateFunc)();
    StateFunc *mStateFuncs;             /* 0x0350 */
    s32 mStateValue;                    /* 0x0354 */
    u32 unk_358;                        /* 0x0358 */
    Player *mTalkPlayer;                /* 0x035c */
    s16 mLookVertAngle;                 /* 0x0360 */
    s16 mLookHorzAngle;                 /* 0x0362 */
    s16 mTargetLookVertAngle;           /* 0x0364 */
    s16 mTargetLookHorzAngle;           /* 0x0366 */
    u8 mTalkState;                      /* 0x0368 */
    u8 mPad369[0x3];                    /* 0x0369 */

    /* Inline is load-bearing: forcing translation units emit the genuine D1
     * and D0 without introducing a homeless D2 definition. */
    virtual ~PrincessPeach() {}

    virtual int InitResources();        /* slot  0 */
    virtual int CleanupResources();     /* slot  3 */
    virtual int Behavior();             /* slot  6 */
    virtual int Render();               /* slot  9 */
    virtual void OnPendingDestroy();    /* slot 12 */

    int InitState0();
    int State0();
    int InitState1();
    int State1();
    int InitState2();
    int State2();
    int InitState3();
    int State3();
    int InitState4();
    int State4();

    void UpdateLookAt();
    void UpdateGroundCollision(dBgCh_Actr *collision);
    /* The owned player-contact query at 0x02129f8c remains the C symbol
     * func_ov085_02129f8c. In C++ mode the same source either drops the ROM's
     * final redundant compare or re-normalizes the already-0/1 result, moving
     * the body by one or two words; no source-level exact form is known. */
    void UpdateModel();
    void CallStateBehavior();
    void CallStateInit();
    void SetState(int state);
};

typedef char PrincessPeach_size_must_be_0x36c[
    sizeof(PrincessPeach) == 0x36c ? 1 : -1];

#endif
