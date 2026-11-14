//cpp
/**
 * d_a_holhei.cpp
 * Object - Chuckya (HOLHEI 190)
 *
 * ov062/daHolhei_c. Source order is the reverse of the ROM: mwccarm emits
 * one .text section per function backwards. The destructor stays inline in
 * include/daHolhei_c.h so the cartridge gets D1 then D0 and no D2.
 * common.h is first so Matrix4x3 is the flat s32 m[12]; the nested
 * math/Matrix.h spelling scalarizes a twelve-word copy.
 *
 * The file handles, the carry offsets, the ten state records and
 * g_profile_HOLHEI are this TU's statics; mwcc emits their initializer,
 * __sinit_d_a_holhei.cpp (0x0211cf30). Each file handle's filePtr is the
 * loaded BMD or BCA. The records keep their data_ov062_* address names, as
 * their handlers keep func_ov062_*.
 *
 * Known limits, measured in this TU:
 * - UpdateHeldShadow reloads mHeld before each pos word.
 *   func_ov062_02116a08 copies the player's position through one int
 *   pointer; three separate pl->mPos loads change the code.
 * - BuildHoldMatrix hands &mHoldMtx out through this object's
 *   mCarryMatrix; a grabbed actor's mCarryMatrix gates the hold path.
 *
 * Naming: helpers keep func_ov062_* names; daHolhei_c_ChangeState is
 * coined. The state handlers are members; the remaining extern "C"
 * helpers take the actor as daHolhei_c *self.
 */

#include "common.h"
#include "daHolhei_c.h"
#include "SharedFilePtr.h"
#include "daPly_c.h"
#include "dCamera_c.h"
#include "SurfaceInfo.h"
#include "dBgPi.h"
#include "Sound.h"
#include "math/Approach.h"
#include "math/CalcLib.h"
#include "SpawnInfo.h"
#include "LevelRng.h"
#include "math/MtxStack.h"

/* A state record: daHolhei_c_ChangeState stores its address in mState
   and calls a non-null enter; past the UpdateKillByInvincibleChar check,
   Behavior calls a non-null update. Every handler returns 1. */
typedef int (daHolhei_c::*HolheiStateFn)();
struct HolheiState {
    HolheiStateFn enter;  /* 0x0 */
    HolheiStateFn update; /* 0x8 */
};

extern void func_02012694(unsigned int a, const Vector3 *p);
int daHolhei_c_ChangeState(daHolhei_c *self, HolheiState *p);
extern "C" int func_ov002_020db5f4(daPly_c *c, dActor_c *arg);
extern "C" void func_ov002_020db54c(void *self, int a, int b, short c);
extern int data_02092138;
extern dCamera_c *gCamera;
/* local extern: named parameters preserve @NNN */
extern "C" int ApproachAngle(short *angle, short target, int a, int b, int c);
/* local extern: named parameters preserve @NNN */
extern "C" short Vec3_HorzAngle(const Vector3 *v0, const Vector3 *v1);
/* local extern: named parameters preserve @NNN */
extern "C" int Vec3_Dist(const Vector3 *a, const Vector3 *b);
extern Matrix4x3 IDENTITY_MATRIX4X3;
extern "C" int Math_Function_0203b14c(void *, int, int, int, int);
/* local extern: named parameters preserve @NNN */
extern "C" unsigned short DecIfAbove0_Short(unsigned short *p);
extern signed char gCurrentSublevel;
extern "C" int AngleDiff(int a, int b);

/* Resource handles owned by this TU. The static initializer constructs the
   model (id 0x327) and five animations (ids 0x328..0x32c) and registers each
   destructor. The wrappers' constructor and destructor resolve to the retail
   veneers through the manifest. Names follow the runtime asset paths; they
   are reconstructed identifiers. */
struct HolheiModelFile : SharedFilePtr {
    HolheiModelFile(u32);
    ~HolheiModelFile();
};
struct HolheiAnimFile : SharedFilePtr {
    HolheiAnimFile(u32);
    ~HolheiAnimFile();
};

/* Runtime asset handles resolved through the overlay 0 file table. */
enum HolheiAssetId {
    ASSET_HORUHEI_BMD = 0x0327,
    ASSET_HORUHEI_LIFT_BCA = 0x0328,
    ASSET_HORUHEI_LIFT_LOOP_BCA = 0x0329,
    ASSET_HORUHEI_STRUGGLE_BCA = 0x032a,
    ASSET_HORUHEI_THROW_BCA = 0x032b,
    ASSET_HORUHEI_WALK_BCA = 0x032c
};

/* This TU's statics, in the order the retail initializer (0x0211cf30)
   builds them. */
HolheiModelFile gHoruheiModelFile(ASSET_HORUHEI_BMD);
HolheiAnimFile gHoruheiLiftAnimationFile(ASSET_HORUHEI_LIFT_BCA);
HolheiAnimFile gHoruheiLiftLoopAnimationFile(ASSET_HORUHEI_LIFT_LOOP_BCA);
HolheiAnimFile gHoruheiStruggleAnimationFile(ASSET_HORUHEI_STRUGGLE_BCA);
HolheiAnimFile gHoruheiThrowAnimationFile(ASSET_HORUHEI_THROW_BCA);
HolheiAnimFile gHoruheiWalkAnimationFile(ASSET_HORUHEI_WALK_BCA);

/* Where a carried actor sits, indexed by its param1 == 2 in
   HomeCarryOffset. */
Vector3 data_ov062_0211df10[2] = {
    Vector3(0x2c000, -0x14000, -0x78000),
    Vector3(0x44000, -0x14000, -0x78000),
};

/* State records: {enter, update}. The handlers keep their address names,
   so the records keep theirs. */
HolheiState data_ov062_0211ded0 = {&daHolhei_c::func_ov062_02116c78,
                                   &daHolhei_c::func_ov062_02116bf8};
HolheiState data_ov062_0211dee0 = {&daHolhei_c::func_ov062_02116b80,
                                   &daHolhei_c::func_ov062_02116a08};
HolheiState data_ov062_0211df00 = {&daHolhei_c::func_ov062_02116980,
                                   &daHolhei_c::func_ov062_02116894};
HolheiState data_ov062_0211de70 = {&daHolhei_c::func_ov062_02116850,
                                   &daHolhei_c::func_ov062_021167c0};
HolheiState data_ov062_0211de90 = {&daHolhei_c::func_ov062_02116784,
                                   &daHolhei_c::func_ov062_021165e8};
HolheiState data_ov062_0211dea0 = {&daHolhei_c::func_ov062_021165e0,
                                   &daHolhei_c::func_ov062_021164e8};
HolheiState data_ov062_0211dec0 = {&daHolhei_c::func_ov062_02116498,
                                   &daHolhei_c::func_ov062_021163b0};
HolheiState data_ov062_0211def0 = {&daHolhei_c::func_ov062_02116368,
                                   &daHolhei_c::func_ov062_0211632c};
HolheiState data_ov062_0211de80 = {&daHolhei_c::func_ov062_021162b8,
                                   &daHolhei_c::func_ov062_02116274};
HolheiState data_ov062_0211deb0 = {&daHolhei_c::func_ov062_02116238,
                                   &daHolhei_c::func_ov062_021161a8};

/* Actor ID 0xbf is PLAYER. The mFlags bits this file tests (0x100, 0x400,
   0x2000, 0x4000) are undocumented, so they go through IsFlagSet. Both are
   int-returning: that is what makes the ROM materialize the 0/1 before
   testing it. */
static inline int IsPlayerActor(dActor_c *actor) {
    return actor->actorID == 0xbf;
}

static inline int IsFlagSet(dActor_c *actor, u32 bit) {
    return (actor->mFlags & bit) != 0;
}

/* The cartridge's body is the one `new`: 0x438 into fBase_c::operator new,
   dEnemyBase_c's C2, this TU's vtable, then the member C1s. */
daHolhei_c *daHolhei_c_classInit(void) {
    return new daHolhei_c();
}

/* The actor profile, in SpawnInfo's layout (include/SpawnInfo.h): the
   factory, behavior and render priorities (the behavior priority equals the
   HOLHEI registry id, 190), actor flags, then the clip offset Y, clip radius,
   clip distance and far distance passed to SetRanges. The retail values are
   unchanged. */
SpawnInfo<daHolhei_c> g_profile_HOLHEI = {
    daHolhei_c_classInit, 0x00be, 0x0064, 0x10000003, 0xc8000, 0x320000, 0x01000000, 0x01000000};

s32 daHolhei_c::OnAimedAtWithEgg() {
    return 0xca000;
}

/* One model and five animations, the shadow, both collision volumes,
   home and previous positions, then the starting state. */
int daHolhei_c::InitResources()
{
    mModel.SetFile(dExtSimpleModel_c::LoadFile(gHoruheiModelFile), 1, -1);
    mShadowModel.InitCylinder();
    dExtFrameCtrl_c::LoadFile(gHoruheiLiftAnimationFile);
    dExtFrameCtrl_c::LoadFile(gHoruheiLiftLoopAnimationFile);
    dExtFrameCtrl_c::LoadFile(gHoruheiStruggleAnimationFile);
    dExtFrameCtrl_c::LoadFile(gHoruheiThrowAnimationFile);
    dExtFrameCtrl_c::LoadFile(gHoruheiWalkAnimationFile);
    mVertAccel = -0x3000;
    mTerminalVelocity = -0x1e000;
    mdCc_c.Init(this, 0xc8000, 0xfa000, 0x200004, 0x3010);
    mModel.speed = 0x1000;
    mMeshClsn.Init(this, 0x118000, 0x118000, 0, 0);
    mHomePosX = mPosX;
    mHomePosY = mPosY;
    mHomePosZ = mPosZ;
    mCoinType = 1;
    mExtraCoinCount = 4;
    mPrevPosX = mPosX;
    mPrevPosY = mPosY;
    mPrevPosZ = mPosZ;
    ::daHolhei_c_ChangeState(this, &data_ov062_0211dee0);
    return 1;
}

int daHolhei_c::Behavior()
{
    if (UpdateKillByInvincibleChar(mMeshClsn, mModel, 3))
        return 1;

    DecIfAbove0_Short(&mStateTimer);
    DecIfAbove0_Short(&mTurnWait);
    DecIfAbove0_Short(&mChaseCooldown);

    {
        HolheiState* m = mState;
        if (m->update != 0)
            (this->*(m->update))();
    }

    mAngleX = mPrevAngleX;
    mAngleY = mPrevAngleY;
    mAngleZ = mPrevAngleZ;
    UpdatePos((dCc_c*)&mdCc_c);

    if (mState != &data_ov062_0211dea0) {
        if (mState != &data_ov062_0211dec0 && mState != &data_ov062_0211de70) {
            int atEdge = 0;
            signed char t = gCurrentSublevel;
            if (t == 0x16) {
                if (mPosX > (int)0xff95c000)
                    atEdge = 1;
            } else if (t == 0x15) {
                if (mPosZ < (int)0xff2f4000)
                    atEdge = 1;
            }
            if (atEdge != 0
                || (mHorzSpeed != 0
                    && IsGoingOffCliff(mMeshClsn, 0x3c000, (s16)0x2888, 0, 1, 0x32000))) {
                mHorzSpeed = 0;
                mEdgeStop = 1;
                mPosX = mPrevPosX;
                mPosY = mPrevPosY;
                mPosZ = mPrevPosZ;
            } else {
                if (mEdgeStop == 1)
                    mEdgeStop = 0;
            }
        }
        mPrevPosX = mPosX;
        mPrevPosY = mPosY;
        mPrevPosZ = mPosZ;
        UpdateWMClsn(mMeshClsn, 3);
    }

    if (mState == &data_ov062_0211ded0 || mState == &data_ov062_0211dee0 ||
        mState == &data_ov062_0211de90 || mState == &data_ov062_0211df00) {
        HandleCylinderHit();
    }

    mdCc_c.Clear();
    mModel.Advance();
    mModel.UpdateVerts();

    dActor_c *held = mHeld;
    if (held != 0 && IsFlagSet(this, 0x4000) && held->mCarryMatrix != 0) {
        UpdateHeldShadow();
    } else {
        mdCc_c.Update();
        mCarryOffsX = 0;
        mCarryOffsY = 0;
        mCarryOffsZ = 0;
        BuildHoldMatrix();
        UpdateShadow();
    }
    return 1;
}

int daHolhei_c::Render()
{
    dActor_c *held = mHeld;
    if (held != 0 && IsFlagSet(this, 0x4000) && held->mCarryMatrix != 0)
        HomeCarryOffset();
    /* dExtAnmModel_c overrides Render; the ROM calls dExtSimpleModel_c::Render on this member. */
    mModel.dExtSimpleModel_c::Render((const Vector3 *)0);
    return 1;
}

void daHolhei_c::OnPendingDestroy()
{
}

int daHolhei_c::CleanupResources()
{
    gHoruheiModelFile.Release();
    gHoruheiLiftAnimationFile.Release();
    gHoruheiLiftLoopAnimationFile.Release();
    gHoruheiStruggleAnimationFile.Release();
    gHoruheiThrowAnimationFile.Release();
    gHoruheiWalkAnimationFile.Release();
    return 1;
}

/* Home the carry offset toward the param1-selected Vector3, then publish
   the carried matrix onto the model. */
void daHolhei_c::HomeCarryOffset() {
    int idx = 0;
    if (mHeld->param1 == 2)
        idx = 1;
    Math_Function_0203b14c(&mCarryOffsX, data_ov062_0211df10[idx].x, 0x800, 0x3e8000, 4);
    Math_Function_0203b14c(&mCarryOffsY, data_ov062_0211df10[idx].y, 0x800, 0x3e8000, 4);
    Math_Function_0203b14c(&mCarryOffsZ, data_ov062_0211df10[idx].z, 0x800, 0x3e8000, 4);
    Matrix4x3 *r = UpdateCarry(*mHeld, *(Vector3 *)&mCarryOffsX);
    mModel.mat4x3 = *r;
}

/* Bone 3's transform times the model's yaw, stored where a holder looks. */
void daHolhei_c::BuildHoldMatrix() {
    Matrix4x3_FromRotationY(&mModel.mat4x3, mAngleY);
    mModel.mat4x3.m[9] = mPosX >> 3;
    mModel.mat4x3.m[10] = mPosY >> 3;
    mModel.mat4x3.m[11] = mPosZ >> 3;
    MTX_Concat43(&mModel.data.transforms[3], &mModel.mat4x3, &mHoldMtx);
    mCarryMatrix = &mHoldMtx;
}

/* Shadow under this actor. Off the ground the radius grows. */
void daHolhei_c::UpdateShadow() {
    mShadowMtx = IDENTITY_MATRIX4X3;
    mShadowMtx.m[9] = mPosX >> 3;
    mShadowMtx.m[10] = mPosY >> 3;
    mShadowMtx.m[11] = mPosZ >> 3;
    if (mMeshClsn.IsOnGround() != 0) {
        DropShadowRadHeight(mShadowModel, mShadowMtx, 0x12c000, 0x32000, 0xf);
    } else {
        DropShadowRadHeight(mShadowModel, mShadowMtx, 0x12c000, 0x3e8000, 0xf);
    }
}

/* Shadow under the held actor. mHeld is reloaded before each word. */
void daHolhei_c::UpdateHeldShadow() {
    mShadowMtx = IDENTITY_MATRIX4X3;
    mShadowMtx.m[9] = mHeld->mPosX >> 3;
    mShadowMtx.m[10] = mHeld->mPosY >> 3;
    mShadowMtx.m[11] = mHeld->mPosZ >> 3;
    DropShadowRadHeight(mShadowModel, mShadowMtx, 0x12c000, 0x32000, 0xf);
}

/* Coined name. Switches to state p and runs its enter function, if any;
   Behavior runs the record's update from then on. */
int daHolhei_c_ChangeState(daHolhei_c *self, HolheiState *p) {
    self->mState = p;
    HolheiState *q = self->mState;
    if (q->enter == 0)
        return 1;
    return (self->*q->enter)();
}

/* Face home, on the walk animation, and wait before turning. */
int daHolhei_c::func_ov062_02116c78() {
    mTurnWait = 0x3c;
    mChargeStep = 0;
    mModel.speed = 0x1000;
    mModel.SetAnim((BCA_File *)(gHoruheiWalkAnimationFile.filePtr), 0, 0x1000, 0);
    mTargetAngY = Vec3_HorzAngle(&mPos, reinterpret_cast<const Vector3 *>(&mHomePosX));
    return 1;
}

/* Brake, then turn onto mTargetAngY and go back to the walk state. */
int daHolhei_c::func_ov062_02116bf8() {
    ApproachLinear(mHorzSpeed, 0, 0x2000);
    if (mTurnWait == 0) {
        ApproachAngle(&mPrevAngleY, mTargetAngY, 0xa, 0x200, 0x100);
        if (AngleDiff(mTargetAngY, mAngleY) < 0x100)
            ::daHolhei_c_ChangeState(this, &data_ov062_0211dee0);
    }
    return 1;
}

/* Pick a random facing and a random time on the walk animation. */
int daHolhei_c::func_ov062_02116b80() {
    mTargetAngY = (short)(((unsigned int)RandomIntInternal(&data_0209e650.value) >> 8) << 0xc);
    mStateTimer =
        (short)((((unsigned int)RandomIntInternal(&data_0209e650.value) >> 8) & 0x3f) + 0x64);
    mModel.speed = 0x1000;
    mModel.SetAnim((BCA_File *)(gHoruheiWalkAnimationFile.filePtr), 0, 0x1000, 0);
    return 1;
}

/* Walk. A wall or the edge bails out; a player near home pulls Chuckya off
   the path once the chase cooldown has run out. */
int daHolhei_c::func_ov062_02116a08() {
    struct { int vd0[3]; int vd1[3]; } L;
    int r;
    dActor_c *pl;

    L.vd0[0] = 0;
    L.vd0[1] = 0;
    L.vd0[2] = 0;

    Vec3_Dist(&mPos, reinterpret_cast<const Vector3 *>(&mHomePosX));

    r = ClassifyObstruction();
    if (r != 0 || mEdgeStop == 1) {
        if (r != 2)
            ::daHolhei_c_ChangeState(this, &data_ov062_0211df00);
        else
            ::daHolhei_c_ChangeState(this, &data_ov062_0211de70);
        return 1;
    }

    ApproachAngle(&mPrevAngleY, mTargetAngY, 0xa, 0x200, 0x100);

    mHorzSpeed = 0xa000;
    pl = ClosestPlayer();

    mMoveSound = Sound::PlayLong(mMoveSound, 3, 0x18a, mCamSpacePos, 0);

    if (pl != 0 && mChaseCooldown == 0) {
        int *pos = &pl->mPosX;
        L.vd1[0] = pos[0];
        L.vd1[1] = pos[1];
        L.vd1[2] = pos[2];

        if (Vec3_Dist(reinterpret_cast<const Vector3 *>(&mHomePosX),
                      reinterpret_cast<const Vector3 *>(L.vd1)) < 0x3e8000) {
            mHorzSpeed = 0;
            ::daHolhei_c_ChangeState(this, &data_ov062_0211de90);
        }
        return 1;
    }

    if (mStateTimer == 0)
        ::daHolhei_c_ChangeState(this, &data_ov062_0211dee0);

    return 1;
}

/* Step back to the saved position and pick a new facing toward home. */
int daHolhei_c::func_ov062_02116980() {
    mPosX = mPrevPosX;
    mPosY = mPrevPosY;
    mPosZ = mPrevPosZ;
    mHorzSpeed = 0;
    if (AngleDiff(mTargetAngY,
                  Vec3_HorzAngle(&mPos, reinterpret_cast<const Vector3 *>(&mHomePosX))) <= 0x2000)
        mTargetAngY = (s16)(mPrevAngleY - 0x1000);
    else
        mTargetAngY = Vec3_HorzAngle(&mPos, reinterpret_cast<const Vector3 *>(&mHomePosX));
    mStateTimer = 0x46;
    return 1;
}

/* After the timer, either commit the turn into a walk or keep steering. */
int daHolhei_c::func_ov062_02116894() {
    if (mStateTimer != 0)
        return 1;
    if (mHorzSpeed != 0) {
        if (ClassifyObstruction() != 0 || mEdgeStop == 1) {
            mTargetAngY = (s16)(mPrevAngleY - 0x2000);
        } else {
            mPrevAngleY = mTargetAngY;
            ::daHolhei_c_ChangeState(this, &data_ov062_0211dee0);
            mChaseCooldown = 0x1e;
            mTargetAngY = mPrevAngleY;
            return 1;
        }
    }
    if (AngleDiff(mTargetAngY, mAngleY) < 0x100)
        mHorzSpeed = 0xa000;
    ApproachAngle(&mPrevAngleY, mTargetAngY, 0xa, 0x200, 0x100);
    return 1;
}

/* Stop, and aim the next turn at home. */
int daHolhei_c::func_ov062_02116850() {
    mVertSpeed = 0;
    mTargetAngY = Vec3_HorzAngle(&mPos, reinterpret_cast<const Vector3 *>(&mHomePosX));
    mStateTimer = 0x14;
    mHorzSpeed = 0;
    return 1;
}

/* Turn toward mTargetAngY. Facing it starts the walk; otherwise the timer
   is refreshed. When the timer expires, cool down and return to the walk. */
int daHolhei_c::func_ov062_021167c0() {
    ApproachAngle(&mPrevAngleY, mTargetAngY, 0xa, 0x200, 0x100);
    if (AngleDiff(mTargetAngY, mAngleY) < 0x100)
        mHorzSpeed = 0xa000;
    else
        mStateTimer = 0x14;
    if (mStateTimer == 0) {
        mChaseCooldown = 0x1e;
        ::daHolhei_c_ChangeState(this, &data_ov062_0211dee0);
    }
    return 1;
}

/* Restart the walk animation with the charge not yet begun. */
int daHolhei_c::func_ov062_02116784() {
    mChargeStep = 0;
    mModel.SetAnim((BCA_File *)(gHoruheiWalkAnimationFile.filePtr), 0, 0x1000, 0);
    return 1;
}

/* Charge the player. Step 0 turns, step 1 runs at the saved point, step 2
   gives up once Chuckya has gone past it. */
int daHolhei_c::func_ov062_021165e8()
{
    daPly_c *player;
    int r;
    Vector3 pos;

    player = ClosestPlayer();
    r = ClassifyObstruction();
    if (r != 0 || mEdgeStop == 1) {
        if (r != 2)
            ::daHolhei_c_ChangeState(this, &data_ov062_0211df00);
        else
            ::daHolhei_c_ChangeState(this, &data_ov062_0211de70);
        return 1;
    }

    if (player == 0)
        return 1;

    {
        const Vector3 &p = player->mPos;
        pos = p;
    }

    if (mChargeStep == 0) {
        mTargetAngY = HorzAngleToCPlayer();
        ApproachAngle(&mPrevAngleY, mTargetAngY, 0x80, 0x200, 0x400);
        if (AngleDiff(mTargetAngY, mAngleY) < 0x200) {
            mHorzSpeed = 0x1e000;
            mChargeStep = 1;
            mChasePosX = pos.x;
            mChasePosY = pos.y;
            mChasePosZ = pos.z;
        }
    }

    mMoveSound = Sound::PlayLong(mMoveSound, 3, 0x18a, mCamSpacePos, 0);

    {
        int s = mChargeStep;
        if (s != 0) {
            if (s == 1) {
                if (Vec3_Dist(&mPos, reinterpret_cast<const Vector3 *>(&mChasePosX)) < 0x3c000)
                    mChargeStep = 2;
            } else {
                if (Vec3_Dist(&mPos, reinterpret_cast<const Vector3 *>(&mChasePosX)) > 0xc8000)
                    ::daHolhei_c_ChangeState(this, &data_ov062_0211ded0);
            }
        }
    }
    return 1;
}

int daHolhei_c::func_ov062_021165e0() {
    return 1;
}

/* While carried: play the grab animation once, then on release copy the
   holder's facing, mark the cylinder, and hop if param1 is 2. */
int daHolhei_c::func_ov062_021164e8()
{
    if (mGrabAnim == 0 && IsFlagSet(this, 0x4000)) {
        mModel.SetAnim((BCA_File *)(gHoruheiStruggleAnimationFile.filePtr), 0, 0x1000, 0);
        mGrabAnim = 1;
    }
    if (IsFlagSet(this, 0x400) || IsFlagSet(this, 0x2000) || !IsFlagSet(this, 0x100)) {
        dActor_c *held = mHeld;
        mPrevAngleY = held->mAngleY;
        mdCc_c.flags |= 2;
        ::daHolhei_c_ChangeState(this, &data_ov062_0211dec0);
        if (mHeld->param1 == 2) {
            mVertSpeed = 0x50000;
            mHorzSpeed = 0x14000;
        }
        mHeld = 0;
    }
    return 1;
}

/* Pop off: gravity, a launch, and a clear of the runtime flags. */
int daHolhei_c::func_ov062_02116498() {
    mVertAccel = -0x4000;
    mVertSpeed = 0x3c000;
    mHorzSpeed = 0xa000;
    mTerminalVelocity = -0x64000;
    func_02012694(0xf7, (const Vector3 *)(&mCamSpacePosX));
    mFlags = 0;
    return 1;
}

/* Below the death plane, or on landing: poof, shake, drop coins, die. */
int daHolhei_c::func_ov062_021163b0() {
    Vector3 v[2];
    if (data_02092138 > mPosY) {
        KillAndTrackInDeathTable();
        return 1;
    }
    if (mMeshClsn.IsOnGround() != 0) {
        TriplePoofDust();
        KillAndTrackInDeathTable();
        func_02012694(0x125, (const Vector3 *)(&mCamSpacePosX));
        gCamera->func_0200d8c8(&mPos, 0x7d0000);
        v[0].x = mPosX;
        v[0].y = mPosY;
        v[0].z = mPosZ;
        v[0].y += 0x32000;
        v[1].x = mPosX;
        v[1].y = mPosY;
        v[1].z = mPosZ;
        SpawnCoins(v[1], mExtraCoinCount + 1, 0xa000, 0);
    }
    return 1;
}

int daHolhei_c::func_ov062_02116368() {
    mModel.SetAnim((BCA_File *)(gHoruheiLiftAnimationFile.filePtr), 0x40000000, 0x1000, 0);
    mChargeStep = 0;
    return 1;
}

/* When the animation finishes, leave this state. */
int daHolhei_c::func_ov062_0211632c() {
    if (mModel.Finished()) {
        mChargeStep = 0;
        ::daHolhei_c_ChangeState(this, &data_ov062_0211de80);
    }
    return 1;
}

/* Start an animation, then wait a random few frames facing a random way. */
int daHolhei_c::func_ov062_021162b8() {
    mModel.SetAnim((BCA_File *)(gHoruheiLiftLoopAnimationFile.filePtr), 0, 0x1000, 0);
    mStateTimer =
        (short)((((unsigned int)RandomIntInternal(&data_0209e650.value) >> 8) & 0x1f) + 0x14);
    mTargetAngY = (short)(((unsigned int)RandomIntInternal(&data_0209e650.value) >> 8) << 0xc);
    return 1;
}

/* Spin, and leave once the timer reaches zero. */
int daHolhei_c::func_ov062_02116274() {
    mPrevAngleY += 0x500;
    if (mStateTimer == 0)
        ::daHolhei_c_ChangeState(this, &data_ov062_0211deb0);
    return 1;
}

int daHolhei_c::func_ov062_02116238() {
    mModel.SetAnim((BCA_File *)(gHoruheiThrowAnimationFile.filePtr), 0x40000000, 0x1000, 0);
    return 1;
}

/* On frame 0x14 of the throw animation, launch whoever is held. When the
   animation finishes, clear the cylinder bit and leave. */
int daHolhei_c::func_ov062_021161a8() {
    if (mHeld != 0 && mModel.WillHitFrame(0x14)) {
        func_ov002_020db54c(mHeld, 0x28000, 0x50000, mAngleY);
        mHeld = 0;
        func_02012694(0x126, (const Vector3 *)(&mCamSpacePosX));
    }
    if (mModel.Finished()) {
        mdCc_c.flags &= ~2;
        ::daHolhei_c_ChangeState(this, &data_ov062_0211ded0);
    }
    return 1;
}

/* Hit flag 0x10 kills through KillByInvincibleChar. Otherwise, when the
   hitter is a PLAYER (actor 0xbf): with the closest player's bearing more
   than 0x2000 (45 degrees) off Chuckya's facing, hit flag 0x1000 lets the
   hitter try to grab it; within 0x2000, Chuckya tries to grab the hitter
   (func_ov002_020db5f4). */
void daHolhei_c::HandleCylinderHit() {
    u32 id;
    dActor_c *a;
    int angle;
    struct Vector3_16 v;

    id = mdCc_c.otherOwner;
    if (id != 0 && (a = dActor_c::FindWithID(id)) != 0 && (mdCc_c.hitFlags & 0x10) != 0) {
        v.x = 0x1000;
        v.y = 0;
        v.z = 0;
        KillByInvincibleChar(v, *(daPly_c *)a, 0);
        func_02012694(0x125, (const Vector3 *)(&mCamSpacePosX));
        return;
    }

    mFlags &= ~0x80;

    id = mdCc_c.otherOwner;
    if (id == 0)
        return;
    a = dActor_c::FindWithID(id);
    if (a == 0)
        return;
    if (!IsPlayerActor(a))
        return;
    daPly_c *player = static_cast<daPly_c *>(a);
    angle = HorzAngleToCPlayer();
    angle = AngleDiff(angle, mAngleY);
    if (angle > 0x2000) {
        if ((mdCc_c.hitFlags & 0x1000) == 0)
            return;
        mFlags |= 0x80;
        if (player->TryGrab(*this) == 0)
            return;
        mHeld = player;
        mdCc_c.flags |= 2;
        mHorzSpeed = 0;
        ::daHolhei_c_ChangeState(this, &data_ov062_0211dea0);
        return;
    }
    if (func_ov002_020db5f4(player, this) == 0)
        return;
    mHeld = player;
    mHorzSpeed = 0;
    ::daHolhei_c_ChangeState(this, &data_ov062_0211def0);
}

/* 1 on a wall, 2 on a slope steeper than 0x1000, else 0. */
int daHolhei_c::ClassifyObstruction() {
    Vector3 v;
    short slope = 0;
    if (mMeshClsn.IsOnGround()) {
        dBgPi *floor = mMeshClsn.GetFloorResult();
        floor->surface.CopyNormalTo(v);
        slope = func_02010844(&v, mAngleY);
    }
    if (mMeshClsn.IsOnWall())
        return 1;
    if (slope < 0)
        slope = (short)-slope;
    if (slope > 0x1000)
        return 2;
    return 0;
}
