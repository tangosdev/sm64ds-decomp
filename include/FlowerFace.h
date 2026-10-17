#ifndef FLOWERFACE_H
#define FLOWERFACE_H

#include "types.h"

/* FlowerFace, Yoshi in the "loves me, loves me not" flower game, and the 22
 * petals that fall off the flower: the records FlowerFace_Construct builds and
 * the FlowerFace_* / FallingPetal_* helpers in src/minigames/unit020bfec0.cpp
 * animate. Both names are coined. The files resolve (tools/asset_catalog.py)
 * to MG/yoshi_model.bmd, the petal MG/f_parts.bmd, and MG/yoshi_wait,
 * yoshi_pick, yoshi_like and yoshi_hate.bca. Layout views only: the helpers
 * are free functions over char* records and read them through their
 * FLOWERFACE() and PETAL() casts. */
struct FallingPetal {
    Vector3 mPos;    /* 0x00 */
    Vector3 mVel;    /* 0x0c -- mPos += mVel each frame */
    Vector3 mAccel;  /* 0x18 -- y is -0x40: the fall */
    Vector3 mAxis;   /* 0x24 -- the petal's up axis, tipped toward its motion */
    s32 mRot[4];     /* 0x30 -- a quaternion */
    s32 mActive;     /* 0x40 */
    u8 mModel[0x50]; /* 0x44 -- a Model */
    s16 mLife;       /* 0x94 -- frames left; 0 clears mActive */
    u8 pad_96[0x2];  /* 0x96 */
};

struct FlowerFaceFile {
    s32 handle; /* +0x0 */
    s32 file;   /* +0x4 -- the loaded file, 0 until loaded */
};

struct FlowerFace {
    s32 mState[2];             /* 0x000 -- the current state's member-pointer record */
    FallingPetal mPetals[22];  /* 0x008 */
    u8 mModel[0x64];           /* 0xd18 -- a ModelAnim */
    FlowerFaceFile mModelFile; /* 0xd7c -- yoshi_model.bmd */
    FlowerFaceFile mPetalFile; /* 0xd84 -- f_parts.bmd */
    FlowerFaceFile mWaitAnim;  /* 0xd8c -- yoshi_wait.bca */
    FlowerFaceFile mPickAnim;  /* 0xd94 -- yoshi_pick.bca, as a petal comes off */
    FlowerFaceFile mLikeAnim;  /* 0xd9c -- yoshi_like.bca, the "loves me" ending */
    FlowerFaceFile mHateAnim;  /* 0xda4 -- yoshi_hate.bca, the "loves me not" ending */
};

#ifndef SM64DS_PLATFORM_PC
typedef char FlowerFaceFile_size_must_be_0x8[sizeof(FlowerFaceFile) == 0x8 ? 1 : -1];
typedef char FallingPetal_size_must_be_0x98[sizeof(FallingPetal) == 0x98 ? 1 : -1];
typedef char FlowerFace_size_must_be_0xdac[sizeof(FlowerFace) == 0xdac ? 1 : -1];
#endif

#endif
