/* Shell game (MG_CUP). Three cups shuffle on the sub screen; the player
 * touches the cup containing the requested item.
 *
 * mModel is the object Presenter_Construct builds and Presenter_Destruct
 * tears down. mFx is the 0x20 effects under the cups. mCup is the three
 * positions (element ctor func_0203d738, empty).
 */
#ifndef DSCMGCUP_C_H
#define DSCMGCUP_C_H
#include "dScMgSingle3DBase_c.h"

extern "C" int  Presenter_Destruct(char *t); /* decl_common.h's own signature */
extern "C" void __cxa_vec_cleanup(void *base, int count, int stride, void *dtor);
extern "C" void CupFx_ElemDtor(void);
extern "C" void NullDestructor_0203d47c(void);

/* Presenter_Construct's own offsets. Byte arrays, not the real classes:
 * a BlendModelAnim member would make ~dScMgCup_c destroy it twice. */
struct CupModelParts {
    u8  pmf[8];                  /* +0x00 -- Presenter_Update calls this */
    u8  blendModelAnim[0x70];    /* +0x08 -- BlendModelAnim */
    u8  model[0x50];             /* +0x78 -- Model */
    u8  textureSequence[0x14];   /* +0xc8 -- TextureSequence */
    u8  textureTransformer[0x14];/* +0xdc -- TextureTransformer */
    u8  filePtr[0x90];           /* +0xf0 -- 18 SharedFilePtr, through +0x178 */
    s32 pos[3];                  /* +0x180 -- Presenter_UpdateMatrix's translation; Presenter_Load zeroes it */
    s32 lean[4];                 /* +0x18c -- Presenter_TrackStylus eases these toward the touch;
                                    Presenter_Draw adds them into the model */
};

#ifndef SM64DS_PLATFORM_PC
typedef char CupModelParts_size_must_be_0x19c[sizeof(CupModelParts) == 0x19c ? 1 : -1];
#endif

/* One effect slot. CupFx_Init fills it, CupFx_Update steps
 * it, CupFx_Draw draws x>>12, y>>12. */
struct CupFx {
    s32 x;            /* +0x00 */
    s32 y;            /* +0x04 */
    s32 vx;           /* +0x08 -- added to x each tick */
    s32 vy;           /* +0x0c -- added to y each tick */
    u8  pad_10[2];    /* +0x10 */
    s8 frame;         /* +0x12 -- counts down; + row*6 indexes sCupFxFrameSprites */
    u8  row;          /* +0x13 -- 1 for kind 2; while set, y drops by 6 */
    u8  delay;        /* +0x14 -- DecIfAbove0_Byte, reloaded to 3 */
    u8  active;       /* +0x15 */
    u8  kind;         /* +0x16 -- 1 lets CupFx_Move follow the cup */
    s8  cup;          /* +0x17 */
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
    virtual void StartRound(int arg);               /* slot 18 */
    virtual void OnNewRecord();                          /* slot 20 */
    s32 Behavior();
    s32 Render();

    CupModelParts mModel; /* 0x4f38 */
    /* Presenter_TrackStylus: while mTouchLock != 0, divide these by 255 and 192
       instead of the live gTouchX / gTouchY touch. */
    s32 mTouchX;          /* 0x50d4 */
    s32 mTouchY;          /* 0x50d8 */
    s32 mTouchLock;       /* 0x50dc -- component +0x1a4; the ctor zeroes it */
    /* Presenter_PlayCheer and Presenter_PlayLose play a sound only when this is 0. */
    s32 mSuppressSound;   /* 0x50e0 -- component +0x1a8 */
    u8  pad_50e4[4];      /* 0x50e4 */
    CupFx mFx[0x20];      /* 0x50e8 -- elem dtor CupFx_ElemDtor */
    CupPos mCup[3];       /* 0x53e8 -- elem dtor NullDestructor_0203d47c */
    s32 mMidX;            /* 0x5400 -- average of the two cups' rest x */
    s32 mMidY;            /* 0x5404 -- average of the two rest y */
    s32 mHalfX;           /* 0x5408 -- mMidX minus the first cup's rest x */
    s32 mOnes[3];         /* 0x540c -- Fix12 passed to Cup_DrawOamList; it reciprocates it */
    s32 mState;           /* 0x5418 */
    s32 mTimer;           /* 0x541c */
    s32 mIds[3];          /* 0x5420 -- which cup sits in each slot */
    s32 mSwap0;           /* 0x542c -- index into mIds, from sCupSwapFirst */
    s32 mSwap1;           /* 0x5430 -- the other index, from sCupSwapSecond */
    s32 mAnim[3];         /* 0x5434 -- index into sCupAnimTable */
    s32 mFrame[3];        /* 0x5440 */
    s32 mTick[3];         /* 0x544c -- counts up to that frame's length */
    s32 mShuffleSound;    /* 0x5458 */
    u16 mShuffleAngle;    /* 0x545c */
    s16 mShuffleSpeed;    /* 0x545e */
    /* Field names below describe their observed use; they are not ROM names. */
    u8  mSwapsRemaining;  /* 0x5460 -- swaps left this round */
    u8 mRoundRow;         /* 0x5461 -- round row in sCupRoundTable */
    u8  mCupContents[3];  /* 0x5462 -- per-cup value; deed8's mode argument */
    u8  mFlags[3];        /* 0x5465 */
    u8  mTargetContent;   /* 0x5468 -- the value the HUD is asking for */
    u8  mCorrect;         /* 0x5469 -- 1 when the touched cup's mCupContents matched mTargetContent */
    u8  mFakeOut;         /* 0x546a -- the next swap turns back a third of the way in */
    u8  mSparkleShuffles; /* 0x546b -- shuffles left that trail mFx; nothing here sets it */
    u8  mFakeOutAt;       /* 0x546c -- 1..10; row 7 fakes out when mSwapsRemaining reaches it */
    u8  mRevealCup;       /* 0x546d -- first cup with a nonzero mCupContents revealed while mCorrect != 1, or 0xff */
    u8  pad_546e[2];      /* 0x546e */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dScMgCup_c_size_must_be_0x5470[sizeof(dScMgCup_c) == 0x5470 ? 1 : -1];
#endif

#endif
