/* Shell game (MG_CUP). Three cups shuffle on the sub screen; the player
 * touches the cup containing the requested item.
 *
 * mModel is the Mario face (include/Presenter.h); the scene's destructor
 * destroys it as a member. mFx is the 0x20 effects under the cups. mCup is
 * the three positions (element ctor func_0203d738, empty).
 */
#ifndef DSCMGCUP_C_H
#define DSCMGCUP_C_H
#include "dScMgSingle3DBase_c.h"
#include "Presenter.h"

extern "C" void __cxa_vec_cleanup(void *base, int count, int stride, void *dtor);
extern "C" void CupFx_ElemDtor(void);
extern "C" void NullDestructor_0203d47c(void);

/* One effect slot. CupFx_Init fills it, CupFx_Update steps
 * it, CupFx_Draw draws x>>12, y>>12. */
struct CupFx {
    s32 x;        /* +0x00 */
    s32 y;        /* +0x04 */
    s32 vx;       /* +0x08 -- added to x each tick */
    s32 vy;       /* +0x0c -- added to y each tick */
    u8 pad_10[2]; /* +0x10 */
    s8 frame;     /* +0x12 -- counts down; + row*6 indexes sCupFxFrameSprites */
    u8 row;       /* +0x13 -- 1 for kind 2; while set, y drops by 6 */
    u8 delay;     /* +0x14 -- DecIfAbove0_Byte, reloaded to 3 */
    u8 active;    /* +0x15 */
    u8 kind;      /* +0x16 -- 1 lets CupFx_Move follow the cup */
    s8 cup;       /* +0x17 */
};

#ifndef SM64DS_PLATFORM_PC
typedef char CupFx_size_must_be_0x18[sizeof(CupFx) == 0x18 ? 1 : -1];
#endif

/* Fix12 screen position. StateSelect subtracts x>>12, y>>12 from the touch. */
struct CupPos {
    s32 x;
    s32 y;
};

#ifndef SM64DS_PLATFORM_PC
typedef char CupPos_size_must_be_0x8[sizeof(CupPos) == 0x8 ? 1 : -1];
#endif

struct dScMgCup_c : dScMgSingle3DBase_c {
    dScMgCup_c();
    virtual ~dScMgCup_c();

    /* Eight states, dispatched by Behavior through sCupStateTable.
       The spellings are not in the ROM. */
    void StateSetup();
    void StatePrepareShuffle();
    void StateShuffle();
    void StateWaitForInput();
    void StateSelect();
    void StateResult();
    void StateFinish();
    void StateIdle();
    s32 InitResources();
    virtual void StartRound(int arg); /* slot 18 */
    virtual void OnNewRecord();       /* slot 20 */
    s32 Behavior();
    s32 Render();

    /* StateSelect locks the touched point into mModel.mTouchX/mTouchY and
       sets mModel.mTouchLocked, so the face keeps looking at the cup. */
    Presenter mModel;   /* 0x4f38 */
    u8 pad_50e4[4];     /* 0x50e4 */
    CupFx mFx[0x20];    /* 0x50e8 -- elem dtor CupFx_ElemDtor */
    CupPos mCup[3];     /* 0x53e8 -- elem dtor NullDestructor_0203d47c */
    s32 mMidX;          /* 0x5400 -- average of the two cups' rest x */
    s32 mMidY;          /* 0x5404 -- average of the two rest y */
    s32 mHalfX;         /* 0x5408 -- mMidX minus the first cup's rest x */
    s32 mCupScale[3];   /* 0x540c -- Fix12 size; converted to reciprocal for OAM rendering */
    s32 mState;         /* 0x5418 */
    s32 mTimer;         /* 0x541c */
    s32 mCupAtSlot[3];  /* 0x5420 -- which cup sits in each slot */
    s32 mSwapSlotA;     /* 0x542c -- index into mCupAtSlot, from sCupSwapFirst */
    s32 mSwapSlotB;     /* 0x5430 -- the other index, from sCupSwapSecond */
    s32 mAnim[3];       /* 0x5434 -- index into sCupAnimTable */
    s32 mFrame[3];      /* 0x5440 */
    s32 mTick[3];       /* 0x544c -- counts up to that frame's length */
    s32 mShuffleSound;  /* 0x5458 */
    u16 mShuffleAngle;  /* 0x545c */
    s16 mShuffleSpeed;  /* 0x545e */
    /* Field names below describe their observed use; they are not ROM names. */
    u8 mSwapsRemaining;     /* 0x5460 -- swaps left this round */
    u8 mRoundRow;           /* 0x5461 -- round row in sCupRoundTable */
    u8 mCupContents[3];     /* 0x5462 -- per-cup value; deed8's mode argument */
    u8 mRevealRequested[3]; /* 0x5465 -- selects reveal rather than cover animation */
    u8 mTargetContent;      /* 0x5468 -- the value the HUD is asking for */
    u8 mCorrect; /* 0x5469 -- 1 when the touched cup's mCupContents matched mTargetContent */
    u8 mFakeOut; /* 0x546a -- the next swap turns back a third of the way in */
    u8 mSparkleShuffles; /* 0x546b -- shuffles left that trail mFx; nothing here sets it */
    u8 mFakeOutAt;       /* 0x546c -- 1..10; row 7 fakes out when mSwapsRemaining reaches it */
    u8 mRevealCup; /* 0x546d -- first cup with a nonzero mCupContents revealed while mCorrect != 1, or 0xff */
    u8 pad_546e[2]; /* 0x546e */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dScMgCup_c_size_must_be_0x5470[sizeof(dScMgCup_c) == 0x5470 ? 1 : -1];
#endif

#endif
