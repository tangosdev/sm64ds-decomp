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
 * pointer. mSpeed.z is reloaded because a named temporary moves a word.
 * EnterDrop's unused Vector3 temporary is built with Vector3's (x, y, z)
 * constructor; three field stores do not schedule the same.
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
#include "daPly_c.h"
#include "math/Approach.h"
#include "math/CalcLib.h"
#include "math/MtxStack.h"
#include "LevelRng.h"

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

void func_02012694(unsigned int id, const Vector3 *pos);

extern BasabasaAnimationFileHandle data_ov065_0211d690; /* flying animation */
extern BasabasaModelFilePtr data_ov065_0211d698;        /* flying model */
extern BasabasaAnimationFileHandle data_ov065_0211d6a0; /* hanging animation */
extern BasabasaModelFilePtr data_ov065_0211d6a8;        /* hanging model */

/* The four states, by the address of their .bss record. */
extern daBasabasa_c::State data_ov065_0211d6e0; /* chase */
extern daBasabasa_c::State data_ov065_0211d6f0; /* return */
extern daBasabasa_c::State data_ov065_0211d700; /* perch */
extern daBasabasa_c::State data_ov065_0211d710; /* drop */

/* The compiler writes the whole body: the vtable store, the dExtShadowModel_c, both
 * dExtAnmModel_c, the dBgCh_Actr and the dCcAc_c in reverse declaration order,
 * then dEnemyBase_c::~dEnemyBase_c. */
daBasabasa_c::~daBasabasa_c() {}

/* Reacts to whatever the collision cylinder touched this frame. */
void daBasabasa_c::CheckTouch() {
    daPly_c *p;
    int flags;
    int isPlayer;

    if (mdCcAc_c.otherOwner == 0)
        return;
    p = (daPly_c *)dActor_c::FindWithID(mdCcAc_c.otherOwner);
    if (p == 0)
        return;
    flags = mdCcAc_c.hitFlags;
    if (flags & 0x2400) {
        mIsFlying = 1;
        mAnmModel1.SetAnim((BCA_File *)data_ov065_0211d690.filePtr, 0, 0x1000, 0);
        mDeathState = 2;
        KillByAttack(p, 0);
        func_02012694(0x112, &mCamSpacePos);
        return;
    }
    if (flags & 0x10) {
        Vector3_16 v;
        v.x = -0x2000;
        v.y = 0;
        v.z = 0;
        KillByInvincibleChar(v, *p, 0);
        func_02012694(0x112, &mCamSpacePos);
        return;
    }
    if (flags & 0x40000) {
        mDeathState = 4;
        mIsFlying = 1;
        mAnmModel1.SetAnim((BCA_File *)data_ov065_0211d690.filePtr, 0, 0x1000, 0);
        KillByAttack(p, 0);
        func_02012694(0x112, &mCamSpacePos);
        return;
    }
    if (flags & 0x4380) {
        mIsFlying = 1;
        mAnmModel1.SetAnim((BCA_File *)data_ov065_0211d690.filePtr, 0, 0x1000, 0);
        mDeathState = 3;
        KillByAttack(p, 0);
        func_02012694(0x112, &mCamSpacePos);
        return;
    }
    if (flags & 0x40) {
        mIsFlying = 1;
        mAnmModel1.SetAnim((BCA_File *)data_ov065_0211d690.filePtr, 0, 0x1000, 0);
        mDeathState = 2;
        KillByAttack(p, 0);
        func_02012694(0x112, &mCamSpacePos);
        return;
    }
    if (flags & 0x20) {
        mDeathState = 1;
        KillByAttack(p, 0);
        func_02012694(0x112, &mCamSpacePos);
        return;
    }

    /* Through an int: testing actorID in the `if` compiles differently. */
    isPlayer = (p->actorID == 0xbf);
    if (isPlayer == 0)
        return;

    if (BumpedUnderneathByPlayer(*p) == 1 || p->IsOnShell() == 1 || p->mIsMetal == 1) {
        Vector3 pos;
        SmallPoofDust();
        KillAndTrackInDeathTable();
        func_02012694(0x112, &mCamSpacePos);
        pos.x = mPos.x;
        pos.y = mPos.y;
        pos.z = mPos.z;
        SpawnCoins(pos, mExtraCoinCount + 1, 0xa000, 0);
        return;
    }

    if (JumpedOnByPlayer(mdCcAc_c, *p) != 0) {
        p->Bounce(0x28000);
        mDeathState = 1;
        KillByAttack(p, 0);
        func_02012694(0x112, &mCamSpacePos);
        return;
    }
    Vector3 pos;
    pos.x = mPos.x;
    pos.y = mPos.y;
    pos.z = mPos.z;
    p->Hurt(pos, 1, 0xc000, 1, 0, 1);
    if (mCurrentState != &data_ov065_0211d6e0)
        return;
    mPerchTimer = 0;
    SetState(&data_ov065_0211d6f0);
}

int daBasabasa_c::ExecuteReturn() {
    Vector3 v;
    v.x = 0;
    v.y = 0;
    v.z = 0;
    int dist = Vec3_Dist(&mPos, &mHomePos);
    if (dist > 0x1f4000 || mWithMeshClsn.IsOnWall() != 0) {
        mTargetAngleY = Vec3_HorzAngle(&mPos, &mHomePos);
        /* The states read mStateTimer unsigned (ldrh). */
        if (mStateTimer < 20)
            mStateTimer = 20;
    }
    if (dist < 0x1f4000 && (mFlags & 8)) {
        mPerchTimer++;
        if (mPerchTimer > 100) {
            mPerchTimer = 0;
            mFlags |= 1;
            mPrevAngle.x = -0x8000;
            mAngle.x = mPrevAngle.x;
            mPos.x = mHomePos.x;
            mPos.y = mHomePos.y;
            mPos.z = mHomePos.z;
            mSpeed.x = 0;
            mSpeed.y = 0;
            mSpeed.z = 0;
            SetState(&data_ov065_0211d700);
            return 1;
        }
    }

    ApproachAngle(&mPrevAngle.x, 0, 0xa, 0x200, 0x100);
    ApproachAngle(&mPrevAngle.y, mTargetAngleY, 0xa, 0x200, 0x100);
    v.z = 0xa000;
    Matrix4x3_FromRotationY(&gScratchMtx, mAngle.y);
    MTX_MultVec43(&v, &gScratchMtx, &mSpeed);
    if (mStateTimer == 0) {
        SetState(&data_ov065_0211d6f0);
    }
    return 1;
}

int daBasabasa_c::EnterReturn() {
    unsigned int r;
    r = RandomIntInternal(&data_0209e650.value);
    mTargetAngleY = ((r >> 8) & 0xf) << 0xc;
    r = RandomIntInternal(&data_0209e650.value);
    mStateTimer = ((r >> 8) & 0x1f) + 50;
    func_02012694(0xf9, &mCamSpacePos);
    mAnmModel1.speed = 0x1000;
    mFlags &= ~1;
    return 1;
}

int daBasabasa_c::ExecuteChase() {
    Vector3 v;
    v.x = 0;
    v.y = 0;
    v.z = 0;
    mTargetAngleY = HorzAngleToCPlayer();
    ApproachLinear(mPrevAngle.y, mTargetAngleY, 0x500);
    ApproachLinear(mPrevAngle.x, 0x100, 0x500);
    v.z = 0xa000;
    Matrix4x3_FromRotationY(&gScratchMtx, mAngle.y);
    Matrix4x3_ApplyInPlaceToRotationX(&gScratchMtx, mPrevAngle.x);
    MTX_MultVec43(&v, &gScratchMtx, &mSpeed);
    if (Vec3_Dist(&mPos, &mHomePos) > 0x5dc000 || mStateTimer == 0) {
        mPerchTimer = 0;
        SetState(&data_ov065_0211d6f0);
    }
    return 1;
}

int daBasabasa_c::EnterChase() {
    mVertAccel = 0;
    mIsFlying = 1;
    mFlags = 0x10000003;
    mStateTimer = 100;
    mAnmModel1.speed = 0x2000;
    mAnmModel1.SetAnim((BCA_File *)data_ov065_0211d690.filePtr, 0, 0x1000, 0);
    mTargetAngleY = (((u32)RandomIntInternal(&data_0209e650.value) >> 8) & 0xf) << 0xc;
    return 1;
}

int daBasabasa_c::ExecuteDrop() {
    int floorY, posY;
    daPly_c *p;
    mPrevAngle.y += 0x1000;
    p = ClosestPlayer();
    posY = mPos.y;
    floorY = posY - 0xc8000;
    if (p != 0) {
        floorY = p->mGroundY + 0x50000;
    }
    if (mStateTimer == 0 || floorY > posY) {
        SetState(&data_ov065_0211d6e0);
    }
    return 1;
}

int daBasabasa_c::EnterDrop() {
    mVertAccel = -0x1000;
    mTerminalVelocity = -0xa000;
    mIsFlying = 0;
    mAnmModel2.speed = 0;
    mAnmModel2.SetAnim((BCA_File *)data_ov065_0211d6a0.filePtr, 0, 0x1000, 0);
    Vector3 vec(mPos.x, mPos.y + 0x64000, mPos.z);
    mStateTimer = 25;
    if (mPos.y == 0x898000)
        mStateTimer = 50;
    func_02012694(0xf9, &mCamSpacePos);
    return 1;
}

int daBasabasa_c::ExecutePerch() {
    daPly_c *p = ClosestNonVanishPlayer();
    if (p) {
        Vector3 v;
        /* One base pointer: reading p->mPos.x and so on directly compiles differently. */
        int *q = &p->mPos.x;
        v.x = q[0];
        v.y = q[1];
        v.z = q[2];
        if (Vec3_Dist(&mPos, &v) < 0x3e8000) {
            mStateTimer = 20;
            SetState(&data_ov065_0211d710);
        }
    }
    return 1;
}

int daBasabasa_c::EnterPerch() {
    mFlags = 3;
    mIsFlying = 0;
    mAnmModel2.speed = 0x1000;
    mAnmModel2.SetAnim((BCA_File *)data_ov065_0211d6a0.filePtr, 0, 0x1000, 0);
    return 1;
}

int daBasabasa_c::SetState(State *state) {
    mCurrentState = state;
    if (mCurrentState->enter == 0)
        return 1;
    return (this->*mCurrentState->enter)();
}

/* Places both models and the drop shadow at the current position. */
void daBasabasa_c::UpdateModelTransform() {
    Vector3 v;
    Vec3_Asr(&v, &mPos, 3);
    Matrix4x3_FromTranslation(&gScratchMtx, v.x, v.y, v.z);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&gScratchMtx, mAngle.x, mAngle.y, mAngle.z);
    if (mIsFlying == 1) {
        mAnmModel1.mat4x3 = gScratchMtx;
    } else {
        mAnmModel2.mat4x3 = gScratchMtx;
    }
    Matrix4x3_FromTranslation(&gScratchMtx, mPos.x >> 3, (mPos.y - 0x18000) >> 3, mPos.z >> 3);
    mShadowMatrix = gScratchMtx;
    DropShadowRadHeight(mShadowModel, mShadowMatrix, 0x32000, 0x258000, 0xf);
}

/* Releases the four files InitResources claimed. */
int daBasabasa_c::CleanupResources() {
    data_ov065_0211d698.Release();
    data_ov065_0211d6a8.Release();
    data_ov065_0211d690.Release();
    data_ov065_0211d6a0.Release();
    return 1;
}

void daBasabasa_c::OnPendingDestroy() {}

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
    DecIfAbove0_Short(&mStateTimer);
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
        /* Gravity, clamped at terminal velocity. mSpeed.z is reloaded and
           stored back unchanged, as in the ROM. */
        int fallSpeed = mSpeed.y + mVertAccel;
        int clamped = mTerminalVelocity;
        if (fallSpeed >= clamped)
            clamped = fallSpeed;
        int keep = mSpeed.z;
        mSpeed.y = clamped;
        mSpeed.z = keep;
    }
    UpdatePosWithOnlySpeed(&mdCcAc_c);
    UpdateWMClsn(mWithMeshClsn, 0);
    mAngle.x = mPrevAngle.x;
    mAngle.y = mPrevAngle.y;
    mAngle.z = mPrevAngle.z;
    UpdateModelTransform();
    if (mIsFlying == 1) {
        CheckTouch();
    }
    mdCcAc_c.Clear();
    {
        daPly_c *p = ClosestPlayer();
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

/* Both dExtAnmModel_c are loaded here, the other half of the evidence that 0x300
 * and 0x364 are two members. */
int daBasabasa_c::InitResources() {
    mAnmModel1.SetFile(dExtSimpleModel_c::LoadFile(data_ov065_0211d698), 1, -1);
    mAnmModel2.SetFile(dExtSimpleModel_c::LoadFile(data_ov065_0211d6a8), 1, -1);
    mShadowModel.InitCylinder();
    dExtFrameCtrl_c::LoadFile(data_ov065_0211d690);
    dExtFrameCtrl_c::LoadFile(data_ov065_0211d6a0);
    mTerminalVelocity = -0xa000;
    mdCcAc_c.Init(this, 0x28000, 0x28000, 0x200000, 0x7eff0);
    mAngle.y = mPrevAngle.y;
    mPrevAngle.x = -0x8000;
    mAngle.x = mPrevAngle.x;
    mIsFlying = 0;
    mAnmModel1.speed = 0x1000;
    mAnmModel2.speed = 0x1000;
    mWithMeshClsn.Init(this, 0x28000, 0x1e000, 0, 0);
    mHomePos.x = mPos.x;
    mHomePos.y = mPos.y;
    mHomePos.z = mPos.z;
    mCoinType = 1;
    SetState(&data_ov065_0211d700);
    return 1;
}

s32 daBasabasa_c::OnAimedAtWithEgg() {
    return 0;
}

void daBasabasa_c::OnTurnIntoEgg(daPly_c &player) {
    GivePlayerCoins(player, (unsigned char)(mExtraCoinCount + 1), 0);
    KillAndTrackInDeathTable();
}

s32 daBasabasa_c::OnYoshiTryEat() {
    return 4;
}

daBasabasa_c *daBasabasa_c_classInit() {
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
