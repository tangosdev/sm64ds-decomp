#ifndef FLOWERFACE_H
#define FLOWERFACE_H

#include "types.h"
#include "SharedFilePtr.h"
#include "dExtAnmModel_c.h"

/* FlowerFace, Yoshi in the "loves me, loves me not" flower game, and the 22
 * petals that fall off the flower, animated by the FlowerFace_* /
 * FallingPetal_* helpers in src/minigames/unit020bfec0.cpp. Both names are
 * coined. The files resolve (tools/asset_catalog.py) to MG/yoshi_model.bmd,
 * the petal MG/f_parts.bmd, and MG/yoshi_wait, yoshi_pick, yoshi_like and
 * yoshi_hate.bca. The constructors and destructors are the classes' own
 * (FlowerFace: ov006 0x020c3f54 and 0x020c3e70; FallingPetal: 0x020c402c and
 * 0x020c3e54), so the flower scene holds a typed FlowerFace member and the
 * compiler builds the petal array. The other helpers are free functions
 * over char* records and read them through their FLOWERFACE() and PETAL()
 * casts. */
struct FallingPetal {
    FallingPetal();
    ~FallingPetal();

    Vector3 mPos;    /* 0x00 */
    Vector3 mVel;    /* 0x0c -- mPos += mVel each frame */
    Vector3 mAccel;  /* 0x18 -- y is -0x40: the fall */
    Vector3 mAxis;   /* 0x24 -- the petal's up axis, tipped toward its motion */
    s32 mRot[4];     /* 0x30 -- a quaternion */
    s32 mActive;     /* 0x40 */
    dExtSimpleModel_c mModel; /* 0x44 -- 0x50 bytes: UpdateMatrix sets its mat4x3 */
    s16 mLife;       /* 0x94 -- frames left; 0 clears mActive */
    u8 pad_96[0x2];  /* 0x96 */
};

/* Yoshi's file handles. Each constructor takes the file's ID and each
 * destructor drops the handle; the names are coined.
 * src/minigames/unit020bfec0.cpp defines them inline over the arm9 veneers
 * the ROM calls (func_02017acc / func_02017ab4 for the two models,
 * SharedFilePtr::Construct / SharedFilePtr_Destruct_Anim for the
 * animations). */
struct FlowerFaceModelFilePtr : SharedFilePtr {
    FlowerFaceModelFilePtr(u32 fileID);
    ~FlowerFaceModelFilePtr();
};

struct FlowerFaceAnimationFileHandle : SharedFilePtr {
    FlowerFaceAnimationFileHandle(u32 fileID);
    ~FlowerFaceAnimationFileHandle();
};

struct FlowerFace {
    FlowerFace();
    ~FlowerFace();

    s32 mState[2];             /* 0x000 -- the current state's member-pointer record */
    FallingPetal mPetals[22];  /* 0x008 */
    /* 0x64 bytes. Draw sets its mat4x3; Update advances its dExtFrameCtrl_c
       base, StatePick tests it and PlayPetalPulled rewinds its currFrame. */
    dExtAnmModel_c mModel;     /* 0xd18 */
    FlowerFaceModelFilePtr mModelFile;       /* 0xd7c -- yoshi_model.bmd (0x222) */
    FlowerFaceModelFilePtr mPetalFile;       /* 0xd84 -- f_parts.bmd (0x201) */
    FlowerFaceAnimationFileHandle mWaitAnim; /* 0xd8c -- yoshi_wait.bca (0x224) */
    /* Pick plays as a petal comes off; Like and Hate are the "loves me" and
       "loves me not" endings. */
    FlowerFaceAnimationFileHandle mPickAnim; /* 0xd94 -- yoshi_pick.bca (0x223) */
    FlowerFaceAnimationFileHandle mLikeAnim; /* 0xd9c -- yoshi_like.bca (0x221) */
    FlowerFaceAnimationFileHandle mHateAnim; /* 0xda4 -- yoshi_hate.bca (0x220) */
};

#ifndef SM64DS_PLATFORM_PC
typedef char
    FlowerFaceModelFilePtr_size_must_be_0x8[sizeof(FlowerFaceModelFilePtr) == 0x8 ? 1 : -1];
typedef char FlowerFaceAnimationFileHandle_size_must_be_0x8
    [sizeof(FlowerFaceAnimationFileHandle) == 0x8 ? 1 : -1];
typedef char FallingPetal_size_must_be_0x98[sizeof(FallingPetal) == 0x98 ? 1 : -1];
typedef char FlowerFace_size_must_be_0xdac[sizeof(FlowerFace) == 0xdac ? 1 : -1];
#endif

#endif
