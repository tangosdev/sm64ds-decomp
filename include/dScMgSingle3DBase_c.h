/* Base class for the "single 3D minigame" family -- 13 direct RTTI children
 * (card, cup, memory x2, mahjong-carlo x2, roulette, slot3, sound, BSC,
 * snowball, flower, 3DEsp). Adds a Particle::SysTracker at 0x471c. See
 * notes/minigame-provenance.md for the field evidence. */
#ifndef DSCMGSINGLE3DBASE_C_H
#define DSCMGSINGLE3DBASE_C_H
#include "dScMgBase_c.h"

/* The FOURTH shadow of Particle::SysTracker in this tree, alongside
 * include/Particle.h, include/Particle__SysTracker.h and the copy in
 * include/Stage.h. Every field below is spelled exactly as Stage.h spells it,
 * because two headers describing one class under two sets of names is a trap
 * for whoever reads them next -- this copy used to call 0x754/0x760/0x79c/0x7a8
 * mParticle1..4 and leave the head three fields unk_. Only D0/D1 files include
 * this header and none of them touch these members, so the alignment is free.
 * Merging the four into one definition is still its own change. */
namespace Particle {
struct SysTracker {
    void *mResourceFile;    /* 0x000 */
    void *mManager;         /* 0x004 */
    u8  mContents;          /* 0x008 */
    u8  pad_009[0x747];
    s32 mRunningSlidingDustSystemID;  /* 0x750 */
    u8  mRunningSlidingDustCallback;  /* 0x754 */
    u8  pad_755[0x7];
    s32 mSystemID_75c;      /* 0x75c */
    u8  mCallback_760;      /* 0x760 */
    u8  pad_761[0x7];
    s32 mBigSplashSystemID; /* 0x768 */
    u8  mBigSplashCallback; /* 0x76c */
    u8  pad_76d[0x7];
    s32 mSystemID_774;      /* 0x774 */
    u8  mCallback_778;      /* 0x778 */
    u8  pad_779[0x7];
    s32 mSystemID_780;      /* 0x780 */
    u8  mCallback_784;      /* 0x784 */
    u8  pad_785[0x7];
    s32 mRippleSystemID;    /* 0x78c */
    u8  mRippleCallback;    /* 0x790 */
    u8  pad_791[0x7];
    s32 mSystemID_798;      /* 0x798 */
    u8  mCallback_79c;      /* 0x79c */
    u8  pad_79d[0x7];
    s32 mSystemID_7a4;      /* 0x7a4 */
    u8  mCallback_7a8;      /* 0x7a8 */
    u8  pad_7a9[0x7];
    s32 mSystemID_7b0;      /* 0x7b0 */
    u8  mCallback_7b4;      /* 0x7b4 */
    u8  pad_7b5[0xb];
    s32 mSystemID_7c0;      /* 0x7c0 */
    u8  mCallback_7c4;      /* 0x7c4 */
    u8  pad_7c5[0xf];
    u8  mCallback_7d4;      /* 0x7d4 */
    u8  pad_7d5[0xf];
    u8  mCallback_7e4;      /* 0x7e4 */
    u8  pad_7e5[0xb];
    u8  mCallback_7f0;      /* 0x7f0 */
    u8  pad_7f1[0x3];
    u8  mCallback_7f4;      /* 0x7f4 */
    u8  pad_7f5[0x3];
    u8  mCallback_7f8;      /* 0x7f8 */
    u8  pad_7f9[0x3];
    s32 mCallbackParam_7fc; /* 0x7fc */
    u8  mCallback_800;      /* 0x800 */
    u8  pad_801[0x3];
    s32 mCallbackParam_804; /* 0x804 */
    u8  mCallback_808;      /* 0x808 */
    u8  pad_809[0x7];
    u8  mWeatherCallback;   /* 0x810 */
    u8  pad_811[0x3];
    u8  mWeatherCallbackCount; /* 0x814 */
    u8  pad_815[0x3];
    u8  mCallback_818;      /* 0x818 */
    u8  pad_819[0x3];       /* rounds 0x819 up to the 0x81c alignment boundary */

    /* Non-virtual: declaring them changes no layout. */
    s32 Initialise();
    s32 Update();

    ~SysTracker();
};

typedef char SysTracker_size_must_be_0x81c[sizeof(SysTracker) == 0x81c ? 1 : -1];

/* Called by dScMgSingle3DBase_c::BeforeRender. */
void RenderAll();
}

struct dScMgSingle3DBase_c : dScMgBase_c {
    /* Declared first (key function); overrides slots 16 (D1) and 17 (D0).
       MUST STAY DEFINED INLINE -- all 13 children inline this body, and
       _ZN19dScMgSingle3DBase_cD2Ev exists nowhere in the ROM, so an
       out-of-line definition leaves every child with an undefined external.
       MEASURED on dScMgMemory_c; do not move the body out. */
    virtual ~dScMgSingle3DBase_c() {}

    /* --- re-overrides of dScMgBase_c's virtuals, in _ZTV order. Slots 26
           and 33 are new at this class; their signatures are not
           reconstructed yet, so they stay undeclared. --- */
    virtual void AfterInitResources(u32 vfSuccess); /* slot  2 */
    virtual void AfterCleanupResources(u32 vfSuccess); /* slot  5 */
    virtual int  BeforeBehavior();                  /* slot  7 */
    virtual int  BeforeRender();                    /* slot 10 */

    /* DO NOT restyle the seven `offset 0xNN` comments below into this
     * tree's usual hex-comment form: check_header_offsets.py's DATA_SIZE
     * precompute cannot parse the namespaced mSysTracker line and stops at
     * the last field it CAN parse, which silently undercounts every derived
     * class's field offsets. MEASURED -- see notes/minigame-provenance.md. */
    /* 0x4660..0x471c IS ONE MINIGAME CAMERA, 0xbc bytes, and three readings
     * agree. src/game/camera/Camera_UpdateMatrices.c -- the ov006 routine both
     * dScMgRoulette_c::Render and dScMg3DEsp_c::Render call as
     * Camera_UpdateMatrices(this + 0x4660) -- carries the layout outright:
     * view matrix at +0x00, projection at +0x60, `eye` at +0xa0, `target` at
     * +0xac, `angle` at +0xb8, and it computes the view direction as
     * eye - target. Those three land on 0x4700 / 0x470c / 0x4718 here. And
     * 0x4660 + 0xbc = 0x471c, which is exactly where mSysTracker starts.
     * The head of the object stays a pad: nothing in this family reads a
     * matrix element individually. */
    u8  pad_4660[0xa0];
    s32 mCameraEyeX; /* offset 0x4700 */
    s32 mCameraEyeY; /* offset 0x4704 */
    s32 mCameraEyeZ; /* offset 0x4708 */
    s32 mCameraTargetX; /* offset 0x470c */
    s32 mCameraTargetY; /* offset 0x4710 */
    s32 mCameraTargetZ; /* offset 0x4714 */
    s16 mCameraAngle; /* offset 0x4718 */
    u8  pad_471a[0x2];
    Particle::SysTracker mSysTracker; /* 0x471c */
};

/* A floor, not a claim the object ends here: 0x471c + sizeof(SysTracker).
   See notes/minigame-provenance.md. */
typedef char dScMgSingle3DBase_c_size_must_be_0x4f38[sizeof(dScMgSingle3DBase_c) == 0x4f38 ? 1 : -1];

#endif
