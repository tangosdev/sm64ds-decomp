//cpp
/* daBasabasa_c -- the swoop (BASABASA), ov065 0x02116f98..0x02117f40.
 * _ZTI12daBasabasa_c is at 0x0211cc60; the vtable address point is 0x0211cca0.
 * The base is dEnemyBase_c. The destructor is the key function. defer_codegen
 * off emits D1, D0, then a D2 with no ROM home, in source order. The factory
 * (was Swoop_Spawn) is `new daBasabasa_c()`.
 */
/* Notes: the shared file handles and the four state records keep address
 * names; their definitions at the bottom are this file's static initializer.
 * actorID and mFlags are tested through an int. ExecutePerch keeps its base
 * pointer. mSpeedZ is reloaded because a named temporary moves a word.
 * EnterDrop's PoofPos is a three-int constructor; a Vector3 assignment does
 * not schedule the same stores. mHomePosX..Z stay three words: a Vector3
 * union member emits a local-class destructor this TU has no home for.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daBasabasa_c_AssetId {
    ASSET_BASABASA_BMD = 0x0298,
    ASSET_BASABASA_FLY_BCA = 0x0299,
    ASSET_BASABASA_WAIT_BCA = 0x029a,
    ASSET_BASABASA_WAIT_BMD = 0x029b
};

#pragma defer_codegen off
#include "common.h"
#include "daBasabasa_c.h"
#include "SharedFilePtr.h"
#include "Player.h"
#include "math/Approach.h"
#include "math/CalcLib.h"
#include "math/MtxStack.h"

/* EnterDrop's unused stack temporary. The three-int constructor is what
 * schedules the stores; a plain Vector3 assignment does not. */
struct PoofPos {
    int x, y, z;
    PoofPos(int a, int b, int c) : x(a), y(b), z(c) {}
    ~PoofPos() {}
};

/* The model handles construct through func_02017acc and destroy through
 * func_02017ab4; the animation handles go through SharedFilePtr::Construct
 * and SharedFilePtr_Destruct_Anim. The wrappers are declared, never defined:
 * the manifest aliases their undefined members onto the ROM veneers. */
struct BasabasaModelFilePtr : SharedFilePtr {
    BasabasaModelFilePtr(u32 fileID);
    ~BasabasaModelFilePtr();
};

struct BasabasaAnimationFileHandle : SharedFilePtr {
    BasabasaAnimationFileHandle(u32 fileID);
    ~BasabasaAnimationFileHandle();
};

/* arm9 C helpers with no shared header. Vec3_Asr's definition takes a
 * file-local struct, so no shared spelling agrees with it yet. */
extern "C" {
void func_ov002_020aea30(void *self, void *actor, void *collision);
void func_02012694(unsigned int id, const Vector3 *pos);
void MulVec3Mat4x3(const void *in, const void *m, void *out);
void Vec3_Asr(Vector3 *d, Vector3 *s, int sh);
}

extern int data_0209e650; /* RandomIntInternal's seed */

extern BasabasaAnimationFileHandle data_ov065_0211d690; /* flying animation */
extern BasabasaModelFilePtr data_ov065_0211d698;        /* flying model */
extern BasabasaAnimationFileHandle data_ov065_0211d6a0; /* hanging animation */
extern BasabasaModelFilePtr data_ov065_0211d6a8;        /* hanging model */

/* The four states, by the address of their .bss record. */
extern daBasabasa_c::State data_ov065_0211d6e0; /* chase */
extern daBasabasa_c::State data_ov065_0211d6f0; /* return */
extern daBasabasa_c::State data_ov065_0211d700; /* perch */
extern daBasabasa_c::State data_ov065_0211d710; /* drop */

// @symbol _ZN12daBasabasa_cD1Ev
// @symbol _ZN12daBasabasa_cD0Ev
/* The compiler writes the whole body: the vtable store, the dExtShadowModel_c, both
 * dExtAnmModel_c, the dBgCh_Actr and the dCcAc_c in reverse declaration order,
 * then dEnemyBase_c::~dEnemyBase_c. */
daBasabasa_c::~daBasabasa_c() {}

// @symbol _ZN12daBasabasa_c10CheckTouchEv
/* Reacts to whatever the collision cylinder touched this frame. */
void daBasabasa_c::CheckTouch() {
    Player *p;
    int flags;
    int isPlayer;

    if (mdCcAc_c.otherOwner == 0)
        return;
    p = (Player *)dActor_c::FindWithID(mdCcAc_c.otherOwner);
    if (p == 0)
        return;
    flags = mdCcAc_c.hitFlags;
    if (flags & 0x2400) {
        mIsFlying = 1;
        mAnmModel1.SetAnim((BCA_File *)data_ov065_0211d690.filePtr, 0, 0x1000, 0);
        mDeathState = 2;
        func_ov002_020aea30(this, p, 0);
        func_02012694(0x112, &mCamSpacePos);
        return;
    }
    if (flags & 0x10) {
        short v[3];
        v[0] = -0x2000;
        v[1] = 0;
        v[2] = 0;
        KillByInvincibleChar(*(Vector3_16 *)v, *p, 0);
        func_02012694(0x112, &mCamSpacePos);
        return;
    }
    if (flags & 0x40000) {
        mDeathState = 4;
        mIsFlying = 1;
        mAnmModel1.SetAnim((BCA_File *)data_ov065_0211d690.filePtr, 0, 0x1000, 0);
        func_ov002_020aea30(this, p, 0);
        func_02012694(0x112, &mCamSpacePos);
        return;
    }
    if (flags & 0x4380) {
        mIsFlying = 1;
        mAnmModel1.SetAnim((BCA_File *)data_ov065_0211d690.filePtr, 0, 0x1000, 0);
        mDeathState = 3;
        func_ov002_020aea30(this, p, 0);
        func_02012694(0x112, &mCamSpacePos);
        return;
    }
    if (flags & 0x40) {
        mIsFlying = 1;
        mAnmModel1.SetAnim((BCA_File *)data_ov065_0211d690.filePtr, 0, 0x1000, 0);
        mDeathState = 2;
        func_ov002_020aea30(this, p, 0);
        func_02012694(0x112, &mCamSpacePos);
        return;
    }
    if (flags & 0x20) {
        mDeathState = 1;
        func_ov002_020aea30(this, p, 0);
        func_02012694(0x112, &mCamSpacePos);
        return;
    }

    /* Through an int: testing actorID in the `if` compiles differently. */
    isPlayer = (int)(p->actorID == 0xbf);
    if (isPlayer == 0)
        return;

    if (BumpedUnderneathByPlayer(*p) == 1 || p->IsOnShell() == 1 || p->mIsMetal == 1) {
        Vector3 pos;
        SmallPoofDust();
        KillAndTrackInDeathTable();
        func_02012694(0x112, &mCamSpacePos);
        pos.x = mPosX;
        pos.y = mPosY;
        pos.z = mPosZ;
        SpawnCoins(pos, mExtraCoinCount + 1, 0xa000, 0);
        return;
    }

    if (JumpedOnByPlayer(mdCcAc_c, *p) != 0) {
        p->Bounce(0x28000);
        mDeathState = 1;
        func_ov002_020aea30(this, p, 0);
        func_02012694(0x112, &mCamSpacePos);
        return;
    }
    Vector3 pos;
    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    p->Hurt(pos, 1, 0xc000, 1, 0, 1);
    if (mCurrentState != &data_ov065_0211d6e0)
        return;
    mPerchTimer = 0;
    SetState(&data_ov065_0211d6f0);
}

// @symbol _ZN12daBasabasa_c13ExecuteReturnEv
int daBasabasa_c::ExecuteReturn() {
    Vector3 v;
    v.x = 0;
    v.y = 0;
    v.z = 0;
    int dist = Vec3_Dist(&mPos, (Vector3 *)&mHomePosX);
    if (dist > 0x1f4000 || mWithMeshClsn.IsOnWall() != 0) {
        mTargetAngleY = Vec3_HorzAngle(&mPos, (Vector3 *)&mHomePosX);
        /* The states read mStateTimer unsigned (ldrh). */
        if ((u16)mStateTimer < 0x14)
            mStateTimer = 0x14;
    }
    if (dist < 0x1f4000 && (mFlags & 8)) {
        mPerchTimer++;
        if (mPerchTimer > 0x64) {
            mPerchTimer = 0;
            mFlags |= 1;
            mPrevAngleX = -0x8000;
            mAngleX = mPrevAngleX;
            mPosX = mHomePosX;
            mPosY = mHomePosY;
            mPosZ = mHomePosZ;
            mSpeedX = 0;
            mVertSpeed = 0;
            mSpeedZ = 0;
            SetState(&data_ov065_0211d700);
            return 1;
        }
    }

    ApproachAngle(&mPrevAngleX, 0, 0xa, 0x200, 0x100);
    ApproachAngle(&mPrevAngleY, mTargetAngleY, 0xa, 0x200, 0x100);
    v.z = 0xa000;
    Matrix4x3_FromRotationY(&data_020a0e68, mAngleY);
    MulVec3Mat4x3(&v, &data_020a0e68, &mSpeedX);
    if ((u16)mStateTimer == 0) {
        SetState(&data_ov065_0211d6f0);
    }
    return 1;
}

// @symbol _ZN12daBasabasa_c11EnterReturnEv
int daBasabasa_c::EnterReturn() {
    unsigned int r;
    r = RandomIntInternal(&data_0209e650);
    mTargetAngleY = ((r >> 8) & 0xf) << 0xc;
    r = RandomIntInternal(&data_0209e650);
    mStateTimer = ((r >> 8) & 0x1f) + 0x32;
    func_02012694(0xf9, &mCamSpacePos);
    mAnmModel1.speed = 0x1000;
    mFlags &= ~1;
    return 1;
}

// @symbol _ZN12daBasabasa_c12ExecuteChaseEv
int daBasabasa_c::ExecuteChase() {
    Vector3 v;
    v.x = 0;
    v.y = 0;
    v.z = 0;
    mTargetAngleY = HorzAngleToCPlayer();
    ApproachLinear(mPrevAngleY, mTargetAngleY, 0x500);
    ApproachLinear(mPrevAngleX, 0x100, 0x500);
    v.z = 0xa000;
    Matrix4x3_FromRotationY(&data_020a0e68, mAngleY);
    Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, mPrevAngleX);
    MulVec3Mat4x3(&v, &data_020a0e68, &mSpeedX);
    if (Vec3_Dist(&mPos, (Vector3 *)&mHomePosX) > 0x5dc000 || (u16)mStateTimer == 0) {
        mPerchTimer = 0;
        SetState(&data_ov065_0211d6f0);
    }
    return 1;
}

// @symbol _ZN12daBasabasa_c10EnterChaseEv
int daBasabasa_c::EnterChase() {
    mVertAccel = 0;
    mIsFlying = 1;
    mFlags = 0x10000003;
    mStateTimer = 0x64;
    mAnmModel1.speed = 0x2000;
    mAnmModel1.SetAnim((BCA_File *)data_ov065_0211d690.filePtr, 0, 0x1000, 0);
    mTargetAngleY = (((u32)RandomIntInternal(&data_0209e650) >> 8) & 0xf) << 0xc;
    return 1;
}

// @symbol _ZN12daBasabasa_c11ExecuteDropEv
int daBasabasa_c::ExecuteDrop() {
    int floorY, posY;
    Player *p;
    mPrevAngleY += 0x1000;
    p = ClosestPlayer();
    posY = mPosY;
    floorY = posY - 0xc8000;
    if (p != 0) {
        floorY = p->mGroundY + 0x50000;
    }
    if ((u16)mStateTimer == 0 || floorY > posY) {
        SetState(&data_ov065_0211d6e0);
    }
    return 1;
}

// @symbol _ZN12daBasabasa_c9EnterDropEv
int daBasabasa_c::EnterDrop() {
    mVertAccel = -0x1000;
    mTerminalVelocity = -0xa000;
    mIsFlying = 0;
    mAnmModel2.speed = 0;
    mAnmModel2.SetAnim((BCA_File *)data_ov065_0211d6a0.filePtr, 0, 0x1000, 0);
    PoofPos vec(mPosX, mPosY + 0x64000, mPosZ);
    mStateTimer = 0x19;
    if (mPosY == 0x898000)
        mStateTimer = 0x32;
    func_02012694(0xf9, &mCamSpacePos);
    return 1;
}

// @symbol _ZN12daBasabasa_c12ExecutePerchEv
int daBasabasa_c::ExecutePerch() {
    Player *p = ClosestNonVanishPlayer();
    if (p) {
        Vector3 v;
        /* One base pointer: reading p->mPosX and so on directly compiles differently. */
        int *q = &p->mPosX;
        v.x = q[0];
        v.y = q[1];
        v.z = q[2];
        if (Vec3_Dist(&mPos, &v) < 0x3e8000) {
            mStateTimer = 0x14;
            SetState(&data_ov065_0211d710);
        }
    }
    return 1;
}

// @symbol _ZN12daBasabasa_c10EnterPerchEv
int daBasabasa_c::EnterPerch() {
    mFlags = 3;
    mIsFlying = 0;
    mAnmModel2.speed = 0x1000;
    mAnmModel2.SetAnim((BCA_File *)data_ov065_0211d6a0.filePtr, 0, 0x1000, 0);
    return 1;
}

// @symbol _ZN12daBasabasa_c8SetStateEPNS_5StateE
int daBasabasa_c::SetState(State *state) {
    mCurrentState = state;
    if (mCurrentState->enter == 0)
        return 1;
    return (this->*mCurrentState->enter)();
}

// @symbol _ZN12daBasabasa_c20UpdateModelTransformEv
/* Places both models and the drop shadow at the current position. */
void daBasabasa_c::UpdateModelTransform() {
    Vector3 v;
    Vec3_Asr(&v, &mPos, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, v.x, v.y, v.z);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68, mAngleX, mAngleY, mAngleZ);
    if (mIsFlying == 1) {
        mAnmModel1.mat4x3 = data_020a0e68;
    } else {
        mAnmModel2.mat4x3 = data_020a0e68;
    }
    Matrix4x3_FromTranslation(&data_020a0e68, mPosX >> 3, (mPosY - 0x18000) >> 3, mPosZ >> 3);
    mShadowMatrix = data_020a0e68;
    DropShadowRadHeight(mShadowModel, mShadowMatrix, 0x32000, 0x258000, 0xf);
}

// @symbol _ZN12daBasabasa_c16CleanupResourcesEv
/* Releases the four files InitResources claimed. */
int daBasabasa_c::CleanupResources() {
    data_ov065_0211d698.Release();
    data_ov065_0211d6a8.Release();
    data_ov065_0211d690.Release();
    data_ov065_0211d6a0.Release();
    return 1;
}

// @symbol _ZN12daBasabasa_c16OnPendingDestroyEv
void daBasabasa_c::OnPendingDestroy() {}

// @symbol _ZN12daBasabasa_c6RenderEv
int daBasabasa_c::Render() {
    /* Through an int: testing mFlags in the `if` compiles differently. */
    int flag = (mFlags & 0x40000) != 0;
    if (flag != 0)
        return 1;
    if (mIsFlying == 1) {
        mAnmModel1.Render(0);
    } else {
        mAnmModel2.Render(0);
    }
    return 1;
}

// @symbol _ZN12daBasabasa_c8BehaviorEv
int daBasabasa_c::Behavior() {
    if (UpdateYoshiEat(mWithMeshClsn) != 0) {
        mdCcAc_c.Clear();
        if (mEatenByYoshi != 0) {
            if (mSpitOutTimer == 0) {
                mdCcAc_c.Update();
            }
        }
        UpdateModelTransform();
        return 1;
    }
    if (UpdateKillByInvincibleChar(mWithMeshClsn, mAnmModel1, 3) != 0) {
        return 1;
    }
    if (mDeathState != 0) {
        UpdateDeath(mWithMeshClsn);
        UpdateModelTransform();
        return 1;
    }
    DecIfAbove0_Short((unsigned short *)&mStateTimer);
    if (mCurrentState->execute != 0)
        (this->*mCurrentState->execute)();
    if (mCurrentState == &data_ov065_0211d6e0 || mCurrentState == &data_ov065_0211d6f0) {
        /* Three wingbeat frames, one flap sound. */
        if (mAnmModel1.WillHitFrame(3) != 0 || mAnmModel1.WillHitFrame(0xf) != 0 ||
            mAnmModel1.WillHitFrame(0x1b) != 0) {
            func_02012694(0xe1, &mCamSpacePos);
        }
    }
    {
        /* Gravity, clamped at terminal velocity. mSpeedZ is reloaded and
           stored back unchanged, as in the ROM. */
        int fallSpeed = mVertSpeed + mVertAccel;
        int clamped = mTerminalVelocity;
        if (fallSpeed >= clamped)
            clamped = fallSpeed;
        int keep = mSpeedZ;
        mVertSpeed = clamped;
        mSpeedZ = keep;
    }
    UpdatePosWithOnlySpeed(&mdCcAc_c);
    UpdateWMClsn(mWithMeshClsn, 0);
    mAngleX = mPrevAngleX;
    mAngleY = mPrevAngleY;
    mAngleZ = mPrevAngleZ;
    UpdateModelTransform();
    if (mIsFlying == 1) {
        CheckTouch();
    }
    mdCcAc_c.Clear();
    {
        Player *p = ClosestPlayer();
        if (p != 0 && p->mIsVanish == 0) {
            mdCcAc_c.Update();
        }
    }
    if (mIsFlying == 1) {
        mAnmModel1.Advance();
    } else {
        mAnmModel2.Advance();
    }
    return 1;
}

// @symbol _ZN12daBasabasa_c13InitResourcesEv
/* Both dExtAnmModel_c are loaded here, the other half of the evidence that 0x300
 * and 0x364 are two members. */
int daBasabasa_c::InitResources() {
    mAnmModel1.SetFile((BMD_File *)dExtSimpleModel_c::LoadFile(data_ov065_0211d698), 1, -1);
    mAnmModel2.SetFile((BMD_File *)dExtSimpleModel_c::LoadFile(data_ov065_0211d6a8), 1, -1);
    mShadowModel.InitCylinder();
    dExtFrameCtrl_c::LoadFile(data_ov065_0211d690);
    dExtFrameCtrl_c::LoadFile(data_ov065_0211d6a0);
    mTerminalVelocity = -0xa000;
    mdCcAc_c.Init(this, 0x28000, 0x28000, 0x200000, 0x7eff0);
    mAngleY = mPrevAngleY;
    mPrevAngleX = -0x8000;
    mAngleX = mPrevAngleX;
    mIsFlying = 0;
    mAnmModel1.speed = 0x1000;
    mAnmModel2.speed = 0x1000;
    mWithMeshClsn.Init(this, 0x28000, 0x1e000, 0, 0);
    mHomePosX = mPosX;
    mHomePosY = mPosY;
    mHomePosZ = mPosZ;
    mCoinType = 1;
    SetState(&data_ov065_0211d700);
    return 1;
}

// @symbol _ZN12daBasabasa_c16OnAimedAtWithEggEv
s32 daBasabasa_c::OnAimedAtWithEgg() {
    return 0;
}

// @symbol _ZN12daBasabasa_c13OnTurnIntoEggER6Player
void daBasabasa_c::OnTurnIntoEgg(Player &player) {
    GivePlayerCoins(player, (unsigned char)(mExtraCoinCount + 1), 0);
    KillAndTrackInDeathTable();
}

// @symbol _ZN12daBasabasa_c13OnYoshiTryEatEv
s32 daBasabasa_c::OnYoshiTryEat() {
    return 4;
}

// @symbol daBasabasa_c_classInit
extern "C" daBasabasa_c *daBasabasa_c_classInit() {
    return new daBasabasa_c();
}

/* Retail initializer order is flying model 0x298, hanging model 0x29b,
 * flying anim 0x299, hanging anim 0x29a, then the perch/drop/chase/return
 * State records. mwcc emits __sinit_d_a_basabasa.cpp from these definitions,
 * including the destructor registrations and the PMF descriptor constants
 * at 0x0211cc20..0x0211cc60. */
BasabasaModelFilePtr data_ov065_0211d698(ASSET_BASABASA_BMD);
BasabasaModelFilePtr data_ov065_0211d6a8(ASSET_BASABASA_WAIT_BMD);
BasabasaAnimationFileHandle data_ov065_0211d690(ASSET_BASABASA_FLY_BCA);
BasabasaAnimationFileHandle data_ov065_0211d6a0(ASSET_BASABASA_WAIT_BCA);
daBasabasa_c::State data_ov065_0211d700 = {&daBasabasa_c::EnterPerch, &daBasabasa_c::ExecutePerch};
daBasabasa_c::State data_ov065_0211d710 = {&daBasabasa_c::EnterDrop, &daBasabasa_c::ExecuteDrop};
daBasabasa_c::State data_ov065_0211d6e0 = {&daBasabasa_c::EnterChase, &daBasabasa_c::ExecuteChase};
daBasabasa_c::State data_ov065_0211d6f0 = {&daBasabasa_c::EnterReturn,
                                           &daBasabasa_c::ExecuteReturn};
