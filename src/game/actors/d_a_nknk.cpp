//cpp
/* Koopa Troopa, separable from its shell (ov062/daNknk_c). Both the
 * normal and the small registry profiles construct this same class.
 * Reactions pick a state into mDeathState; the state machine does the rest.
 *
 * _ZTS8daNknk_c 0x0211da5c. _ZTI8daNknk_c 0x0211da68.
 * _ZTV8daNknk_c 0x0211dab4. The out-of-line destructor is the key function:
 * D1 at 0x021174cc, D0 at 0x02117514.
 *
 * Source runs in reverse ROM order (highest address first). Do not reorder.
 */

// leftovers:
// - Particle::System::New / SetSelfDestructFlag, dExtAnmModel_c::SetAnim,
//   dBgCh_Actr::Init, dActor_c::IsTooFarAwayFromPlayer, daPly_c::Bounce /
//   Hurt and Particle::RunningSlidingDustAt stay mangled. Each passes Fix12<int>
//   by value (notes/mwccarm-codegen.md 6az), or the header declaration
//   does not mangle to the ROM symbol. func_0201267c is the bank-3
//   Sound::Play wrapper.
// - GetFloorResult is still called through its local extern (the wall
//   lookup uses the dBgCh_Actr method). Deleting that prototype shifts the
//   static initializer's @NNN temps, which the intact-object manifest pins
//   by name, by one (code bytes otherwise identical).
// - Vec3 / V3 below are local three-int aggregates. inline_fn() returns
//   MATRIX_SCRATCH_PAPER; deleting its definition shifts the same @NNN
//   temps by three (code bytes otherwise identical).
// Inline definitions in Koopa.h, emitted here by the two factories:
// - the `goto tail` chain in func_ov062_02117c98 keeps one shared tail call
//   (func_ov002_020aea30 with &mWithMeshClsn) plus the conditional yaw flip;
//   a structured duplicate of the tail was not verified byte-identical.
/* Runtime asset handles resolved through the overlay 0 file table. */
enum d_a_nknk_AssetId {
    ASSET_NOKONOKO_BMD = 0x03ba,
    ASSET_NOKONOKO_DIVE_BCA = 0x03bb,
    ASSET_NOKONOKO_DIVE_ED_BCA = 0x03bc,
    ASSET_NOKONOKO_DIVE_ST_BCA = 0x03bd,
    ASSET_NOKONOKO_RED_BMD = 0x03c0,
    ASSET_NOKONOKO_RUN_BCA = 0x03c1,
    ASSET_NOKONOKO_WAIT1_BCA = 0x03c2,
    ASSET_NOKONOKO_WAIT2_BCA = 0x03c3,
    ASSET_NOKONOKO_WALK_BCA = 0x03c4,
    ASSET_NOKONOKO_WALK_ED_BCA = 0x03c5,
    ASSET_NOKONOKO_WALK_ST_BCA = 0x03c6
};

#include "Koopa.h"
#include "ParticleIds.h"
#include "common.h"
#include "types.h"
#include "dActor_c.h"
#include "dBgCh_Actr.h"
#include "dExtFrameCtrl_c.h"
#include "decl_dBgCh_Actr.h"
#include "decl_common.h"
#include "decl_Enemy.h"
#include "SharedFilePtr.h"
#include "daPly_c.h"
#include "decl_ActorBase.h"
#include "decl_dCcAc_c.h"
#include "decl_ShadowModel.h"
#include "Particle__System.h"
#include "Particle__Manager.h"
#include "dBgPi.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_trig.h"
#include "Sound.h"
#include "math/Approach.h"
#include "math/CalcLib.h"
#include "private/mtx43.h"
#include "math/MtxStack.h"
#include "LevelRng.h"

/* File-scope resource handles: the ctor/dtor are the ROM's SharedFilePtr
 * veneer pairs (func_02017acc / func_02017ab4 for the two models,
 * SharedFilePtr::Construct / SharedFilePtr_Destruct_Anim for the nine
 * animation handles), spelled through declared-only subclasses so the
 * static initializer names the real entry points. The code reaches them
 * through the data_ov062_0211ced8/cee0/cee8 pointer tables. The model and
 * animation initialization paths both store a u16 file ID at +0, a byte
 * reference count at +2, and a loaded-file pointer at +4. */
struct NknkModelFilePtr : SharedFilePtr {
    NknkModelFilePtr(u32 fileID);
    ~NknkModelFilePtr();
};
struct NknkAnimationFileHandle : SharedFilePtr {
    NknkAnimationFileHandle(u32 fileID);
    ~NknkAnimationFileHandle();
};

extern "C" {
extern int d_a_nknkNewHold(u32 uniqueID, u32 effectID, s32 x, s32 y, s32 z, const void *dir,
                           void *callback);
extern NknkAnimationFileHandle *data_ov062_0211cee8[];
extern int data_ov062_0211cf0c[];
/* The called scalar definition takes u16 startFrame. The unused dExtAnmModel_c
 * member declaration takes Fix12<int>/u32; its measured form grew this helper. */
/* Reconstructed receiver/attacker/nullable collision interface; the original
 * source prototype is not uniquely recoverable from the register traffic. */
}
struct Aea30C;
extern void func_ov002_020aea30(Aea30C *self, dActor_c *attacker, dBgCh_Actr *collision);
extern "C" {

extern void d_a_nknkRunningSlidingDustAtHold(int a, int b, int c);
extern int RandomIntInternal(int *seed);
extern short Vec3_HorzAngle(const Vector3 *a, const Vector3 *b);

extern SharedFilePtr* data_ov062_0211ced8[2];
extern SharedFilePtr* data_ov062_0211cee0[2];

/* Parameter names keep this TU's @NNN. */
typedef void BgchInitAt_(void *self, dActor_c *a, int r, int h, Vector3_16 *p, int q);

extern void Vec3_Asr(Vector3 *d, Vector3 *s, int sh);
inline struct Matrix4x3 *inline_fn()
{
  return &data_020a0e68;
}

extern int SurfaceInfo_TestFlag0x20(int *p); /* CLPS bit 0x20: water */
}

extern "C" {
daNknk_c *daNknk_c_classInit_NOKONOKO()
{
    return new daNknk_c;
}
}

extern "C" {
daNknk_c *daNknk_c_classInit_NOKONOKO_S()
{
    return new daNknk_c;
}
}

s32 daNknk_c::OnYoshiTryEat() {
  if(mModelIndex==0) return 6;
  return 5;
}

/* daNknk_c::OnTurnIntoEgg -- vtable slot 19, verified against ov062 relocs.txt:
 * _ZTV8daNknk_c (0x0211dab4) + 0x4c -> 0x02119628, exactly this placeholder's
 * former address (former name func_ov062_02119628).
 */
void daNknk_c::OnTurnIntoEgg(daPly_c &player) {
    if (mCoinType == 3) {
        if (OnYoshiTryEat() == 6 && !player.IsCollectingCap()) {
            player.RegisterEggCoinCount(0, 0, 1);
        } else {
            GivePlayerCoins(player, 1, 2);
        }
    }
    KillAndTrackInDeathTable();
}

/* daNknk_c::OnAimedAtWithEgg - named from vtable slot identity */
s32 daNknk_c::OnAimedAtWithEgg() {
    unsigned short v = actorID;
    int r;
    if (v == 0xcb) r = 1; else r = 0;
    if (r != 0) r = 0x46000; else r = 0x25800;
    return r;
}

int daNknk_c::InitResources()
{
    int i;
    int r, h;
    int kind;
    BMD_File* f;
    unsigned int isSpecial;

    kind = param1 & 1;
    mModelIndex = kind;

    for (i = 0; i < 9; i++)
        dExtFrameCtrl_c::LoadFile(*data_ov062_0211cee8[i]);

    f = dExtSimpleModel_c::LoadFile(*data_ov062_0211ced8[mModelIndex]);
    if (mAnmModel.SetFile(f, 1, -1) == 0)
        return 0;

    if (mShadowModel.InitCylinder() == 0)
        return 0;

    isSpecial = actorID == 0xcc; /* NOKONOKO_S */
    if (isSpecial)
    {
        mKoopaVariant = 2;
        mAnimSpeed = 0x2000;
        mScaleX = 0x599;
        mScaleY = 0x599;
        mScaleZ = 0x599;
        r = 0x1e000;
        h = 0x32000;
    }
    else
    {
        mKoopaVariant = 0;
        mAnimSpeed = 0x1000;
        mScaleX = 0xa66;
        mScaleY = 0xa66;
        mScaleZ = 0xa66;
        r = 0x3c000;
        h = 0x64000;
        dExtSimpleModel_c::LoadFile(*data_ov062_0211cee0[mModelIndex]);
    }

    mdCc_c.Init(this, r, h, 0x200000, 0xb6efe0);

    mStateTimer = 0;
    mWalkState = 0;
    mState = 1;
    SetAnimByIndex(0);

    mCliffState = 0;
    mInvincibleTimer = 0;
    mHomePosX = mPosX;
    mHomePosY = mPosY;
    mHomePosZ = mPosZ;
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    mWithMeshClsn.Init(this, 0x32000, 0x32000, 0, 0);

    mWithMeshClsn.StartDetectingWater();

    mCoinType = 3;
    LoadBlueCoinModel();

    return 1;
}

int daNknk_c::Behavior()
{
    int state;
    int kind;

    if (UpdateKillByInvincibleChar(mWithMeshClsn, mAnmModel, 3) != 0)
        return 1;

    MakeVanishLuigiWork(mdCc_c);

    if (UpdateYoshiEat(mWithMeshClsn) != 0) {
        int *pb0 = (int *)&mFlags;
        *pb0 = *pb0 & ~0x10000000;
        if (SpawnParticlesIfHitOtherObj(mdCc_c) != 0) {
            PoofDust();
            SpawnBlueCoin();
            KillAndTrackInDeathTable();
        }
        if (mEatenByYoshi != 0)
            KillOnWaterFloor();
        UpdateModelMatrixAndShadow();
        mdCc_c.Clear();
        if (mEatenByYoshi != 0 && mSpitOutTimer == 0)
            mdCc_c.Update();
        if (mKoopaVariant == 1)
            mState = 4;
        else
            mState = 1;
        if (mWithMeshClsn.IsOnGround() != 0) {
            mSafePosX = mPosX;
            mSafePosY = mPosY;
            mSafePosZ = mPosZ;
        }
        return 1;
    }

    if (mDeathState == 0) {
        if (IsTooFarAwayFromPlayer(0x5dc000) != 0)
            return 1;

        {
            int *pb0 = (int *)&mFlags;
            if (mLandingDustTimer != 0)
                mLandingDustTimer -= 1;
            *pb0 = *pb0 | 0x10000000;
        }
        UpdateClosestPlayer(0x3e8000);

        if (mState != 0)
            mAnmModel.Advance();

        kind = mKoopaVariant;
        state = mState;
        switch (kind) {
        case 0:
        case 2:
            func_ov062_02117acc();
            break;
        case 1:
            func_ov062_02117a3c();
            break;
        }

        {
            int ang = mPrevAngleY;
            mAngleY = (s16)ang;
            {
                u16 *p100 = (u16 *)&mStateTimer;
                *p100 = (u16)(*p100 + 1);
            }
        }
        if (state != mState || kind != mKoopaVariant) {
            mStateTimer = 0;
            mWalkState = 0;
        }
        func_ov062_02117c98();
        UpdatePos(&mdCc_c);

        if (mDeathState == 0 && mState != 0) {
            if (IsGoingOffCliff(mWithMeshClsn, 0x32000, 0x3800, false, true, 0x32000) != 0) {
                mPosX = mSafePosX;
                mPosY = mSafePosY;
                mPosZ = mSafePosZ;
            } else {
                mSafePosX = mPosX;
                mSafePosY = mPosY;
                mSafePosZ = mPosZ;
            }
        }

        UpdateWMClsn(mWithMeshClsn, 0);
        KillOnWaterFloor();
        mdCc_c.Clear();
        if (mDeathState == 0) {
            if (mInvincibleTimer == 0) {
                mdCc_c.Update();
            } else {
                mInvincibleTimer -= 1;
            }
        }
    } else {
        UpdateDeath(mWithMeshClsn);
    }

    UpdateModelMatrixAndShadow();
    return 1;
}

int daNknk_c::Render()
{
    int b = (mFlags & 0x40000) != 0;
    if (b)
        return 1;
    if (mKoopaVariant == 1) {
        mAnmModel.ShowMaterial(0, 1);
        mAnmModel.HideMaterial(0, 2);
    } else {
        mAnmModel.HideMaterial(0, 1);
        mAnmModel.ShowMaterial(0, 2);
    }
    Vector3 saved = mScale;
    if (mDeathState == 1 && mKoopaVariant == 2) {
        mScale.x = FX_MUL(mScale.x, 0x800);
        mScale.y = FX_MUL(mScale.y, 0x800);
        mScale.z = FX_MUL(mScale.z, 0x800);
    }
    mAnmModel.Render(&mScale);
    mScale = saved;
    return 1;
}

/* fBase_c slot 12. Empty in the ROM: four bytes, `bx lr`.
 */
void daNknk_c::OnPendingDestroy()
{
}

int daNknk_c::CleanupResources()
{
  int b = actorID == 0xcc; /* NOKONOKO_S has no second model */
  if (b == 0)
  {
    data_ov062_0211cee0[mModelIndex]->Release();
  }
  data_ov062_0211ced8[mModelIndex]->Release();
  for (int i = 0; i < 9; ++i)
  {
    data_ov062_0211cee8[i]->Release();
  }
  UnloadBlueCoinModel();
  return 1;
}

void daNknk_c::func_ov062_02118de8()
{
    mHorzSpeed = 0;
    if (mAnimIndex == 2) {
        if (mAnmModel.Finished() == 0)
            return;
        SetAnimByIndex(4);
        return;
    }
    if (mAnmModel.WillHitFrame((unsigned short)(mAnmModel.GetFrameCount() - 1)) != 0) {
        u16 *hp = &mWalkState;
        *hp = (u16)(*hp + 1);
    } else {
        func_ov062_02118058();
        return;
    }
    SetAnimByIndex(0);
    if (mKoopaVariant == 1)
        mState = 5;
    else
        mState = 2;
    if (((unsigned int)RandomIntInternal(&data_0209e650.value) >> 16) & 0x8000) {
        int r = ((unsigned int)RandomIntInternal(&data_0209e650.value) >> 16) & 0x1fff;
        mTargetAngle = (s16)(mPrevAngleY - r);
    } else {
        int r = ((unsigned int)RandomIntInternal(&data_0209e650.value) >> 16) & 0x1fff;
        mTargetAngle = (s16)(mPrevAngleY + r);
    }
    func_ov062_02118058();
}

void daNknk_c::func_ov062_02118cdc()
{
    if (mTurning) {
        mTurning = (ApproachLinear(mPrevAngleY, mTargetAngle, 0x200) ^ 1) != 0;
    } else {
        if (mPlayerDist >= 0x61a8000) {
            mTargetAngle = mAimAngle;
        }
        mTurning = AngleAwayFromWallOrCliff(mWithMeshClsn, mTargetAngle);
        ApproachLinear(mPrevAngleY, mTargetAngle, 0x200);
    }
    if (mAnimIndex == 1) {
        SpawnFootstepDust(2, 8, 0x13, 0x19);
    }
    switch (mWalkState) {
    case 0:
        UpdateWalkStart();
        break;
    case 1:
        UpdateWalk();
        break;
    case 2:
        UpdateWalkEnd();
        break;
    }
    func_ov062_02118058();
}

void daNknk_c::func_ov062_02118b4c() {
    if (mAnimIndex == 3)
        SpawnFootstepDust(2, 5, 8, 0xb);

    if (mModelIndex != 0) {
        if ((u16)mStateTimer > 0x1e && DistToClosestPlayer() > 0x320000) {
            if (ApproachLinear(mHorzSpeed, 0, 0x1000) == 0)
                return;
            mState = 1;
            SetAnimByIndex(2);
            return;
        }
        ApproachLinear(mPrevAngleY, mAimAngle, 0x400);
        ApproachLinear(mHorzSpeed, 0x11000, 0x1000);
        return;
    }

    if (mPlayerDist >= 0x61a8000) {
        s16 *aim = &mAimAngle;
        *aim = (s16)(*aim + 0x8000);
        mPlayerDist = 0;
    }
    if ((u16)mStateTimer > 0x1e && mPlayerDist > 0x320000) {
        if (ApproachLinear(mHorzSpeed, 0, 0x1000) == 0)
            return;
        mState = 1;
        SetAnimByIndex(2);
        return;
    }
    ApproachLinear(mPrevAngleY, (s16)(mAimAngle + 0x8000), 0x400);
    ApproachLinear(mHorzSpeed, 0x11000, 0x1000);
}

/* The state helpers decrement mTimer and mTurnTimer through a pointer
 * (&mTimer / &mTurnTimer). */
void daNknk_c::func_ov062_02118a50()
{
    if (mHorzSpeed != 0) {
        if (mWithMeshClsn.IsOnWall() != 0) {
            dBgPi *wall = mWithMeshClsn.GetWallResult();
            wall->surface.CopyNormalTo(*(Vector3*)&mWallNormalX);
            mPrevAngleY = ReflectAngle(mWallNormalX, mWallNormalZ, mPrevAngleY);
        }
        SlideToStop(0x4cc);
        return;
    }

    {
        if (mTimer != 0) {
            u16 *p = &mTimer;
            *p = (u16)(*p - 1);
            if (mTimer != 0)
                return;
            SetAnimByIndex(8);
            return;
        }
    }

    if (mAnmModel.WillHitFrame(0x1e) != 0)
        SpawnLandingDust();
    if (mAnmModel.Finished() == 0)
        return;
    mState = 1;
    SetAnimByIndex(2);
}

void daNknk_c::WaitUntilGrounded() {
    int gr = mWithMeshClsn.IsOnGround();
    if (gr == 0) return;
    int v = mKoopaVariant;
    if (v == 1) {
        mState = 5;
    } else {
        mState = 2;
    }
    SetAnimByIndex(0);
    SpawnLandingDust();
}

void daNknk_c::func_ov062_02118718()
{
    int dist = 0x7fffffff;
    dActor_c *other;

    if (mAnimIndex == 3)
        SpawnFootstepDust(2, 5, 8, 0xb);

    if (mTurning) {
        mTurning =
            (ApproachLinear(mPrevAngleY, mTargetAngle, 0x200) ^ 1) != 0;
    } else {
        if (mPlayerDist >= 0x61a8000)
            mTargetAngle = mAimAngle;

        other = FindNearestShell();
        if (other) {
            dist = Vec3_Dist(&mPos, &other->mPos);
            mTargetAngle = Vec3_HorzAngle(&mPos, &other->mPos);
        } else {
            mTurning =
                AngleAwayFromWallOrCliff(mWithMeshClsn, mTargetAngle);
            if (!mTurning) {
                if (mTurnTimer != 0) {
                    u16 *turn = &mTurnTimer;
                    *turn = (u16)(*turn - 1);
                } else {
                    /* (int)& forces the pool add. mTargetAngle -= folds to mAnmModel+0xc2. */
                    if (((unsigned)RandomIntInternal(&data_0209e650.value) >> 16) & 0x8000)
                        *(s16 *)((int)&mTargetAngle) -=
                            ((unsigned)RandomIntInternal(&data_0209e650.value) >> 16) & 0x1fff;
                    else
                        *(s16 *)((int)&mTargetAngle) +=
                            ((unsigned)RandomIntInternal(&data_0209e650.value) >> 16) & 0x1fff;
                    mTurnTimer = 0x14;
                }
            }
        }

        if (mPlayerDist > 0x320000 ||
            (other != 0 &&
             GetSubtraction(mTargetAngle,
                 (short)(mPrevAngleY + 0x8000)) < 0x2000)) {
            ApproachLinear(mPrevAngleY, mTargetAngle, 0x600);
        } else {
            if (mModelIndex != 0)
                ApproachLinear(mPrevAngleY, mAimAngle, 0x600);
            else
                ApproachLinear(mPrevAngleY, (short)(mAimAngle + 0x8000), 0x600);
        }
    }

    if (ApproachLinear(mHorzSpeed, 0x14000, 0x1000) == 0)
        return;
    if (dist >= 0xc8000)
        return;
    if (GetSubtraction(mTargetAngle, mPrevAngleY) >= 0xc00)
        return;

    mPrevAngleY = mTargetAngle;
    mState = 2;
    mHorzSpeed += mHorzSpeed / 5;
    mVertSpeed = dist / 30;
    SetAnimByIndex(6);
    mTimer = 0x14;
}

void daNknk_c::func_ov062_02118588()
{
    dActor_c *found = 0;
    int match = 0;
    if (mState == 2) {
        if (mdCc_c.otherOwner != 0) {
            found = dActor_c::FindWithID(mdCc_c.otherOwner);
            if (found != 0) {
                int t = found->actorID;
                t = (t == 0x11d); /* SHELL */
                if (t != 0)
                    match = 1;
            }
        }
    }
    if (match != 0) {
        mKoopaVariant = 0;
        mState = 4;
        mHorzSpeed = mHorzSpeed / 2;
        found->MarkForDestruction();
        return;
    }
    if (mHorzSpeed != 0) {
        SlideToStop(0x800);
        return;
    }

    {
        if (mTimer != 0) {
            u16 *q = &mTimer;
            *q = (u16)(*q - 1);
            if (mTimer != 0)
                return;
            SetAnimByIndex(8);
            return;
        }
    }

    if (mAnmModel.WillHitFrame(0x1e) != 0)
        SpawnLandingDust();
    if (mAnmModel.Finished() == 0)
        return;
    if (mModelIndex != 0) {
        mState = 5;
        SetAnimByIndex(0);
        return;
    }
    mState = 1;
    SetAnimByIndex(3);
}

void daNknk_c::func_ov062_021183e0()
{
    int dist = 0x7fffffff;
    dActor_c *other;

    if (mAnmModel.Finished())
        SetAnimByIndex(1);

    if (mTurning) {
        mTurning = (ApproachLinear(mPrevAngleY, mTargetAngle, 0x200) ^ 1) != 0;
    } else {
        if (mPlayerDist >= 0x61a8000)
            mTargetAngle = mAimAngle;
        other = FindNearestShell();
        if (other) {
            dist = Vec3_Dist(&mPos, &other->mPos);
            if (dist < 0xc8000)
                mTargetAngle = Vec3_HorzAngle(&mPos, &other->mPos);
        }
        mTurning = AngleAwayFromWallOrCliff(mWithMeshClsn, mTargetAngle);
        ApproachLinear(mPrevAngleY, mTargetAngle, 0x200);
    }

    if (dist < 0xc8000 && GetSubtraction(mTargetAngle, mPrevAngleY) < 0xc00) {
        mPrevAngleY = mTargetAngle;
        mState = 2;
        mHorzSpeed = 0x18000;
        mVertSpeed = dist / 30;
        SetAnimByIndex(6);
        mTimer = 0x14;
        return;
    }

    if (mKoopaVariant == 2)
        ApproachLinear(mHorzSpeed, 0x2000, 0x4cc);
    else
        ApproachLinear(mHorzSpeed, 0x3000, 0x4cc);

    func_ov062_02118058();
}

void daNknk_c::UpdateModelMatrixAndShadow() {
    short *angleY;
    Vector3 v;
    Vec3_Asr(&v, &mPos, 3);
    if (1) {
        Matrix4x3_FromTranslation(&data_020a0e68, v.x, v.y, v.z);
        angleY = &mAngleY;
        Matrix4x3_ApplyInPlaceToRotationY(inline_fn(), *angleY);
        /* Flat copy: assigning Matrix4x3 grows .text by 0x1c bytes. */
        *(Mtx43 *)&mAnmModel.mat4x3 = *(Mtx43 *)&data_020a0e68;
        {
            int b = (int)((mFlags & 0x40000) != 0);
            if (b != 0) {
                return;
            }
        }
    }
    DropShadowRadHeight(mShadowModel, mAnmModel.mat4x3, 0x50000, 0x50000, 0xf);
}

void daNknk_c::UpdateClosestPlayer(int lim) {
    mClosestPlayer = ClosestPlayer();

    if (mClosestPlayer == 0 || Vec3_Dist(&mPos, &mHomePos) > lim) {
        mAimAngle = Vec3_HorzAngle(&mPos, &mHomePos);
        mPlayerDist = 0x61a8000;
    } else {
        Vector3 v;
        int* src = &mClosestPlayer->mPosX;
        v.x = src[0];
        v.y = src[1];
        v.z = src[2];
        if (Vec3_Dist(&mHomePos, &v) > lim) {
            mPlayerDist = 0x4e20000;
            return;
        }
        mPlayerDist = Vec3_Dist(&mPos, &v);
        mAimAngle = Vec3_HorzAngle(&mPos, &v);
    }
}

void daNknk_c::UpdateWalkStart() {
    if (mKoopaVariant == 2) {
        ApproachLinear(mHorzSpeed, 0x2000, 0x4cc);
    } else {
        ApproachLinear(mHorzSpeed, 0x3000, 0x4cc);
    }
    if (mAnmModel.Finished() == 0)
        return;
    {
        u16 *p = &mWalkState;
        *p = (u16)(*p + 1);
    }
    mTimer = (u16)(((unsigned int)RandomIntInternal(&data_0209e650.value) >> 0x10) % 70 + 30);
    SetAnimByIndex(1);
}

void daNknk_c::UpdateWalk() {
    if (mTimer != 0) {
        u16 *p = &mTimer;
        *p = (u16)(*p - 1);
        return;
    }
    if (mAnmModel.WillHitFrame((unsigned short)(mAnmModel.GetFrameCount() - 1)) == 0)
        return;
    {
        u16 *q = &mWalkState;
        *q = (u16)(*q + 1);
    }
    SetAnimByIndex(2);
}

void daNknk_c::UpdateWalkEnd() {
    ApproachLinear(mHorzSpeed, 0, 0x1000);
    int done = mAnmModel.Finished();
    if (!done) return;
    mState = 1;
    SetAnimByIndex(4);
}

void daNknk_c::func_ov062_02118058() {
    daPly_c *o = mClosestPlayer;
    int d = mPlayerDist;
    if (o != 0)
        d = Vec3_Dist(&mPos, &o->mPos);
    if (d >= 0x12c000)
        return;
    int s = GetSubtraction(mAimAngle, mPrevAngleY);
    if (s >= 0x3000)
        return;
    if (mKoopaVariant == 1)
        mState = 1;
    else
        mState = 3;
    SetAnimByIndex(3);
}

/* Signature deliberately copied from the local declaration above: the
   ROM name carries by-value class parameters (e.g. Fix12<int>), which
   mwccarm passes differently at the call site, so declaring the true
   types breaks the byte match. See notes/mwccarm-codegen.md 6az. */
void daNknk_c::SlideToStop(int a1) {
    int r = mWithMeshClsn.IsOnGround();
    if (r == 0) return;
    ApproachLinear(mHorzSpeed, 0, a1);
    int x = mPosX;
    int y = mPosY;
    int z = mPosZ;
    Particle::RunningSlidingDustAt(x, y, z);
}

void daNknk_c::func_ov062_02117c98()
{
    dActor_c *found;
    int turnAround;
    s32 flags;
    u32 id;

    id = mdCc_c.otherOwner;
    if (id == 0)
        return;
    found = dActor_c::FindWithID(id);
    if (found == 0)
        return;

    flags = mdCc_c.hitFlags;
    turnAround = 0;

    if (flags & 0x10) {
        Vector3_16 v;
        v.x = (s16)-0x2000;
        v.y = (s16)turnAround;
        v.z = (s16)turnAround;
        KillByInvincibleChar(v, *(daPly_c *)found, 0x46000);
        return;
    }
    if (flags & 0x22400) {
        if (mKoopaVariant == 0) {
            DropShell();
            return;
        }
        mDeathState = 5;
        func_ov002_020aea30((Aea30C *)this, found, (dBgCh_Actr*)turnAround);
        return;
    }
    if (flags & 0x4000) {
        mDeathState = 6;
        turnAround = 1;
    } else if (flags & 0x447e0) {
        if (mKoopaVariant == 0) {
            DropShell();
            if (flags & 0x3c0)
                mPrevAngleY = found->mAngleY;
        } else {
            if (flags & 0x40040)
                mDeathState = 2;
            else if (flags & 0x20400)
                mDeathState = 5;
            else if (flags & 0x380)
                mDeathState = 3;
            else if (flags & 0x4000)
                mDeathState = 6;
            else {
                mDeathState = 1;
                func_0201267c(0x113, &mCamSpacePos);
                mScaleX = 0x1000;
                mScaleY = 0x1000;
                mScaleZ = 0x1000;
            }
            func_ov002_020aea30((Aea30C *)this, found, 0);
            return;
        }
    } else {
        daPly_c *player = (daPly_c *)found;
        struct {
            VecFx32 sv;
            VecFx32 hv;
        } L;
        int isPlayer;
        isPlayer = (player->actorID == 0xbf /* PLAYER */) ? 1 : turnAround;
        if (isPlayer == 0)
            goto tail;
        if (player->mIsMetal != 0) {
            if (mKoopaVariant == 0) {
                DropShell();
                goto tail;
            }
            mDeathState = 6;
            func_ov002_020aea30((Aea30C *)this, found, 0);
            return;
        }
        {
            s32* s = &player->mPosX;
            L.sv.x = s[0];
            L.sv.y = s[1];
            L.sv.z = s[2];
        }
        if (player->IsOnShell()) {
            mDeathState = 5;
            turnAround = 1;
            goto tail;
        }
        if (JumpedOnByPlayer(mdCc_c, *player)) {
            if (mKoopaVariant == 0) {
                DropShell();
            } else {
                mDeathState = 1;
                func_ov002_020aea30((Aea30C *)this, found, 0);
                mScaleX = 0x1000;
                mScaleY = 0x1000;
                mScaleZ = 0x1000;
                func_0201267c(0x113, &mCamSpacePos);
            }
            ((daPly_c *)found)->Bounce(0x28000);
            return;
        }
        if (player->mIsVanish != 0)
            goto tail;
        if ((mdCc_c.hitFlags & 0x400000) == 0)
            goto tail;
        if (mState == 0)
            goto tail;
        {
            s32 pw;
            L.hv.x = mPosX;
            L.hv.y = mPosY;
            L.hv.z = mPosZ;
            pw = mHorzSpeed;
            if (pw < 0xf000)
                pw = 0xf000;
            player->Hurt(*(Vector3 *)&L.hv, 0, pw, 1, 0, 1);
            BounceOffHurtPlayer();
            if (mKoopaVariant == 2) {
                PoofDust();
                SpawnBlueCoin();
                KillAndTrackInDeathTable();
            }
        }
    }

tail:
    func_ov002_020aea30((Aea30C *)this, found, &mWithMeshClsn);
    if (turnAround)
        mAngleY = (u16)(mPrevAngleY + 0x8000);
}

void daNknk_c::DropShell() {
    func_0201267c(0xed, &mCamSpacePos);
    mKoopaVariant = 1;
    mState = 3;
    mHorzSpeed = 0x14000;
    SetAnimByIndex(7);
    {
        Vector3 v;
        int z = mPosZ;
        int y = mPosY + 0x3c000;
        v.x = mPosX;
        v.y = y;
        v.z = z;
        dActor_c::Spawn(0x11d /* SHELL */, mModelIndex, v, 0, mAreaId, -1);
    }
    mInvincibleTimer = 0xa;
    func_0201267c(1, &mCamSpacePos);
}

dActor_c *daNknk_c::FindNearestShell() {
    dActor_c *found = 0;
    dActor_c *best = 0;
    Fix12i bestDist = 0x7FFFFFFF;
    while (1) {
        found = dActor_c::FindWithActorID(0x11d /* SHELL */, found);
        if (!found) break;
        Fix12i d = Vec3_Dist(&mPos, &found->mPos);
        if (d < bestDist) {
            bestDist = d;
            best = found;
        }
    }
    return best;
}

int daNknk_c::DistToClosestPlayer() {
    daPly_c *target = mClosestPlayer;
    if (!target) return 0x61a8000;
    return Vec3_Dist(&mPos, &target->mPos);
}

void daNknk_c::BounceOffHurtPlayer() {
    mState = 0;
    mVertSpeed = 81920;
    mHorzSpeed = 0;
}

void daNknk_c::func_ov062_02117acc(){
  switch(mState){
  case 0:
      WaitUntilGrounded();
      break;
  case 1: func_ov062_02118de8(); break;
  case 2: func_ov062_02118cdc(); break;
  case 3: func_ov062_02118b4c(); break;
  case 4: func_ov062_02118a50(); break;
  }
}

void daNknk_c::func_ov062_02117a3c()
{
    switch (mState) {
    case 0:
        WaitUntilGrounded();
        break;
    case 1: func_ov062_02118718(); break;
    case 2: func_ov062_02118588(); break;
    case 3: func_ov062_02118588(); break;
    case 4: func_ov062_02118de8(); break;
    case 5: func_ov062_021183e0(); break;
    }
}

void daNknk_c::SpawnBlueCoin() {
    int x = mPosX;
    int z = mPosZ;
    int y = mPosY + 0x78000;
    Vector3 pos;
    pos.x = x;
    pos.y = y;
    pos.z = z;
    dActor_c::Spawn(0x122, 2, pos, 0, mAreaId, -1);
}

void daNknk_c::SetAnimByIndex(int idx) {
    mAnmModel.SetAnim((BCA_File *)data_ov062_0211cee8[idx]->filePtr, data_ov062_0211cf0c[idx],
                      mAnimSpeed, 0);
    mAnimIndex = (u8)idx;
}

// 6f: keep constant live / flip coloring
void daNknk_c::SpawnFootstepDust(
    unsigned int a1, unsigned int a2, unsigned int a3,
    unsigned short a4) /* uxth vs a1-a3: ROM compares h<=a4 after a halfword arg */
{
    unsigned int h = (unsigned int)(mAnmModel.currFrame << 4) >> 16;
    if ((h >= a1 && h <= a2) || (h >= a3 && h <= a4)) {
        Vector3 pos;
        int k, iscb, idx, uid, iscb2;
        short ang;
        Particle::System *ps;
        if (mFootstep != 0) return;
        mFootstep = 1;
        func_0201267c(0xe4, &mCamSpacePos);
        if (mAnimIndex == 1) return;
        ang = (short)(mAngleY + 0x4000);
        pos.x = mPosX;
        pos.y = mPosY;
        pos.z = mPosZ;
        iscb = actorID == 0xcb;
        k = iscb ? 0xf : 0xa;
        pos.y = pos.y + (k << 12);
        if (h <= a2) {
            idx = (unsigned short)ang >> 4;
            pos.x -= k * FX_SinIdx(idx);
            pos.z -= k * FX_CosIdx(idx);
        } else {
            idx = (unsigned short)ang >> 4;
            pos.x += k * FX_SinIdx(idx);
            pos.z += k * FX_CosIdx(idx);
        }
        uid = Particle::System::New(0, kPtclFootstepDust, pos.x, pos.y, pos.z, 0, 0);
        Particle::SetSelfDestructFlag(0xf9);
        if (uid == 0) return;
        ps = Particle::System::FromUniqueID(uid);
        if (ps == 0) return;
        iscb2 = actorID == 0xcb;
        if (iscb2)
            ps->callbackScale = (short)FX_MUL(ps->callbackScale, 0x800);
        else
            ps->callbackScale = (short)FX_MUL(ps->callbackScale, 0x500);
    } else {
        mFootstep = 0;
    }
}

void daNknk_c::SpawnLandingDust() {
    volatile Vector3 pos;
    int t;
    u32 spawned;
    Particle::System *particle;

    if (mLandingDustTimer != 0) return;

    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;

    t = actorID;
    t = t == 0xcb;
    {
        int zArg = pos.z;
        if (t != 0) {
            pos.y = pos.y + 0xf000;
        } else {
            pos.y = pos.y + 0xa000;
        }

        spawned = Particle::System::New(0, kPtclLandingDust, pos.x, pos.y, zArg,
                                        (const Vector3_16f *)((void *)0),
                                        (dPa_c::level_c::callback_c *)((void *)0));
    }

    Particle::SetSelfDestructFlag(kPtclLandingDust);

    mLandingDustTimer = 0xa;
    if (spawned == 0) return;

    particle = Particle::System::FromUniqueID(spawned);
    if (particle == 0) return;

    {
        int t2 = actorID;
        t2 = t2 == 0xcb;
        if (t2 != 0) {
            int v = particle->callbackScale;
            int r = FX_MUL(v, 0x800);
            particle->callbackScale = (short)r;
        } else {
            int v = particle->callbackScale;
            int r = FX_MUL(v, 0x500);
            particle->callbackScale = (short)r;
        }
    }
}

/* Standing on a water floor: drop a coin and die. */
void daNknk_c::KillOnWaterFloor() {
    if (!mWithMeshClsn.IsOnGround()) return;
    dBgPi *floor = mWithMeshClsn.GetFloorResult();
    if (!SurfaceInfo_TestFlag0x20((int *)&floor->surface)) return;
    SpawnCoin();
    KillAndTrackInDeathTable();
}

/* Two model handles (param1 & 1 selects the variant through
 * data_ov062_0211ced8) and nine animation handles (the shared noko-noko
 * clip set indexed by data_ov062_0211cee8). */
NknkModelFilePtr gNokonokoModelFile(ASSET_NOKONOKO_BMD);
NknkModelFilePtr gNokonokoRedModelFile(ASSET_NOKONOKO_RED_BMD);
NknkAnimationFileHandle gNokonokoWalkStAnimationFile(ASSET_NOKONOKO_WALK_ST_BCA);
NknkAnimationFileHandle gNokonokoWalkAnimationFile(ASSET_NOKONOKO_WALK_BCA);
NknkAnimationFileHandle gNokonokoWalkEdAnimationFile(ASSET_NOKONOKO_WALK_ED_BCA);
NknkAnimationFileHandle gNokonokoRunAnimationFile(ASSET_NOKONOKO_RUN_BCA);
NknkAnimationFileHandle gNokonokoWait1AnimationFile(ASSET_NOKONOKO_WAIT1_BCA);
NknkAnimationFileHandle gNokonokoWait2AnimationFile(ASSET_NOKONOKO_WAIT2_BCA);
NknkAnimationFileHandle gNokonokoDiveStAnimationFile(ASSET_NOKONOKO_DIVE_ST_BCA);
NknkAnimationFileHandle gNokonokoDiveAnimationFile(ASSET_NOKONOKO_DIVE_BCA);
NknkAnimationFileHandle gNokonokoDiveEdAnimationFile(ASSET_NOKONOKO_DIVE_ED_BCA);
