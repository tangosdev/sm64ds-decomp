//cpp
/* Princess Peach in the castle courtyard (PEACH_PRINCESS). Five state
 * pairs. SetState selects one, and each state writes 0..4 into mStateValue.
 * defer_codegen off: D1 then D0, no D2. Source follows the ROM.
 */
// NOTES:
// - Offsets 0xa4 and 0xac are dActor_c's mSpeedX and mSpeedZ. Offset 0xe8
//   sits inside the model's ModelComponents and is still read through words[].
// - Vec3_VertAngle and AngleDiff have no header declaration, so they stay
//   local.
#include "common.h"
#include "nitro/fx/fx.h"
#include "daPeach_c.h"
#include "dActor_c.h"
#include "Player.h"
#include "SharedFilePtr.h"
#include "dExtFrameCtrl_c.h"
#include "dCc_c.h"
#include "types.h"
#include "dBgCh_Actr.h"
#include "cstd.h"
#include "math/Approach.h"
#include "math/CalcLib.h"
#include "math/MtxStack.h"
#include "SpawnInfo.h"

struct BMD_File;
struct Vector3_16;

/* dExtAnmModel_c's fourth virtual slot, reached through a four-slot stand-in.
 * Behavior calls it on the dExtAnmModel_c at +0xd4; the real declaration lives
 * with that class, and this file only needs the dispatch. */
struct PeachAnimSlots {
    virtual int g0();
    virtual int g1();
    virtual int g2();
    virtual int g3();
};

/* The twelve-word body of a Matrix4x3, copied whole in and out of the
 * scratch matrix. */
typedef struct {
    int w[12];
} PeachM48;

/* local extern: no header declares these two. */
extern "C" {
int AngleDiff(int, int);
short Vec3_VertAngle(const void *a, const void *b);
}

/* ov085 statics. data_ov085_0212f280 keeps include/decl_common.h's
 * spelling, so this file adds no new reading of it. */
extern int data_ov085_0212f280[]; /* the seven animation files */

/* Runtime asset handles, distinct from the NitroFS file IDs. */
enum daPeach_c_AssetId {
    ASSET_PEACH_JUMP_BCA = 0x03f0,
    ASSET_PEACH_JUMP_END_BCA = 0x03f1,
    ASSET_PEACH_JUMP_LOOP_BCA = 0x03f2,
    ASSET_PEACH_LOW_BMD = 0x03f3,
    ASSET_PEACH_OP_WAIT_BCA = 0x03f4,
    ASSET_PEACH_RUN_BCA = 0x03f6,
    ASSET_PEACH_WAIT_BCA = 0x03f7,
    ASSET_PEACH_WALK_BCA = 0x03f9
};

/* The handles reach their files through these wrappers' constructor and
   destructor veneers; the manifest maps each to the retail destination.
   Descriptive local types, not recovered spellings. */
struct PeachModelFile : SharedFilePtr {
    PeachModelFile(u32 fileID);
    ~PeachModelFile();
};

struct PeachAnimFile : SharedFilePtr {
    PeachAnimFile(u32 fileID);
    ~PeachAnimFile();
};

/* This TU's file handles, in the order the retail initializer (0x0212f3a0)
   builds them. */
PeachModelFile gPeachModelFile(ASSET_PEACH_LOW_BMD);
PeachAnimFile gPeachWaitAnimationFile(ASSET_PEACH_WAIT_BCA);
PeachAnimFile gPeachWalkAnimationFile(ASSET_PEACH_WALK_BCA);
PeachAnimFile gPeachRunAnimationFile(ASSET_PEACH_RUN_BCA);
PeachAnimFile gPeachJumpAnimationFile(ASSET_PEACH_JUMP_BCA);
PeachAnimFile gPeachJumpEndAnimationFile(ASSET_PEACH_JUMP_END_BCA);
PeachAnimFile gPeachJumpLoopAnimationFile(ASSET_PEACH_JUMP_LOOP_BCA);
PeachAnimFile gPeachOpWaitAnimationFile(ASSET_PEACH_OP_WAIT_BCA);

/* SetState indexes this by state. Each record is {init, behavior} as SetState
   and Behavior call it; the member names predate this table and do not
   follow the record order. */
daPeach_c::StateEntry data_ov085_0213055c[5] = {
    {&daPeach_c::State4, &daPeach_c::InitState3}, {&daPeach_c::InitState1, &daPeach_c::State3},
    {&daPeach_c::State1, &daPeach_c::InitState4}, {&daPeach_c::InitState2, &daPeach_c::State2},
    {&daPeach_c::State0, &daPeach_c::InitState0},
};

#pragma defer_codegen off

// @symbol _ZN9daPeach_cD1Ev
// @symbol _ZN9daPeach_cD0Ev
/* ONE declaration, TWO ROM functions. mwccarm emits the complete variant D1
 * and the deleting variant D0 from the single destructor this class declares,
 * so D0 has no source of its own; with deferred code generation off they land
 * in the cartridge's order, D1 first at 0x02129d18 and D0 at 0x02129d60.
 *
 * There is nothing to write. The vptr store and the four member destructor
 * calls -- dBgCh_Actr at 0x194, dCcAc_c at 0x160, dExtShadowModel_c at 0x138,
 * dExtAnmModel_c at 0xd4 -- and then dActor_c's own step are every one a
 * consequence of `struct daPeach_c : dActor_c` and the members that
 * declaration types. D0's deallocation is the inline operator delete reached
 * through fBase_c, which is why nothing here mentions a heap.
 *
 * daPeach_c declares its destructor in the class body. The two functions
 * below demand D1 and D0, in that order: a destructor call, then a
 * delete-expression. Nothing calls either one, so the link drops them.
 * include/daPeach_c.h stays as it is. */
void PeachDemandCompleteDtor(daPeach_c *peach) {
    peach->~daPeach_c();
}

void PeachDemandDeletingDtor(daPeach_c *peach) {
    delete peach;
}

// @symbol _ZN9daPeach_c12UpdateLookAtEv
/* Peach turns her head toward the nearest player, but only when he is close
 * enough (0x15e000) and roughly in front of her (within 0x3000 of her
 * facing). Otherwise both target angles fall back to zero and she looks
 * straight ahead. The two ApproachLinear calls make the turn gradual. */
void daPeach_c::UpdateLookAt() {
    Player *player = ClosestPlayer();
    if (player == 0)
        return;
    Vector3 target;
    Vector3 *playerPos = (Vector3 *)(((long long)(int)&player->mPosX));
    target = *playerPos;
    int dist = Vec3_HorzDist(&mPos, &target);
    target.y = target.y - 0x1e000;
    short yaw = Vec3_HorzAngle(&mPos, &target);
    short pitch = Vec3_VertAngle(&mPosX, &target);
    if (dist < 0x15e000 && AngleDiff(yaw, mAngleY) < 0x3000) {
        mTargetLookVertAngle = pitch;
        mTargetLookHorzAngle = mAngleY - yaw;
    } else {
        mTargetLookVertAngle = 0;
        mTargetLookHorzAngle = 0;
    }
    ApproachLinear(mLookHorzAngle, mTargetLookHorzAngle, 0x250);
    ApproachLinear(mLookVertAngle, mTargetLookVertAngle, 0x100);
}

// @symbol _ZN9daPeach_c21UpdateGroundCollisionEP10dBgCh_Actr
/* The floor normal becomes a pitch at +0xa8 so Peach stands square on a
 * slope. The wall branch reads its normal and drops it -- the ROM computes
 * the copy and uses nothing of it. */
void daPeach_c::UpdateGroundCollision(dBgCh_Actr *clsn) {
    int *words = (int *)this; /* 0xa4 and 0xac are dActor_c's mSpeedX / mSpeedZ */
    int floorN[3];
    int wallN[3];
    dBgCh_Actr_UpdateContinuous_Veneer(clsn);
    if (clsn->IsOnGround()) {
        ((SurfaceInfo *)((char *)clsn->GetFloorResult() + 4))->CopyNormalTo(*(Vector3 *)floorN);
        if (floorN[1] != 0) {
            int x = FX_MUL(floorN[0], words[0xa4 / 4]);
            int z = FX_MUL(floorN[2], words[0xac / 4]);
            mVertSpeed = -(cstd::fdiv(x + z, floorN[1]) + 0x8000);
        }
    }
    if (clsn->IsOnWall()) {
        ((SurfaceInfo *)((char *)clsn->GetWallResult() + 4))->CopyNormalTo(*(Vector3 *)wallN);
    }
}

// @symbol _ZN9daPeach_c16IsTouchingPlayerEv
/* Whether the actor whose id sits in mCylinder.otherOwner still exists and
 * is a player (actorID 0xbf). InitState0 and InitState4 call it and ignore
 * the answer. */
int daPeach_c::IsTouchingPlayer() {
    unsigned int id = mCylinder.otherOwner;
    void *actor;
    unsigned short kind;
    int alive;
    if (id == 0)
        return id;
    actor = dActor_c::FindWithID(id);
    if (actor == 0)
        return (int)actor;
    kind = ((dActor_c *)actor)->actorID;
    if (kind == 0xbf)
        alive = 1;
    else
        alive = 0;
    /* Both paths return alive. The cartridge still compares it with zero
     * first; a single return lets the compiler delete that compare. */
    if (alive == 0)
        goto done;
    return alive;
done:
    return alive;
}

// @symbol _ZN9daPeach_c11UpdateModelEv
/* The model matrix at +0xf0 is rebuilt from the body angle and the position,
 * then the head joint's own matrix (+0x360 inside the animated model) is
 * rotated by the two look angles through the scratch matrix at
 * data_020a0e68. */
void daPeach_c::UpdateModel() {
    char *c = (char *)this;
    Matrix4x3_FromRotationY(&mAnmModel.mat4x3, mAngleY);
    mAnmModel.mat4x3.m[9] = mPosX >> 3;
    mAnmModel.mat4x3.m[10] = mPosY >> 3;
    mAnmModel.mat4x3.m[11] = mPosZ >> 3;
    *(PeachM48 *)&data_020a0e68 = *(PeachM48 *)(*(char **)(c + 0xe8) + 0x360);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, mLookHorzAngle);
    Matrix4x3_ApplyInPlaceToRotationZ(&data_020a0e68, mLookVertAngle);
    *(PeachM48 *)(*(char **)(c + 0xe8) + 0x360) = *(PeachM48 *)&data_020a0e68;
    DropShadowRadHeight(mShadowModel, mAnmModel.mat4x3, 0x8c000, 0x32000, 0xf);
}

// @symbol _ZN9daPeach_c10InitState0Ev
int daPeach_c::InitState0() {
    UpdatePos((dCc_c *)&mCylinder);
    UpdateGroundCollision(&mWithMeshClsn);
    IsTouchingPlayer();
    return 1;
}

// @symbol _ZN9daPeach_c6State0Ev
int daPeach_c::State0() {
    mAnmModel.SetAnim((BCA_File *)gPeachJumpAnimationFile.filePtr, 0, 0x1000, 0);
    ((dExtFrameCtrl_c *)&mAnmModel)->SetFlags(0x40000000);
    mHorzSpeed = 0x4000;
    mVertSpeed = 0xa000;
    mStateValue = 4;
    return 1;
}

// @symbol _ZN9daPeach_c6State2Ev
int daPeach_c::State2() {
    return 1;
}

// @symbol _ZN9daPeach_c10InitState2Ev
int daPeach_c::InitState2() {
    mAnmModel.SetAnim((BCA_File *)gPeachRunAnimationFile.filePtr, 0, 0x1000, 0);
    mHorzSpeed = 0x4000;
    mStateValue = 3;
    return 1;
}

// @symbol _ZN9daPeach_c10InitState4Ev
int daPeach_c::InitState4() {
    short angY = mAngleY;
    mPrevAngleY = angY;
    UpdatePos((dCc_c *)&mCylinder);
    UpdateGroundCollision(&mWithMeshClsn);
    IsTouchingPlayer();
    return 1;
}

// @symbol _ZN9daPeach_c6State1Ev
int daPeach_c::State1() {
    mAnmModel.SetAnim((BCA_File *)gPeachWalkAnimationFile.filePtr, 0, 0x1000, 0);
    mHorzSpeed = 0x1800;
    mStateValue = 2;
    return 1;
}

// @symbol _ZN9daPeach_c6State3Ev
/* The conversation, in three steps held in mTalkState: start the talk, turn
 * to face the player and put the message up, then wait for the player's talk
 * state to go back to -1. */
int daPeach_c::State3() {
    Vector3 msgPos;
    switch (mTalkState) {
    case 0:
        if (mTalkPlayer->StartTalk(*this, 1) != 0)
            mTalkState += 1;
        break;
    case 1:
        if (ApproachLinear(mAngleY, Vec3_HorzAngle(&mPos, &mTalkPlayer->mPos), 0x514) != 0) {
            msgPos.x = mPosX;
            msgPos.y = mPosY;
            msgPos.z = mPosZ;
            msgPos.y = msgPos.y + 0xa0000;
            if (mTalkPlayer->ShowMessage(*this, 0xd0, &msgPos, 0, 0) != 0)
                mTalkState += 1;
        }
        break;
    case 2:
        if (mTalkPlayer->GetTalkState() == -1)
            SetState(0);
        break;
    }
    return 1;
}

// @symbol _ZN9daPeach_c10InitState1Ev
int daPeach_c::InitState1() {
    mAnmModel.SetAnim((BCA_File *)gPeachWaitAnimationFile.filePtr, 0, 0x1000, 0);
    ((dExtFrameCtrl_c *)&mAnmModel)->currFrame = 0;
    ((dExtFrameCtrl_c *)&mAnmModel)->Advance();
    mTalkState = 1;
    mStateValue = 1;
    return 1;
}

// @symbol _ZN9daPeach_c10InitState3Ev
/* The same liveness question IsTouchingPlayer asks, written out here
 * instead of called -- the ROM inlines it at this one site. */
int daPeach_c::InitState3() {
    if (mCylinder.hitFlags & 0x8000000) {
        char *a = (char *)dActor_c::FindWithID(mCylinder.otherOwner);
        if (a) {
            int match = (((dActor_c *)a)->actorID == 0xbf) ? 1 : 0;
            if (match != 0) {
                mTalkPlayer = (Player *)a;
                if (mTalkPlayer->StartTalk(*this, false)) {
                    SetState(1);
                }
            }
        }
    }
    return 1;
}

// @symbol _ZN9daPeach_c6State4Ev
int daPeach_c::State4() {
    mAnmModel.SetAnim((BCA_File *)gPeachOpWaitAnimationFile.filePtr, 0, 0x1000, 0);
    mStateValue = 0;
    return 1;
}

// @symbol _ZN9daPeach_c17CallStateBehaviorEv
/* The behavior half of the selected row. The table rows are pairs of
 * pointers-to-member copied out of the ten ROM constants at 0x0212ff34. */
void daPeach_c::CallStateBehavior() {
    (this->*mState->behavior)();
}

// @symbol _ZN9daPeach_c13CallStateInitEv
void daPeach_c::CallStateInit() {
    (this->*mState->init)();
}

// @symbol _ZN9daPeach_c8SetStateEi
void daPeach_c::SetState(int state) {
    mState = &data_ov085_0213055c[state];
    CallStateInit();
}

// @symbol _ZN9daPeach_c16CleanupResourcesEv
/* One direct release, then a LOOP over a seven-entry table of pointers.
 * Unlike every sibling in this overlay, which writes its releases out one per
 * line, Peach's animation files are reached through data_ov085_0212f280 and
 * freed by index. Seven is the count the ROM's `blt` tests against. */
int daPeach_c::CleanupResources() {
    gPeachModelFile.Release();
    s32 i = 0;
    do {
        ((SharedFilePtr *)data_ov085_0212f280[i])->Release();
        i++;
    } while (i < 7);
    return 1;
}

// @symbol _ZN9daPeach_c16OnPendingDestroyEv
/* Empty -- the ROM body is a single `bx lr`. The override exists to suppress
 * whatever the base does on pending destroy, not to do anything itself. */
void daPeach_c::OnPendingDestroy() {}

// @symbol _ZN9daPeach_c6RenderEv
/* Qualified dExtSimpleModel_c::Render. dExtAnmModel_c overrides the slot. */
int daPeach_c::Render() {
    mAnmModel.dExtSimpleModel_c::Render(0);
    return 1;
}

// @symbol _ZN9daPeach_c8BehaviorEv
int daPeach_c::Behavior() {
    CallStateBehavior();
    UpdateLookAt();
    if (mStateValue != 1)
        ((dExtFrameCtrl_c *)((dExtFrameCtrl_c *)&mAnmModel))->Advance();
    ((PeachAnimSlots *)((char *)&mAnmModel))->g3();
    ((dCc_c *)&mCylinder)->Clear();
    ((dCc_c *)&mCylinder)->Update();
    UpdateModel();
    return 1;
}

// @symbol _ZN9daPeach_c13InitResourcesEv
/* dBgCh_Actr::Init's last parameter is a Vector3_16* as well -- the S5_ in
 * ...P10Vector3_16S5_ back-references the pointer type, it is not an int. */
int daPeach_c::InitResources() {
    void *f = dExtSimpleModel_c::LoadFile(gPeachModelFile);
    ((dExtModel_c *)&mAnmModel)->SetFile((BMD_File *)f, 1, -1);
    for (int i = 0; i < 7; i++)
        dExtFrameCtrl_c::LoadFile(*(SharedFilePtr *)data_ov085_0212f280[i]);
    if (mShadowModel.InitCylinder() == 0)
        return 0;
    mCylinder.Init(this, 0x90000, 0xc0000, 0x4800004, 0);
    mWithMeshClsn.Init(this, 0x40000, 0x40000, (Vector3_16 *)0, (Vector3_16 *)0);
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    SetState(0);
    UpdateModel();
    return 1;
}

// @symbol daPeach_c_classInit
/* PEACH_PRINCESS factory. */
extern "C" daPeach_c *daPeach_c_classInit(void) {
    return new daPeach_c();
}

/* The actor profile, in SpawnInfo's layout (include/SpawnInfo.h). The
 * retail values are unchanged. */
SpawnInfo<daPeach_c> g_profile_PEACH_PRINCESS = {
    daPeach_c_classInit, /* classInit */
    0x00ba,              /* behaviorPriority */
    0x00ac,              /* renderPriority */
    0x00000003,          /* actorFlags */
    0x64000,             /* clipOffsetY */
    0x64000,             /* clipRadius */
    0x01000000,          /* clipDistance */
    0x01000000           /* farDistance */
};
