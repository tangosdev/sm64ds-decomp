//cpp
/* Dorrie (DOSSY) and the cap she wears (DOSSY_CAP), plus the platform
 * element of mPlatforms. ov065 0x02117f40..0x021196d8, 29 functions.
 *
 * _ZTS9daDossy_c 0x0211cd34, _ZTS12daDossyCap_c 0x0211cd58, each naming
 * _ZTI8dActor_c. _ZTV9daDossy_c 0x0211ce48, _ZTV12daDossyCap_c 0x0211cdc4.
 * Both destructors are inline, so each D1/D0 pair lands at the bottom of
 * the run and InitResources is the key function. Highest address first.
 *
 * The platform loops index mPlatforms[i] and compile under the default
 * optimisation: the 0x200-stride pointers the ROM walks are strength
 * reduction's, not the source's.
 */
// leftovers:
// - dExtAnmModel_c::SetAnim and the other Fix12<int>-by-value calls stay
//   mangled. A member SetAnim with Fix12<int> speed in func_ov065_021182e4
//   changes that function.
// - These three helpers stay as address-named free-function seams. The
//   original ownership and names are unproven; func_ov065_02119210 is typed
//   with its caller's daDossyCap_c * view.
// - func_ov065_02118838 reads each bone's angles z, y, x into a Vector3s
//   before storing them x, y, z: assigning rot.x/y/z straight from the bone
//   is 9 words off.
// - daDossyCap_c::Render tests the cap-icon flag as `(flags << 30) >> 31`.
//   `>> 1 & 1` selects asr/ands instead of lsl/lsrs.
// - data_ov065_* keep their address names, including the state table
//   data_ov065_0211d7fc.
// - func_ov065_021195bc and func_ov065_021195d0 are veneers. The first
//   argument is not the actor: each veneer calls the method on its second
//   argument, passing the third.
// - The BOOL/TRUE/FALSE spelling in func_ov065_02119594 and
//   func_ov065_0211956c stays. `bool isPlayer = other->actorID == 0xbf`
//   shrinks each function 0x28 to 0x1c: the ROM keeps the ternary 0/1
//   materialization plus the `cmp r2, #0` re-test. Removing the typedef
//   also renumbers this TU's @NNN temps (manifest rename @1027).

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daDossy_c_AssetId {
    ASSET_DOSSY_BMD = 0x02e9,
    ASSET_DOSSY_ARM_L_KCL = 0x02ea,
    ASSET_DOSSY_ARM_R_KCL = 0x02eb,
    ASSET_DOSSY_BODY_KCL = 0x02ec,
    ASSET_DOSSY_HEAD_KCL = 0x02ed,
    ASSET_DOSSY_HEAD_DOWN_BCA = 0x02ee,
    ASSET_DOSSY_HEAD_UP_BCA = 0x02ef,
    ASSET_DOSSY_LEG_L_KCL = 0x02f0,
    ASSET_DOSSY_LEG_R_KCL = 0x02f1,
    ASSET_DOSSY_NECK_KCL = 0x02f2,
    ASSET_DOSSY_SWIM_BCA = 0x02f3
};

#include "common.h"
#include "daDossy_c.h"
#include "daDossyCap_c.h"
#include "dBgW.h"
#include "SharedFilePtr.h"
#include "daPly_c.h"
#include "Vector3s.h"
#include "nitro/fx/fx.h"
#include "Sound.h"
#include "math/Approach.h"
#include "math/CalcLib.h"
#include "math/MtxStack.h"
#include "nitro/fx/fx_trig.h"
namespace cstd {
s16 atan2(s32, s32);
}

typedef enum { FALSE = 0, TRUE = 1 } BOOL;

/* The spawned daObjMarioCap_c (actor 0x10d) seen only as far as
   dEnemyBase_c::mEatingPlayer, which daDossyCap_c::Behavior hands over.
   Including dEnemyBase_c.h instead renumbers this TU's @NNN pins by 26. */
struct MarioCapEatView : dActor_c {
    s32 mEatingPlayer; /* 0x0d0 */
};

#ifndef SM64DS_PLATFORM_PC
typedef char MarioCapEatView_size_must_be_0xd4[sizeof(MarioCapEatView) == 0xd4 ? 1 : -1];
#endif

/* The resource handles as the ROM lays them out: {id, loaded file}. The
   model handle constructs through func_02017acc and destroys through
   func_02017ab4; the animation handles construct through
   SharedFilePtr::Construct and destroy through SharedFilePtr_Destruct_Anim;
   the collision handles construct through func_02017b4c and destroy through
   SharedFilePtr_Destruct_Clsn. The manifest aliases each undefined member
   onto its ROM symbol. */
struct DossyModelFilePtr : SharedFilePtr {
    DossyModelFilePtr(u32 fileID);
    ~DossyModelFilePtr();
};

struct DossyAnimationFileHandle : SharedFilePtr {
    DossyAnimationFileHandle(u32 fileID);
    ~DossyAnimationFileHandle();
};

struct DossyCollisionFileHandle : SharedFilePtr {
    DossyCollisionFileHandle(u32 fileID);
    ~DossyCollisionFileHandle();
};

extern DossyModelFilePtr data_ov065_0211d720;
extern DossyAnimationFileHandle data_ov065_0211d768;
extern DossyAnimationFileHandle data_ov065_0211d748;
extern DossyAnimationFileHandle data_ov065_0211d770;
extern DossyCollisionFileHandle gDossyArmLCollisionFile;
extern DossyCollisionFileHandle gDossyArmRCollisionFile;
extern DossyCollisionFileHandle gDossyLegLCollisionFile;
extern DossyCollisionFileHandle gDossyLegRCollisionFile;
extern DossyCollisionFileHandle gDossyNeckCollisionFile;
extern DossyCollisionFileHandle gDossyBodyCollisionFile;
extern DossyCollisionFileHandle gDossyHeadCollisionFile;

extern "C" {
extern u8 data_ov065_0211c078[];
extern void *data_ov065_0211cd68[];
extern SharedFilePtr data_ov002_0210d9c0;
extern SharedFilePtr *data_ov065_0211c080[];
extern SharedFilePtr *data_ov065_0211c08c[];

int *Vec3_LslInPlace(Vector3 *v, s32 sh);
extern int AngleDiff(int a, int b);
}
int func_ov065_021180d4(daDossyCap_c *cap);
void func_ov065_02118c4c(daDossyCap_c *cap);
int func_ov065_02119210(daDossyCap_c *cap);
void func_ov065_021195bc(dBgW *collider, dActor_c *owner, dActor_c *other);
void func_ov065_021195d0(dBgW *collider, dActor_c *owner, dActor_c *other);

typedef void (daDossy_c::*StateFunc)();
extern "C" StateFunc data_ov065_0211d7fc[];

DorriePlatform::DorriePlatform()
{
}

daDossy_c *daDossy_c_classInit(void) {
    return new daDossy_c();
}

daDossyCap_c *daDossyCap_c_classInit(void) {
    return new daDossyCap_c();
}

int daDossyCap_c::OnYoshiTryEat()
{
    return 4;
}

#pragma long_calls on
void func_ov065_021195d0(dBgW *collider, dActor_c *owner, dActor_c *other) {
    static_cast<daDossy_c *>(owner)->func_ov065_02119594(other);
}
#pragma long_calls off

#pragma long_calls on
void func_ov065_021195bc(dBgW *collider, dActor_c *owner, dActor_c *other) {
    static_cast<daDossy_c *>(owner)->func_ov065_0211956c(other);
}
#pragma long_calls off

void daDossy_c::func_ov065_02119594(dActor_c *other)
{
    BOOL isPlayer = (other->actorID == 0xbf) ? TRUE : FALSE;
    if (isPlayer) {
        mClsnState = 1;
        mClsnPlayer = other;
    }
}

void daDossy_c::func_ov065_0211956c(dActor_c *other)
{
    BOOL isPlayer = (other->actorID == 0xbf) ? TRUE : FALSE;
    if (isPlayer) {
        mClsnState = 2;
        mClsnPlayer = other;
    }
}

int daDossyCap_c::InitResources()
{
    mModel.SetFile((BMD_File *)data_ov002_0210d9c0.filePtr, 1, -1);
    mCapIcon.Link((char *)this, 2, 2, 0);
    mdCcAc_c.Init(this, 0x96000, 0x96000, 0x100002, 0x8000);
    mCarrier = 0;
    return 1;
}

int daDossy_c::InitResources()
{
    int i;
    int j;
    BMD_File *f = dExtSimpleModel_c::LoadFile(data_ov065_0211d720);
    mAnmModel.SetFile(f, 1, -1);
    dExtSimpleModel_c::LoadFile(data_ov002_0210d9c0);
    for (i = 0; i < 3; i++)
        dExtFrameCtrl_c::LoadFile(*data_ov065_0211c080[i]);
    mAnmModel.SetAnim((BCA_File *)data_ov065_0211d770.filePtr, 0, 0x1000, 0);
    func_ov065_02118cc4();
    func_ov065_02118838();

    for (j = 0; j < 7; j++) {
        KCL_File *kcl = dBgW_Kc::LoadFile(*data_ov065_0211c08c[j]);
        mPlatforms[j].mClsn.dBgW_KcMbg::SetFile(kcl, mPlatforms[j].mClsnNextMat, 0x1000, mAngleY,
                                                *(CLPS_Block *)data_ov065_0211cd68[j]);
        if ((unsigned)j <= 2) {
            mPlatforms[j].mClsn.SetBeforeClsnCallback(&dBgW::UpdatePosAndAngs);
            if (j == 2)
                mPlatforms[j].mClsn.SetStoodOnCallback(func_ov065_021195bc);
            else
                mPlatforms[j].mClsn.SetStoodOnCallback(func_ov065_021195d0);
        }
        mPlatforms[j].mClsn.Enable(this);
    }

    mSpawnPos = mPos;
    mPosX += 0x7d0000;
    mWithMeshClsn.Init(this, 0x1e0000, 0xa0000, 0, 0);
    mCylClsn1.Init(this, 0xdc000, 0xfa000, 2, 0x20);
    Vector3 offset;
    offset.x = 0;
    offset.y = 0x50000;
    offset.z = 0x150000;
    mCylClsn2.Init(this, offset, 0x60000, 0x1b0000, 2, 0x400000);
    mHomePos = mPos;
    mStateTimer = 0;
    mClsnState = 0;
    mClsnPlayer = 0;
    mRider = 0;
    mPushDownHeight = mSinkHeight = 0;
    if ((param1 & 0xff) == 1)
        mHasCap = 1;
    else
        mHasCap = 0;
    mCap = 0;
    if (mHasCap != 0) {
        mCapPos.x = 0;
        mCapPos.y = 0;
        mCapPos.z = 0;
        func_ov065_02118838();
        daDossyCap_c *cap = (daDossyCap_c *)dActor_c::Spawn(0xa9, 0, mPos, &mAngle, mAreaId, -1);
        if (cap != 0) {
            mCap = cap;
            cap->mCarrier = this;
        } else {
            mHasCap = 0;
        }
    }
    return 1;
}

int func_ov065_02119210(daDossyCap_c *cap) {
    int flags = cap->mCapIcon.mFlags << 30;
    return ((unsigned)flags >> 31) != 0;
}

int daDossyCap_c::Behavior()
{
    daDossy_c *carrier = mCarrier;
    if (carrier == 0)
        return 1;
    int eaten = (mFlags & 0x20000) != 0;
    if (eaten != 0) {
        dActor_c *spit = dActor_c::Spawn(0x10d, 0x1210, mPos, &mAngle, mAreaId, -1);
        if (spit != 0) {
            ((MarioCapEatView *)spit)->mEatingPlayer = mEatingPlayer;
            ((daPly_c *)mEatingPlayer)->mObjInMouth = spit;
            spit->mFlags |= 0x20000;
            mFlags &= ~0x20000;
            mEatingPlayer = 0;
            mCapIcon.Unlink();
            mCapIcon.Link((char *)this, 2, 0, 0);
        }
        return 1;
    }
    Vector3 *capPos = &carrier->mCapPos;
    mPos = *capPos;
    mAngleX = mCarrier->mHeadRotX;
    mAngleY = mCarrier->mAngleY;
    unsigned int flags = mCapIcon.mFlags;
    if ((flags << 30) >> 31) {
        func_ov065_02118c4c(this);
        if (func_ov065_021180d4(this) != 0) {
            mdCcAc_c.Clear();
            return 1;
        }
        mdCcAc_c.Clear();
        mdCcAc_c.Update();
    }
    return 1;
}

int daDossy_c::Behavior()
{
    s16 offCenter;
    int outer;
    int inner;
    int neck;
    int neckReach;

    if (mClsnState != 0)
        ApproachLinear(mSinkHeight, 0xa000, 0x1000);
    else
        ApproachLinear(mSinkHeight, 0, 0x1000);

    (this->*data_ov065_0211d7fc[mState])();

    mDistToCenter = Vec3_HorzDist(&mPos, &mSpawnPos);
    mAngToCenter = Vec3_HorzAngle(&mPos, &mSpawnPos);

    offCenter = AngleDiff(mAngToCenter, mAngleY);

    /* Her neck's horizontal reach pushes out the inner edge of the ring
       she swims in, or the outer edge once offCenter is 0x4000 or more. */
    neck = FX_CosIdx((u16)offCenter >> 4) * 0x190;
    neckReach = FX_MUL(neck, FX_SinIdx((u16)mPlatforms[1].mRot.x >> 4));
    if (offCenter < 0x4000) {
        inner = neckReach + 0x5f8000;
        outer = 0x97c000;
    } else {
        inner = 0x5f8000;
        outer = neckReach + 0x97c000;
    }

    if (mDistToCenter >= outer) {
        mPosX = mSpawnPos.x - FX_MUL(outer, FX_SinIdx((u16)mAngToCenter >> 4));
        mPosZ = mSpawnPos.z - FX_MUL(outer, FX_CosIdx((u16)mAngToCenter >> 4));
    } else if (mDistToCenter <= inner) {
        mPosX = mSpawnPos.x - FX_MUL(inner, FX_SinIdx((u16)mAngToCenter >> 4));
        mPosZ = mSpawnPos.z - FX_MUL(inner, FX_CosIdx((u16)mAngToCenter >> 4));
    }

    UpdatePos(0);
    dBgCh_Actr_UpdateContinuous_Veneer(&mWithMeshClsn);
    mPosY = mSpawnPos.y - mSinkHeight - mPushDownHeight;
    mAnmModel.Advance();
    func_ov065_02118cc4();
    func_ov065_02118838();
    func_ov065_02118248();
    mClsnState = 0;
    mClsnPlayer = 0;
    return 1;
}

int daDossyCap_c::Render()
{
    unsigned int flags = mCapIcon.mFlags;
    if ((flags << 30) >> 31) {
        Vector3 scale;
        scale.x = 0x2c00;
        scale.y = 0x2c00;
        scale.z = 0x2c00;
        mModel.Render(&scale);
    }
    return 1;
}

int daDossy_c::Render()
{
    mAnmModel.Render(0);
    return 1;
}

int daDossy_c::CleanupResources()
{
    int i;
    data_ov002_0210d9c0.Release();
    for (i = 0; i < 7; i++) {
        mPlatforms[i].mClsn.Disable();
        data_ov065_0211c08c[i]->Release();
    }
    for (i = 0; i < 3; i++)
        data_ov065_0211c080[i]->Release();
    data_ov065_0211d720.Release();
    return 1;
}

void daDossy_c::func_ov065_02118cc4()
{
    Matrix4x3_FromRotationY(&mAnmModel.mat4x3, mAngleY);
    mAnmModel.mat4x3.m[9] = mPosX >> 3;
    mAnmModel.mat4x3.m[10] = mPosY >> 3;
    mAnmModel.mat4x3.m[11] = mPosZ >> 3;
}

void func_ov065_02118c4c(daDossyCap_c *cap) {
    Vector3 pos;
    Vec3_Asr(&pos, &cap->mPos, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, pos.x, pos.y, pos.z);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, cap->mAngleY);
    Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, cap->mAngleX);
    cap->mModel.mat4x3 = data_020a0e68;
}

void daDossy_c::func_ov065_02118838() {
    Vector3s rot;
    Vector3 v;
    Matrix4x3 base;
    s32 i;

    Matrix4x3_FromTranslation(&data_020a0e68, mPosX, mPosY, mPosZ);
    Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, mAngleY);
    base = data_020a0e68;
    for (i = 0; i < 7; i++) {
        ModelBone *bone = &mAnmModel.data.bones[data_ov065_0211c078[i]];
        {
            u16 tz = bone->rotZ;
            u16 ty = bone->rotY;
            u16 tx = bone->rotX;
            rot.x = tx;
            rot.y = ty;
            rot.z = tz;
        }
        mPlatforms[i].mRot = rot;
        if (i == 2) {
            mPlatforms[i].mRot.x += mPlatforms[1].mRot.x;
            mPlatforms[i].mRot.y += mPlatforms[1].mRot.y;
            mPlatforms[i].mRot.z += mPlatforms[1].mRot.z;
        }
        v.x = 0;
        v.y = 0;
        v.z = 0;
        data_020a0e68 = base;
        MTX_Concat43(&mAnmModel.data.transforms[data_ov065_0211c078[i]], &data_020a0e68,
                     &data_020a0e68);
        v.x = data_020a0e68.m[9];
        v.y = data_020a0e68.m[10];
        v.z = data_020a0e68.m[11];
        VEC_Subtract(&v, &mPos, &v);
        Vec3_LslInPlace(&v, 3);
        VEC_Add(&v, &mPos, &v);
        if (i == 2) {
            s32 scaled;
            mHeadRotX = mPlatforms[2].mRot.x;
            mHomePos = v;
            scaled = FX_SinIdx((u16)mHeadRotX >> 4) * 0x96;
            mCapPos = mHomePos;
            mCapPos.x += FX_MUL(scaled, FX_SinIdx((u16)mAngleY >> 4));
            mCapPos.y += 0x8c000 - FX_SinIdx((u16)mHeadRotX >> 4) * 0x1e;
            mCapPos.z += FX_MUL(scaled, FX_CosIdx((u16)mAngleY >> 4));
            mHomePos.x += FX_SinIdx((u16)mAngleY >> 4) * 0x82;
            mHomePos.y += 0x50000;
            mHomePos.z += FX_CosIdx((u16)mAngleY >> 4) * 0x82;
        }
        Matrix4x3_FromTranslation(&data_020a0e68, v.x, v.y, v.z);
        Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, mAngleY + mPlatforms[i].mRot.y);
        Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, mPlatforms[i].mRot.x);
        Matrix4x3_ApplyInPlaceToRotationZ(&data_020a0e68, mPlatforms[i].mRot.z);
        mPlatforms[i].mClsnNextMat = data_020a0e68;
        mPlatforms[i].mClsn.Transform(mPlatforms[i].mClsnNextMat, mAngleY);
    }
}

void daDossy_c::func_ov065_02118634()
{
    int landed = 0;

    if (mCylClsn2.otherOwner != 0) {
        daPly_c *player = (daPly_c *)dActor_c::FindWithID(mCylClsn2.otherOwner);
        if (player != 0 && player->mIsAirborne == 0)
            landed = 1;
    }

    if (mCylClsn1.otherOwner != 0 || landed != 0) {
        mState++;
        mHorzSpeed = 0;
        mAngVelY = 0;
        mAnmModel.SetAnim((BCA_File *)data_ov065_0211d768.filePtr, 0x40000000, 0x1000, 0);
        mCylClsn1.Clear();
        mCylClsn2.Clear();
        mStateTimer = 0;
        mPushDownHeight = mStateState = 0;
        func_0201267c(0xe6, &mCamSpacePos);
    } else {
        s16 target;
        int speed;
        dActor_c *player = mClsnPlayer;
        if (player != 0) {
            target = player->mAngleY;
            speed = 0x7000;
        } else {
            s16 a = cstd::atan2(0x7d0000, mDistToCenter - 0x7d0000);
            if ((s16)(mAngleY - mAngToCenter) < 0)
                a = -a;
            target = mAngToCenter + a;
            speed = 0x3800;
        }
        ApproachLinear(mHorzSpeed, speed, 0x400);

        ApproachLinear(mAngVelY, (s16)(target - mAngleY) / 100, 5);

        mAngleY += mAngVelY;
        mPrevAngleY = mAngleY;

        mCylClsn1.Clear();
        mCylClsn2.Clear();
        mCylClsn1.Update();
        Vector3 offset;
        offset.x = 0;
        offset.y = 0x50000;
        offset.z = 0x150000;
        mCylClsn2.SetPosRelativeToActor(offset);
        mCylClsn2.Update();
    }
}

void daDossy_c::func_ov065_021183c8()
{
    Vector3 playerPos;

    if (mStateState == 0) {
        mPushDownHeight += 0xa000;
        if (mPushDownHeight >= 0x64000) {
            mPushDownHeight = 0x64000;
            mStateState++;
        }
    } else if (mStateState == 1) {
        mPushDownHeight -= 0x5000;
        if (mPushDownHeight <= 0) {
            mPushDownHeight = 0;
            mStateState++;
        }
    }

    if (mAnmModel.Finished() == 0)
        return;

    mStateTimer++;
    DecIfAbove0_Short(&mRideCooldown);

    if (mClsnPlayer != 0) {
        if (mClsnState == 2 && mRideCooldown == 0) {
            if (mCap == 0 || func_ov065_02119210(mCap) == 0) {
                playerPos = mClsnPlayer->mPos;
                if (playerPos.y >= mHomePos.y && Vec3_HorzDist(&playerPos, &mHomePos) < 0x88000 &&
                    ((daPly_c *)mClsnPlayer)->SetNoControlState(5, -1, 0) != 0) {
                    mState++;
                    mStateState = 0;
                    mAnmModel.SetAnim((BCA_File *)data_ov065_0211d748.filePtr, 0x40000000, 0x1000,
                                      0);
                    mRider = mClsnPlayer;
                }
            }
        }
        if (Vec3_HorzDist(&mPos, &mClsnPlayer->mPos) < 0x1f4000)
            mStateTimer = 0;
    } else {
        if (mStateTimer > 0x96) {
            mState++;
            mStateState = 0;
            mAnmModel.SetAnim((BCA_File *)data_ov065_0211d748.filePtr, 0x40000000, 0x1000, 0);
        }
        mStateTimer++;
    }
}

void daDossy_c::func_ov065_021182e4()
{
    switch (mStateState) {
    case 0:
        if (mAnmModel.Finished() == 0)
            return;
        mStateState = 1;
        mAnmModel.SetAnim((BCA_File *)data_ov065_0211d770.filePtr, 0, 0x1000, 0);
        if (mRider == 0) return;
        ((daPly_c *)mRider)->Unk_020ca150(5);
        return;
    case 1:
        if (mRider != 0) {
            if (((daPly_c *)mRider)->Unk_020c9e5c(5) != 0)
                return;
        }
        mState = 0;
        mStateState = 0;
        mRider = 0;
        return;
    }
}

int daDossy_c::func_ov065_02118248()
{
    int i;

    int hidden = (int)((mFlags & 8) != 0);
    if (hidden != 0) {
        for (i = 0; i < 7; i++) {
            if (mPlatforms[i].mClsn.IsEnabled())
                mPlatforms[i].mClsn.Disable();
        }
        return 1;
    }
    for (i = 0; i < 7; i++) {
        if (!mPlatforms[i].mClsn.IsEnabled())
            mPlatforms[i].mClsn.Enable(this);
    }
    return 0;
}

int func_ov065_021180d4(daDossyCap_c *cap) {
    daPly_c *player;
    u32 id;

    /* Single exit for the failure path: an early return per check is
       56 bytes longer. */
    id = cap->mdCcAc_c.otherOwner;
    if (id != 0 && (cap->mdCcAc_c.hitFlags & 0x400000)) {
        player = (daPly_c *)dActor_c::FindWithID(id);
        if (player != 0) {
            int isPlayer = (int)(player->actorID == 0xbf);
            if (isPlayer != 0 && player->mIsAirborne == 0
                && player->Unk_020c9e5c(5) == 0) {
                Vector3 *playerPos = &player->mPos;
                Vector3 pos = *playerPos;
                int idx = (u16)player->mAngleY >> 4;
                int speed = player->mHorzSpeed;

                pos.y += 0x50000;
                pos.x += FX_MUL(speed, FX_SinIdx(idx));
                pos.z += FX_MUL(speed, FX_CosIdx(idx));
                if (dActor_c::Spawn(0x10d, 0x1210, pos, &cap->mAngle, cap->mAreaId, -1) != 0) {
                    cap->mCapIcon.Unlink();
                    cap->mCapIcon.Link((char *)cap, 2, 0, 0);
                    cap->mCarrier->mRideCooldown = 0x1e;
                }
                return 1;
            }
        }
    }
    return 0;
}

DorriePlatform::~DorriePlatform()
{
}

/* The eleven file-scope resource handles, in retail initializer order: the
   model file (func_02017acc / func_02017ab4), three animation files
   (SharedFilePtr::Construct / SharedFilePtr_Destruct_Anim), then seven
   collision files (func_02017b4c / SharedFilePtr_Destruct_Clsn). The
   manifest aliases each wrapper ctor/dtor onto those ROM symbols. */
DossyModelFilePtr data_ov065_0211d720(ASSET_DOSSY_BMD);
DossyAnimationFileHandle data_ov065_0211d768(ASSET_DOSSY_HEAD_DOWN_BCA);
DossyAnimationFileHandle data_ov065_0211d748(ASSET_DOSSY_HEAD_UP_BCA);
DossyAnimationFileHandle data_ov065_0211d770(ASSET_DOSSY_SWIM_BCA);
DossyCollisionFileHandle gDossyArmLCollisionFile(ASSET_DOSSY_ARM_L_KCL);
DossyCollisionFileHandle gDossyArmRCollisionFile(ASSET_DOSSY_ARM_R_KCL);
DossyCollisionFileHandle gDossyLegLCollisionFile(ASSET_DOSSY_LEG_L_KCL);
DossyCollisionFileHandle gDossyLegRCollisionFile(ASSET_DOSSY_LEG_R_KCL);
DossyCollisionFileHandle gDossyNeckCollisionFile(ASSET_DOSSY_NECK_KCL);
DossyCollisionFileHandle gDossyBodyCollisionFile(ASSET_DOSSY_BODY_KCL);
DossyCollisionFileHandle gDossyHeadCollisionFile(ASSET_DOSSY_HEAD_KCL);

/* The state table Behavior dispatches through: three pointer-to-member
   records. Retail stores the records as compiler temporaries in .data
   (0x0211cd2c, 0x0211cd1c, 0x0211cd24) and the live table in .bss. */
extern "C" StateFunc data_ov065_0211d7fc[3] = {
    &daDossy_c::func_ov065_02118634,
    &daDossy_c::func_ov065_021183c8,
    &daDossy_c::func_ov065_021182e4,
};
