#ifndef ROTATINGUPDOWNPLATFORM_H
#define ROTATINGUPDOWNPLATFORM_H

#include "types.h"

/* A path-driven moving platform.  The ROM's RTTI calls this class
 * daLinelift2_c, while the imported method symbols call it
 * RotatingUpDownPlatform.  Both names point at the same vtable address in
 * ov091 (0x02134ec0), so the latter remains the compiler-facing compatibility
 * spelling until the function symbols are renamed together.
 *
 * RTTI proves single inheritance from dBgActor_c at offset zero.  The table is
 * exactly 32 slots, the same size as dBgActor_c's, and replaces only the four
 * fBase_c lifecycle slots plus D1/D0.  It adds no virtual function.
 *
 * The object size is independently fixed at 0x358 by ArrowPathLift_Spawn's
 * allocation.  That factory constructs PathPtr at +0x344.  The function
 * currently named RotatingUpDownPlatform_Spawn is not this constructor: its
 * bytes allocate 0x3a8 and construct RotatingUpDownPlatformUtm instead.
 *
 * SM64DS proves this class as daObjRotateUpdownLift_c through RTTI,
 * allocation size and vtable identity. The factory and profile spellings
 * below are reconstructed source-style names -- evidence-bounded proposals,
 * not recovered SM64DS symbols.
 *
 * daObjRotateUpdownLift_c_classInit_UPDOWN_LIFT at 0x02131bdc (historical
 * alias RotatingUpDownPlatform_Spawn) allocates 0x3a8 and installs this
 * class's cartridge vtable. It backs the UPDOWN_LIFT registry profile, whose
 * descriptor at 0x02134bf8 is reconstructed as g_profile_UPDOWN_LIFT.
 */

#ifdef __cplusplus

#include "dBgActor_c.h"
#include "PathPtr.h"

struct RotatingUpDownPlatform : dBgActor_c {
    s32 mState;                       /* 0x320 */
    s32 mNodeCount;                   /* 0x324 */
    s32 mNodeIndex;                   /* 0x328 */
    s32 mBasePosX;                    /* 0x32c */
    s32 mBasePosY;                    /* 0x330 */
    s32 mBasePosZ;                    /* 0x334 */
    s32 mTargetPosX;                  /* 0x338 */
    s32 mTargetPosY;                  /* 0x33c */
    s32 mTargetPosZ;                  /* 0x340 */
    PathPtr mPathPtr;                 /* 0x344 */
    s32 mSinkOffsetY;                 /* 0x34c */
    s16 mBaseAngleY;                  /* 0x350 */
    u8  mVariant;                     /* 0x352 */
    u8  pad_353;
    u16 mStateTimer;                  /* 0x354 */
    u8  mIsPressed;                   /* 0x356 */
    u8  pad_357;

    /* --- vtable --- */
    virtual ~RotatingUpDownPlatform();

    /* Overrides of fBase_c slots 0, 3, 6 and 9.  Virtualness is inherited;
       leaving the keyword off keeps the destructor as this class's first
       declared virtual/key function. */
    int InitResources();
    int CleanupResources();
    int Behavior();
    int Render();
};

typedef char RotatingUpDownPlatform_size_must_be_0x358[
    sizeof(RotatingUpDownPlatform) == 0x358 ? 1 : -1];

#else

/* Flat C spelling retained for any legacy consumers. */
#include "Model.h"
#include "dBgW_KcMbg.h"

struct RotatingUpDownPlatform {
    u8  pad_000[0x8];
    u32 mParam;                       /* 0x008 */
    u16 actorID;                      /* 0x00c */
    u8  aliveState;                   /* 0x00e */
    u8  shouldBeKilled;               /* 0x00f */
    u8  unk_010;                      /* 0x010 */
    u8  unk_011;                      /* 0x011 */
    u8  unk_012;                      /* 0x012 */
    u8  pauseFlags;                   /* 0x013 */
    u8  sceneNode[0x14];              /* 0x014 */
    u8  behavNode[0x10];              /* 0x028 */
    u8  renderNode[0x10];             /* 0x038 */
    u8  pad_048[0x14];
    s32 mPosX;                        /* 0x05c */
    s32 mPosY;                        /* 0x060 */
    s32 mPosZ;                        /* 0x064 */
    u8  pad_068[0x26];
    s16 mAngleY;                      /* 0x08e */
    u8  pad_090[0x44];
    Model mModel;                     /* 0x0d4 */
    dBgW_KcMbg mMeshCollider;         /* 0x124 */
    u8  pad_2ec[0x34];
    s32 mState;                       /* 0x320 */
    s32 mNodeCount;                   /* 0x324 */
    s32 mNodeIndex;                   /* 0x328 */
    s32 mBasePosX;                    /* 0x32c */
    s32 mBasePosY;                    /* 0x330 */
    s32 mBasePosZ;                    /* 0x334 */
    s32 mTargetPosX;                  /* 0x338 */
    s32 mTargetPosY;                  /* 0x33c */
    s32 mTargetPosZ;                  /* 0x340 */
    u8  mPathPtr[0x8];                /* 0x344 */
    s32 mSinkOffsetY;                 /* 0x34c */
    s16 mBaseAngleY;                  /* 0x350 */
    u8  mVariant;                     /* 0x352 */
    u8  pad_353;
    u16 mStateTimer;                  /* 0x354 */
    u8  mIsPressed;                   /* 0x356 */
    u8  pad_357;
};

typedef char RotatingUpDownPlatform_size_must_be_0x358[
    sizeof(struct RotatingUpDownPlatform) == 0x358 ? 1 : -1];

#endif /* __cplusplus */

#endif /* ROTATINGUPDOWNPLATFORM_H */
