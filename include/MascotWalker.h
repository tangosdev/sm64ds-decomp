#ifndef MASCOTWALKER_H
#define MASCOTWALKER_H

#include "types.h"
#include "SharedFilePtr.h"
#include "dExtBlendAnmModel_c.h"
#include "dExtShadowModel_c.h"

/* The walker's file handles. Each constructor takes the file's ID and
 * each destructor drops the handle; the names are coined.
 * src/minigames/unit020bfec0.cpp defines them inline over the arm9 veneers
 * the ROM calls (func_02017acc / func_02017ab4 for the model,
 * SharedFilePtr::Construct / SharedFilePtr_Destruct_Anim for the
 * animations). */
struct MascotWalkerModelFilePtr : SharedFilePtr {
    MascotWalkerModelFilePtr(u32 fileID);
    ~MascotWalkerModelFilePtr();
};

struct MascotWalkerAnimationFileHandle : SharedFilePtr {
    MascotWalkerAnimationFileHandle(u32 fileID);
    ~MascotWalkerAnimationFileHandle();
};

/* MascotWalker, the small figure that walks the card table's paths in the card
 * games (the dealer embeds one at +0xdc, which fixes the 0xf8 size): a Toad,
 * whose files resolve (tools/asset_catalog.py) to MG/kino_d.bmd, kino_wait.bca
 * and kino_walk.bca. The name is coined. The constructor and destructor are
 * the class's own (ov006 0x020c0a48 and 0x020c09f8, in
 * src/minigames/unit020bfec0.cpp), so Dealer's own constructor and destructor
 * build and tear it down as a member. The other MascotWalker_* helpers there
 * are free functions over a char* record and read it through their WALKER()
 * cast. Unnamed regions are touched in one place at most, or sit inside an
 * embedded object. */
struct MascotWalker {
    MascotWalker();
    ~MascotWalker();

    MascotWalkerModelFilePtr mModelFile; /* 0x00 -- kino_d.bmd (0x205) */
    /* kino_wait.bca (0x206): its file is the BCA Load starts and StartPause restarts. */
    MascotWalkerAnimationFileHandle mPauseAnimFile; /* 0x08 */
    /* kino_walk.bca (0x207): StartNextPath restarts it. */
    MascotWalkerAnimationFileHandle mWalkAnimFile; /* 0x10 */
    /* 0x70 bytes. UpdateMatrix sets its mat4x3 and turns bone 6 of its
       data.transforms by mHeadAngle; StateWalk pauses when its dExtFrameCtrl_c
       base hits frame 0. */
    dExtBlendAnmModel_c mModel; /* 0x18 */
    /* InitCylinder'd by Load and InitModel'd on mModel.mat4x3 by Draw. */
    dExtShadowModel_c mShadow; /* 0x88 */
    s32 mPath;        /* 0xb0 -- the path walked: a point count, then the points */
    s32 mState[2];    /* 0xb4 -- the current state's member-pointer record */
    /* Load sets all three to 0x1000 (1.0), beside mPos and the angles; nothing
       in this file reads it back. */
    Vector3 mScale;   /* 0xbc */
    Vector3 mPos;     /* 0xc8 */
    Vector3 mDir;     /* 0xd4 -- unit vector toward the next point */
    s32 mPathPoint;   /* 0xe0 -- the point being walked to */
    s32 mPathIndex;   /* 0xe4 -- which of the four paths, cycling */
    /* Load copies data_020a0edc's x/y/z into 0xe8/0xea/0xec, the same start
       rotation Dealer_Load copies into Dealer::mRot. */
    s16 mAngleX;      /* 0xe8 */
    s16 mAngleY;      /* 0xea -- eased toward mTargetAngle */
    s16 mAngleZ;      /* 0xec */
    s16 mTargetAngle; /* 0xee -- horizontal angle to the next point */
    /* Applied as an X rotation to bone 6 by UpdateMatrix; Update eases it
       toward sMascotWalkerOffset (clamped to +/-0x3000) while mPaused, else to 0. */
    s16 mHeadAngle; /* 0xf0 */
    /* Counted down by StateWalk and StatePause: 0x78 frames of pause from
       StartPause, then 0x258..0x4af frames of walking from StatePause. */
    s16 mStateTimer; /* 0xf2 */
    s32 mPaused;     /* 0xf4 -- set by StartPause, cleared when StatePause resumes the walk */
};

#ifndef SM64DS_PLATFORM_PC
typedef char
    MascotWalkerModelFilePtr_size_must_be_0x8[sizeof(MascotWalkerModelFilePtr) == 0x8 ? 1 : -1];
typedef char MascotWalkerAnimationFileHandle_size_must_be_0x8
    [sizeof(MascotWalkerAnimationFileHandle) == 0x8 ? 1 : -1];
typedef char MascotWalker_size_must_be_0xf8[sizeof(MascotWalker) == 0xf8 ? 1 : -1];
#endif

#endif
