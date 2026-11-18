//cpp
/* daBtfly_c -- butterfly (actor 0x150) on ov100. Kind 0 is the spawner:
 * State4 releases three more of actor 0x150, and kind 1 turns into a 1-Up
 * (actor 0x114) in State6. Behavior dispatches State0..State7 through
 * data_ov100_02148628, a pointer-to-member table.
 *
 * `#pragma defer_codegen off` is load-bearing: it emits .text in source
 * order, so this file is ROM-ascending: D1, D0, State7..State0, CleanupResources,
 * OnPendingDestroy, Render, Behavior, InitResources, classInit
 * (0x02140d80..0x02141f04). Do not reorder the functions without the pragma.
 *
 * One ~daBtfly_c() emits D1 and D0 together. A second hand-mangled D0 beside
 * it is the mwccarm ICE (ELFgen.c:483).
 *
 * daBtfly_c_classInit (0x02141ea4..0x02141f04, historical alias
 * Butterfly_Spawn) appends after InitResources, at the end of source order:
 * the pragma above never toggles back on, so it stays in effect for this
 * new function too. fBase_c::operator new(size_t) forwards
 * `return new daBtfly_c();` to the same _ZN7fBase_cnwEm(1012) allocator the
 * loose factory called by hand, and daBtfly_c has no user-declared
 * constructor, so the inherited dActor_c ctor plus the vtable store plus the
 * six member subobjects in field order (dExtAnmModel_c, dExtSimpleModel_c, dExtShadowModel_c x2,
 * dBgCh_Actr, dCcAcPos_c) come from the implicit default constructor with
 * zero mangled calls.
 *
 * deslop leftovers:
 * - State7 mFlutterPhase is a u16: the adds step it as one (ldrh/strh).
 *   The phase load is ldrsh plus a zero-extend shift, so the read goes
 *   through an s16 local (ldrh of the field is 8 bytes shorter) (#821 r82).
 * - State7 UpdateContinuous(): WRONG-DEST. ROM calls
 *   dBgCh_Actr_UpdateContinuous_Veneer (0x020383fc).
 * - State7 func_02035638 is mClsnFlags & 0x10. No method; IsOnGround is
 *   mFlags & 0x10.
 * - State0: building the angle triple before reading param1 differs by
 *   10 words.
 * - daBtfly_c_classInit and g_profile_BUTTERFLY sit past this text run.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daBtfly_c_AssetId {
    ASSET_BUTTERFLY_BMD = 0x043a,
    ASSET_BUTTERFLY_FLY_BCA = 0x043b
};

#include "daBtfly_c.h"
#include "types.h"
#include "common.h"
#include "decl_common.h"
#include "decl_Actor.h"
#include "SharedFilePtr.h"
#include "daPly_c.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_trig.h"
#include "math/Approach.h"
#include "LevelRng.h"

/* Lays .text down in source order (see the note at the top). */
#pragma defer_codegen off

typedef void (daBtfly_c::*ButterflyState)();

/* File-scope objects at the end of this file construct the two resource
 * handles (model file 0x43a, animation file 0x43b) and fill the eight-entry
 * state table. mwcc emits __sinit_d_a_btfly.cpp from those definitions. */
struct BtflyModelFilePtr : SharedFilePtr {
    BtflyModelFilePtr(u32 fileID);
    ~BtflyModelFilePtr();
};

struct BtflyAnimationFilePtr : SharedFilePtr {
    BtflyAnimationFilePtr(u32 fileID);
    ~BtflyAnimationFilePtr();
};

extern "C" {
// local extern: CalcLib.h also conflicts with this TU's retained Vec3_Asr and Vec3_HorzLen prototypes.
s16 Vec3_HorzAngle(const Vector3 *a, const Vector3 *b);
void Vec3_Sub(Vector3 *out, const Vector3 *a, const Vector3 *b);
/* local extern: CalcLib.h clashes with this TU's C Vec3_HorzLen (int) and Vec3_Asr (void *) */
int Vec3_HorzLen(const Vector3 *v);
void Vec3_Asr(void *d, void *s, int sh);
void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
void Matrix4x3_ApplyInPlaceToRotationY(void *m, s16 ang);
int RandomIntInternal(int *seed);
extern Matrix4x3 data_020a0e68; /* scratch matrix */
/* Init LoadFile: ov002 BMD shared with the other small fauna, this overlay's
 * BCA, the still-model BMD, and the animated-model BMD. Cleanup Release. */
extern SharedFilePtr data_ov002_0210d9d8;
extern BtflyAnimationFilePtr gButterflyFlyAnimationFile;
extern SharedFilePtr data_ov100_02148668;
extern BtflyModelFilePtr gButterflyModelFile;
extern ButterflyState data_ov100_02148628[];

}
s16 Vec3_VertAngle(const Vector3 *v1, const Vector3 *v0);

/* One ~daBtfly_c() emits both D1 and D0. */
daBtfly_c::~daBtfly_c()
{
}

void daBtfly_c::State7()
{
    int hasContact;
    int v;

    /* The phase is read as a signed angle, then indexed as unsigned: a
     * single (u16) read shortens State7 by 8 bytes. */
    if (mStateTimer > 120) {
        s16 phase = mFlutterPhase;
        v = FX_MUL(FX_CosIdx((u16)phase >> 4), 0xa3);
        if (v > 0) {
            v = FX_MUL(v, 0x4800);
            mFlutterPhase += 0x2710;
        } else {
            mFlutterPhase += 0xfa0;
        }
        mScale += v;
    }

    ApproachLinear(mHorzSpeed, 0x14000, 0x1000);

    daPly_c *player = ClosestPlayer();
    if (player != 0) {
        ApproachLinear(mPrevAngle.y, HorzAngleToCPlayer(), 0x320);
        ApproachLinear(mPrevAngle.x, Vec3_VertAngle(&mPos, &player->mPos), 0x320);
    }

    dBgCh_Actr_UpdateContinuous_Veneer(&mWithMeshClsn);

    {
        int noId = (mdCcAcPos_c.otherOwner == 0);
        hasContact = (noId == 0);
    }
    /* mClsnFlags & 0x10 is func_02035638. No accessor; IsOnGround is mFlags & 0x10. */
    if (hasContact || mWithMeshClsn.IsOnWall() || mWithMeshClsn.IsOnGround() ||
        func_02035638((u8 *)&mWithMeshClsn) || mStateTimer > 157) {
        if (hasContact != 0) {
            dActor_c *hit = dActor_c::FindWithID(mdCcAcPos_c.otherOwner);
            if (hit != 0) {
                int isPlayer = (hit->actorID == 0xbf);
                if (isPlayer != 0) {
                    Vector3 pos;
                    pos.x = mPos.x;
                    pos.y = mPos.y;
                    pos.z = mPos.z;
                    ((daPly_c *)hit)->Hurt(pos, 2, 0xc000, 1, 0, 1);
                }
            }
        }

        TriplePoofDust();
        MarkForDestruction();
    }

    mdCcAcPos_c.Clear();
    {
        Vector3 off;
        off.x = 0;
        off.y = -0x32000;
        off.z = 0;
        mdCcAcPos_c.SetPosRelativeToActor(off);
    }
    mdCcAcPos_c.Update();
}

void daBtfly_c::State6()
{
    if (mStateTimer == 20) {
        if (mKind != 1) {
            mScale = 0;
            mUseAnimModel = 0;
            return;
        }
        dActor_c::Spawn(0x114, 0, mPos, 0, mAreaId, -1);
        MarkForDestruction();
        return;
    }
    if (mStateTimer <= 20)
        return;
    mScale += 0x40;
    if (mScale < 0x800)
        return;
    SetRanges(0x32000, 0x32000, 0x1000000, 0x320000);
    mFlutterPhase = 0;
    mStateTimer = 0;
    mState = 7;
}

void daBtfly_c::State5()
{
    s16 hAngle;

    if (!IsPlayerInRange(0x5dc)) {
        MarkForDestruction();
        return;
    }

    if (mStateTimer > 110 && DistToCPlayer() < 0xc8000 && (unsigned char)(mKind + 0xff) <= 1) {
        mHorzSpeed = 0;
        mStateTimer = 0;
        mState = 6;
        mFlags &= ~0x10000;
    } else {
        ApproachLinear(mHorzSpeed, 0x8000, 0x800);
    }

    if (mStateTimer >= 60)
        hAngle = mWanderAngle;
    else
        hAngle = Vec3_HorzAngle(&mPos, &mHomePos);

    {
        s16 target;
        if (mPos.y <
            mHomePos.y +
                (int)(((unsigned)RandomIntInternal(&data_0209e650.value) >> 16 & 0xfff) * 0x32 +
                      0x32000))
            target = -0x2000;
        else
            target = 0x2000;
        ApproachLinear(mPrevAngle.x, target, 0x190);
    }

    ApproachLinear(
        mPrevAngle.y, hAngle,
        (s16)((((unsigned)RandomIntInternal(&data_0209e650.value) >> 16) % 800) + 0x190));
}

void daBtfly_c::State4()
{
    int typ = param1 & 0xff;
    if ((typ & 0xc0) != 0) {
        mKind = (u8)(typ >> 6);
    } else {
        if (DistToCPlayer() >= 0xc8000)
            return;
        {
            int sb = (int)((unsigned int)RandomIntInternal(&data_0209e650.value) % 3);
            int sel;
            int i;
            int mask = typ & 0x30;
            Vector3_16 rot = mPrevAngle;
            for (i = 1; i < 3; i++) {
                if (i == sb)
                    sel = 1;
                else if (mask == 0x20)
                    sel = 2;
                else
                    sel = 3;
                rot.y += (s16)((unsigned int)RandomIntInternal(&data_0209e650.value) >> 0x10);
                dActor_c::Spawn(0x150, mask | (sel << 6), mPos, &rot, mAreaId, -1);
            }
            if (sb == 0)
                mKind = 1;
            else if (mask == 0x20)
                mKind = 2;
            else
                mKind = 3;
        }
    }

    mWanderAngle = (s16)((unsigned int)RandomIntInternal(&data_0209e650.value) >> 0x10);
    mPrevAngle.y =
        (s16)(mWanderAngle +
              (int)(((unsigned int)RandomIntInternal(&data_0209e650.value) >> 0x10) & 0x3fff));
    mHorzSpeed =
        (int)(((unsigned int)RandomIntInternal(&data_0209e650.value) >> 0x10) & 0xfff) * 0xf +
        0xf000;
    mStateTimer = 0;
    mState = 5;
}

void daBtfly_c::State3()
{
    ApproachLinear(mPrevAngle.y, Vec3_HorzAngle(&mPos, &mHomePos), 0x800);
    ApproachLinear(mPrevAngle.x, Vec3_VertAngle(&mPos, &mHomePos), 0x50);
    UpdatePos(0);

    mPos.y -= FX_MUL(mHorzSpeed, FX_SinIdx((unsigned short)mPrevAngle.x >> 4)) +
              FX_CosIdx((unsigned short)(short)((mStateTimer << 16) / 100) >> 4) * (short)20 / 4;

    mStateTimer = mStateTimer + 1;
    if (mStateTimer > 100)
        mStateTimer = 0;

    if (IsPlayerInRange(0xbb8))
        return;

    mPos.x = mHomePos.x;
    mPos.y = mHomePos.y;
    mPos.z = mHomePos.z;
    mState = 1;
}

void daBtfly_c::State2()
{
    daPly_c *player;
    Vector3 v;
    Vector3 d;

    mPos.x = mHomePos.x;
    mPos.y = mHomePos.y;
    mPos.z = mHomePos.z;

    player = ClosestPlayer();
    if (player != 0) {
        int *pp;
        Vec3_Sub(&d, &mPos, &player->mPos);
        v = d;
        if (Vec3_HorzLen(&v) > 0x4b0000)
            mState = 3;

        pp = &player->mPos.x;
        mPos.x = mPrevPos.x;
        mPos.y = mPrevPos.y;
        mPos.z = mPrevPos.z;

        v.x = pp[0];
        v.y = pp[1];
        v.z = pp[2];
        v.x += mStateTimer * 0x5000 / 4;
        v.z += mStateTimer * 0x5000 / 4;
        ApproachLinear(mPrevAngle.y, Vec3_HorzAngle(&mPos, &v), 0x300);

        pp = &player->mPos.x;
        v.x = pp[0];
        v.y = pp[1];
        v.z = pp[2];
        v.y += ((mStateTimer * 5 + 0x100) << 12) / 4;
        ApproachLinear(mPrevAngle.x, Vec3_VertAngle(&mPos, &v), 0x500);

        UpdatePos(0);

        mPos.y -= (FX_MUL(mHorzSpeed, FX_SinIdx((unsigned short)mPrevAngle.x >> 4)) +
                   FX_CosIdx((unsigned short)(short)((mStateTimer << 16) / 100) >> 4) * 20 / 4);
        mStateTimer++;
        if (mStateTimer > 100)
            mStateTimer = 0;
        return;
    }
    mPos.x = mPrevPos.x;
    mPos.y = mPrevPos.y;
    mPos.z = mPrevPos.z;
}


void daBtfly_c::State1()
{
    if (IsPlayerInRange(0x3e8) == 0)
        return;
    mState = 2;
    daPly_c *p = ClosestPlayer();
    mPrevAngle.y = Vec3_HorzAngle(&mPos, &p->mPos);
}

void daBtfly_c::State0()
{
    int n;
    int i;
    n = (param1 & 0xf) - 1;
    Vector3_16 rot = mPrevAngle;
    for (i = 0; i < n; i++) {
        Vector3 pos;
        int r;
        r = RandomIntInternal(&data_0209e650.value);
        pos.x = ((int)((unsigned int)r % 20) - 0xa) * 0xa000 + mPos.x;
        r = RandomIntInternal(&data_0209e650.value);
        pos.y = mPos.y + ((int)((unsigned int)r % 40) << 13);
        r = RandomIntInternal(&data_0209e650.value);
        pos.z = ((int)((unsigned int)r % 20) - 0xa) * 0xa000 + mPos.z;
        r = RandomIntInternal(&data_0209e650.value);
        rot.y += (r << 1) >> 16;
        dActor_c::Spawn(0x150, 0, pos, &rot, mAreaId, -1);
    }
    mState = 1;
}

int daBtfly_c::CleanupResources()
{
    gButterflyModelFile.Release();
    gButterflyFlyAnimationFile.Release();
    data_ov002_0210d9d8.Release();
    data_ov100_02148668.Release();
    return 1;
}

void daBtfly_c::OnPendingDestroy()
{
}

int daBtfly_c::Render()
{
    if (mState == 4)
        return 1;
    if (mUseAnimModel != 0)
        mAnmModel.Render(0);
    else
        mModel.Render(&this->dActor_c::mScale);
    return 1;
}

int daBtfly_c::Behavior()
{
    (this->*data_ov100_02148628[mState])();

    if (mKind != 0) {
        int spd = mScale;
        dActor_c::mScale.x = spd;
        dActor_c::mScale.y = spd;
        dActor_c::mScale.z = spd;

        {
            int s = mHorzSpeed;
            int pitch = (unsigned short)mPrevAngle.x >> 4;
            mSpeed.y = FX_MUL(-FX_SinIdx(pitch), s);
            int horiz = FX_MUL(FX_CosIdx(pitch), s);
            int yaw = (unsigned short)mPrevAngle.y >> 4;
            mSpeed.x = FX_MUL(horiz, FX_SinIdx(yaw));
            /* The ROM recomputes the yaw index here; FX_CosIdx(yaw) is two
             * words short. */
            mSpeed.z = FX_MUL(horiz, FX_CosIdx((unsigned short)mPrevAngle.y >> 4));
        }
        UpdatePosWithOnlySpeed(0);
        mStateTimer++;
    }

    if (mState != 4) {
        int t[3];
        Vec3_Asr(t, &mPos.x, 3);
        Matrix4x3_FromTranslation(&data_020a0e68, t[0], t[1], t[2]);
        mAngle.y = mPrevAngle.y;
        Matrix4x3_ApplyInPlaceToRotationY(&data_020a0e68, mAngle.y);
        if (mUseAnimModel != 0) {
            mAnmModel.mat4x3 = data_020a0e68;
            DropShadowRadHeight(mShadowModel1, mAnmModel.mat4x3, 0x14000, 0x12c000, 0xf);
            mAnmModel.Advance();
        } else {
            mModel.mat4x3 = data_020a0e68;
            DropShadowRadHeight(mShadowModel2, mModel.mat4x3, 0x64000, 0x12c000, 0xf);
        }
    }
    return 1;
}

int daBtfly_c::InitResources()
{
    dExtSimpleModel_c::LoadFile(data_ov002_0210d9d8);
    dExtFrameCtrl_c::LoadFile(gButterflyFlyAnimationFile);
    dExtSimpleModel_c::LoadFile(data_ov100_02148668);
    BMD_File *bmd = dExtSimpleModel_c::LoadFile(gButterflyModelFile);

    if (mAnmModel.SetFile(bmd, 1, 1) == 0)
        return 0;
    mAnmModel.SetAnim((BCA_File *)gButterflyFlyAnimationFile.filePtr, 0, 0x1000, 0);
    if (mShadowModel1.InitCylinder() == 0)
        return 0;
    if (mModel.SetFile((BMD_File *)data_ov100_02148668.filePtr, 1, 1) == 0)
        return 0;
    if (mShadowModel2.InitCylinder() == 0)
        return 0;

    mWithMeshClsn.Init(this, 0x32000, 0x32000, 0, 0);

    Vector3 v;
    v.x = 0;
    v.y = -0x32000;
    v.z = 0;
    mdCcAcPos_c.Init(this, v, 0x32000, 0x64000, 0x200000, 0);

    int sub = (int)(u8)(param1 & 0x30);
    if (sub != 0x10 && sub != 0x20) {
        mKind = 0;
        mHorzSpeed = 0x7800;
        mStateTimer = ((u32)RandomIntInternal(&data_0209e650.value) >> 16) % 100;
        if ((u32)(u8)(param1 & 0xf) > 1)
            mState = 0;
        else
            mState = 1;
    } else {
        mState = 4;
    }

    mHomePos.x = mPos.x;
    mHomePos.y = mPos.y;
    mHomePos.z = mPos.z;

    int r = RandomIntInternal(&data_0209e650.value);
    int fc = mAnmModel.GetFrameCount();
    u32 rem = (u32)r % (u32)fc;
    mAnmModel.currFrame = (rem << 16) >> 4;

    mScale = 0x1000;
    s32 spd = mScale;
    dActor_c::mScale.x = spd;
    dActor_c::mScale.y = spd;
    dActor_c::mScale.z = spd;
    mUseAnimModel = 1;
    return 1;
}

daBtfly_c *daBtfly_c_classInit() {
    return new daBtfly_c();
}

/* Static-init globals (was the handwritten __sinit_ov100_021473bc shard).
 * Definition order is the retail initializer's construction order: the model
 * handle, then the animation handle, then the state table. */
BtflyModelFilePtr gButterflyModelFile(ASSET_BUTTERFLY_BMD);
BtflyAnimationFilePtr gButterflyFlyAnimationFile(ASSET_BUTTERFLY_FLY_BCA);

ButterflyState data_ov100_02148628[8] = {
    &daBtfly_c::State0,
    &daBtfly_c::State1,
    &daBtfly_c::State2,
    &daBtfly_c::State3,
    &daBtfly_c::State4,
    &daBtfly_c::State5,
    &daBtfly_c::State6,
    &daBtfly_c::State7,
};
