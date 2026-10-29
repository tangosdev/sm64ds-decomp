#ifndef DEALER_H
#define DEALER_H

#include "types.h"
#include "SharedFilePtr.h"
#include "dExtBlendAnmModel_c.h"
#include "MascotWalker.h"

/* The dealer's file handles. Each constructor takes the file's ID and each
 * destructor drops the handle; the names are coined. src/minigames/
 * unit020bfec0.cpp defines them inline over the arm9 veneers the ROM calls
 * (func_02017acc / func_02017ab4 for the two models, SharedFilePtr::Construct
 * / SharedFilePtr_Destruct_Anim for the animations). */
struct DealerModelFilePtr : SharedFilePtr {
    DealerModelFilePtr(u32 fileID);
    ~DealerModelFilePtr();
};

struct DealerAnimationFileHandle : SharedFilePtr {
    DealerAnimationFileHandle(u32 fileID);
    ~DealerAnimationFileHandle();
};

/* Dealer, Luigi dealing in the card games: the 0x270-byte record the card,
 * roulette, BSC, Monte Carlo and memory scenes embed at +0x4f38. The name is coined: the class
 * has no vtable, so no RTTI names it. Its model is MG/luigi_d.bmd, and the
 * thirteen animation slots resolve (tools/asset_catalog.py) to
 * MG/luigi_d_wait, shuffle_start, shuffle, deal_start, deal, deal_end,
 * deal_end2, deal_wait, throw, win_start, win, win_end and lose.bca.
 * The constructor and destructor are the class's own (ov006 0x020c1d80 and
 * 0x020c1c64, in src/minigames/unit020bfec0.cpp), so the scenes hold a typed
 * Dealer member and the compiler builds and tears it down. The other Dealer_*
 * helpers there are free functions over a char* record and read it through
 * their DEALER() cast. */
struct Dealer {
    Dealer();
    ~Dealer();

    s32 mState[2]; /* 0x000 -- the current state's member-pointer record */
    Vector3 mPos;  /* 0x008 */
    s16 mRot[3];   /* 0x014 */
    s16 mHand; /* 0x01a -- picks deal_end (0) or deal_end2 (2); 1 makes PlayLose play deal_end first */
    /* 0x70 bytes. ResetMatrices sets its mat4x3; the state functions test its
       dExtFrameCtrl_c base with Finished and WillHitFrame and compare its file
       with mAnims. */
    dExtBlendAnmModel_c mModel; /* 0x01c */
    /* 0x50 bytes. ResetMatrices sets its mat4x3; Load clears and sets flags on
       materials 0, 4 and 5 of its data.materials. */
    dExtSimpleModel_c mTable; /* 0x08c */
    MascotWalker mWalker; /* 0x0dc */
    s32 mDealtCount;   /* 0x1d4 -- a short the deal loop bumps once per card, or 0 */
    s16 mShuffleLeft;  /* 0x1d8 -- shuffle repeats left */
    s16 mDealDoneAt;   /* 0x1da -- the repeat that calls OnDealDone */
    s16 mShuffleRound; /* 0x1dc -- 0 for the first hand (four shuffles), then one; cycles 0..5 */
    s16 mDealLeft;     /* 0x1de -- deal repeats left */
    s16 mLampTimer;    /* 0x1e0 */
    s16 mLampPattern;  /* 0x1e2 -- three lamp bits, cycling 1..7 */
    s16 mLightAngle;   /* 0x1e4 -- turns 0x400 a frame */
    s16 mReserved;     /* 0x1e6 -- the constructor clears it; the card, Monte Carlo and memory
                          scenes set it to 1 when they start a shuffle and the card and Monte
                          Carlo scenes clear it again, but nothing reads it */
    DealerModelFilePtr mModelFile;               /* 0x1e8 -- luigi_d.bmd */
    DealerModelFilePtr mTableFile;               /* 0x1f0 */
    DealerAnimationFileHandle mWaitAnim;         /* 0x1f8 -- wait.bca */
    DealerAnimationFileHandle mShuffleStartAnim; /* 0x200 -- shuffle_start.bca */
    DealerAnimationFileHandle mShuffleAnim;      /* 0x208 -- shuffle.bca */
    DealerAnimationFileHandle mDealStartAnim;    /* 0x210 -- deal_start.bca */
    DealerAnimationFileHandle mDealAnim;         /* 0x218 -- deal.bca */
    DealerAnimationFileHandle mDealEndAnim;      /* 0x220 -- deal_end.bca */
    DealerAnimationFileHandle mDealEnd2Anim;     /* 0x228 -- deal_end2.bca */
    DealerAnimationFileHandle mDealWaitAnim;     /* 0x230 -- deal_wait.bca */
    DealerAnimationFileHandle mThrowAnim;        /* 0x238 -- throw.bca */
    DealerAnimationFileHandle mWinStartAnim;     /* 0x240 -- win_start.bca */
    DealerAnimationFileHandle mWinAnim;          /* 0x248 -- win.bca */
    DealerAnimationFileHandle mWinEndAnim;       /* 0x250 -- win_end.bca */
    DealerAnimationFileHandle mLoseAnim;         /* 0x258 -- lose.bca */
    s32 mModelData;                              /* 0x260 -- the loaded luigi_d.bmd */
    s32 mTableData;                              /* 0x264 */
    s32 mShuffleSoundPlayed;                     /* 0x268 -- sound 0x139, once per shuffle */
    s32 mDealSoundPlayed;                        /* 0x26c -- sound 0x13a, once per deal */
};

#ifndef SM64DS_PLATFORM_PC
typedef char DealerModelFilePtr_size_must_be_0x8[sizeof(DealerModelFilePtr) == 0x8 ? 1 : -1];
typedef char
    DealerAnimationFileHandle_size_must_be_0x8[sizeof(DealerAnimationFileHandle) == 0x8 ? 1 : -1];
typedef char Dealer_size_must_be_0x270[sizeof(Dealer) == 0x270 ? 1 : -1];
#endif

#endif
