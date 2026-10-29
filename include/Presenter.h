#ifndef PRESENTER_H
#define PRESENTER_H

#include "types.h"
#include "SharedFilePtr.h"
#include "dExtBlendAnmModel_c.h"
#include "dExtAnmTexPat_c.h"
#include "dExtAnmTexSRT_c.h"

/* The face's file handles. Each constructor takes the file's ID and each
 * destructor drops the handle; the names are coined.
 * src/minigames/unit020bfec0.cpp defines them inline over the arm9 veneers
 * the ROM calls (func_02017acc / func_02017ab4 for the two models,
 * SharedFilePtr::Construct / SharedFilePtr_Destruct_Anim for the BCA
 * animations, SharedFilePtr_Construct_TexSeq / SharedFilePtr_Destruct_TexSeq
 * for the BTP texture sequences). */
struct PresenterModelFilePtr : SharedFilePtr {
    PresenterModelFilePtr(u32 fileID);
    ~PresenterModelFilePtr();
};

struct PresenterAnimationFileHandle : SharedFilePtr {
    PresenterAnimationFileHandle(u32 fileID);
    ~PresenterAnimationFileHandle();
};

struct PresenterTexSeqFileHandle : SharedFilePtr {
    PresenterTexSeqFileHandle(u32 fileID);
    ~PresenterTexSeqFileHandle();
};

/* Presenter, the Mario face that reacts in the cup and sound games,
 * animated by the Presenter_* helpers in src/minigames/unit020bfec0.cpp. The
 * name is coined. Its model is data/DSMT/face_demo_mario.bmd in front of
 * MG/f_demo_back.bmd, and the eight animations resolve
 * (tools/asset_catalog.py) to MG/face_wait, face_mistake, face_win,
 * face_lose, face_lose_st, face_lose_ed, face_smile and face_surprised.bca,
 * each with its BTP. The constructor and destructor are the class's own
 * (ov006 0x020c33dc and 0x020c3288), so the cup and sound scenes hold a
 * typed Presenter member and the compiler tears it down. The other helpers
 * are free functions over a char* record and read it through their
 * PRESENTER() cast. The sixteen animation handles are named members, not
 * arrays: an array of a class with a destructor is torn down through
 * __cxa_vec_cleanup, and the cartridge's destructor is unrolled. */
struct Presenter {
    Presenter();
    ~Presenter();

    s32 mState[2]; /* 0x000 -- the current state's member-pointer record */
    /* 0x70 bytes. UpdateMatrix sets its mat4x3; Draw hands its data to mTexSeq
       and mTexAnim and nudges two of its materials; IsAnimDone tests its
       dExtFrameCtrl_c base and ResetPose rewinds its currFrame. */
    dExtBlendAnmModel_c mFace; /* 0x008 */
    dExtSimpleModel_c mBack;   /* 0x078 -- 0x50 bytes; Load sets its mat4x3 */
    /* Draw hands mFace's data to both; ResetPose rewinds their currFrame. */
    dExtAnmTexPat_c mTexSeq;  /* 0x0c8 */
    dExtAnmTexSRT_c mTexAnim; /* 0x0dc */
    PresenterModelFilePtr mFaceModel;            /* 0x0f0 -- face_demo_mario.bmd (0x286) */
    PresenterModelFilePtr mBackModel;            /* 0x0f8 -- f_demo_back.bmd (0x200) */
    PresenterAnimationFileHandle mWaitAnim;      /* 0x100 -- face_wait (0x1fc) */
    PresenterAnimationFileHandle mMistakeAnim;   /* 0x108 -- face_mistake (0x1f6) */
    PresenterAnimationFileHandle mWinAnim;       /* 0x110 -- face_win (0x1fe) */
    PresenterAnimationFileHandle mLoseAnim;      /* 0x118 -- face_lose (0x1f0) */
    PresenterAnimationFileHandle mLoseStartAnim; /* 0x120 -- face_lose_st (0x1f4) */
    PresenterAnimationFileHandle mLoseEndAnim;   /* 0x128 -- face_lose_ed (0x1f2) */
    PresenterAnimationFileHandle mSmileAnim;     /* 0x130 -- face_smile (0x1f8) */
    PresenterAnimationFileHandle mSurprisedAnim; /* 0x138 -- face_surprised (0x1fa) */
    /* One BTP per animation, in the same order. */
    PresenterTexSeqFileHandle mWaitTexSeq;        /* 0x140 (0x1fd) */
    PresenterTexSeqFileHandle mMistakeTexSeq;     /* 0x148 (0x1f7) */
    PresenterTexSeqFileHandle mWinTexSeq;         /* 0x150 (0x1ff) */
    PresenterTexSeqFileHandle mLoseTexSeq;        /* 0x158 (0x1f1) */
    PresenterTexSeqFileHandle mLoseStartTexSeq;   /* 0x160 (0x1f5) */
    PresenterTexSeqFileHandle mLoseEndTexSeq;     /* 0x168 (0x1f3) */
    PresenterTexSeqFileHandle mSmileTexSeq;       /* 0x170 (0x1f9) */
    PresenterTexSeqFileHandle mSurprisedTexSeq;   /* 0x178 (0x1fb) */
    Vector3 mPos;                                 /* 0x180 */
    s32 mSway[4];                                 /* 0x18c -- damped by 0xe00 each frame */
    /* The point TrackStylus turns toward while mTouchLocked is set; the cup
       scene stores the touch it locks here. */
    s32 mTouchX;      /* 0x19c */
    s32 mTouchY;      /* 0x1a0 */
    s32 mTouchLocked; /* 0x1a4 -- set: turn toward mTouchX/mTouchY, not the live touch */
    s32 mQuiet;       /* 0x1a8 -- while 0 the Play* calls play their sound (func_02012174) */
};

#ifndef SM64DS_PLATFORM_PC
typedef char PresenterModelFilePtr_size_must_be_0x8[sizeof(PresenterModelFilePtr) == 0x8 ? 1 : -1];
typedef char
    PresenterAnimationFileHandle_size_must_be_0x8[sizeof(PresenterAnimationFileHandle) == 0x8 ? 1
                                                                                              : -1];
typedef char
    PresenterTexSeqFileHandle_size_must_be_0x8[sizeof(PresenterTexSeqFileHandle) == 0x8 ? 1 : -1];
typedef char Presenter_size_must_be_0x1ac[sizeof(Presenter) == 0x1ac ? 1 : -1];
#endif

#endif
