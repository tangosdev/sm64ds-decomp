//cpp
/* BBH haunted chair (CHAIR 326), ov020 .text 0x02112938..0x021134e4.
 * State0 arms off the Mad Piano within 300 units. State1 rattles, then
 * tips (State3) or hops (State2). State2 launches at the player and
 * breaks on the ground or a wrong-facing wall. State3 rocks one tilt
 * angle toward its target.
 *
 * InitResources is the key function, so this TU emits the vtable (RTTI
 * _ZTS 0x021149d8, _ZTV 0x02114a14); both destructor variants are
 * inline. common.h comes before daChair_c.h so the shadow-matrix copy
 * stays twelve words. opt_common_subs off holds ApproachStateValue.
 */
// What stays, and why:
// - The extern "C" block holds the arm9 helpers that have no shared header
//   (Vec3_Sub, VEC_Add, AngleDiff,
//   Matrix4x3_FromRotationZXYExt).
// - The ROM calls dBgCh_Actr_UpdateContinuous_Veneer (0x020383fc), not
//   UpdateContinuous. func_0201267c is Sound.h's bank-3 player, one past
//   Sound::PlayBank3.
// - The wall normal goes through the CopyNormalTo reference pun. Vec3 POD
//   twins (player copy, diff, normal, hurt pos): a Vector3 local emits its
//   destructor call. Copying player->mPosX field by field changes State2's
//   size.
// - g_profile_CHAIR is not in this TU. data_ov020_02114af0 is the model
//   handle.

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daChair_c_AssetId { ASSET_CHAIR_BMD = 0x02d0 };

#include "common.h"
#include "nitro/fx/fx.h"
#include "daChair_c.h"
#include "SharedFilePtr.h"
#include "SurfaceInfo.h"
#include "daPly_c.h"
#include "dBgCh_Actr.h"
#include "Sound.h"
#include "cstd.h"
#include "Particle__System.h"
#include "ParticleIds.h"
#include "math/CalcLib.h"
#include "math/MtxStack.h"
#include "nitro/fx/fx_trig.h"

/* Only ApproachStateValue needs this: without it the two (short)accel casts
 * share one sign-extend and both smulbb become muls (21 of 44 words differ). */
#pragma opt_common_subs off

/* 8-byte file handle. The model uses func_02017acc / func_02017ab4. The
 * spelling is local; the manifest aliases the generated names to those ROM
 * symbols. */
struct ChairModelFilePtr : SharedFilePtr {
    ChairModelFilePtr(u32 fileID);
    ~ChairModelFilePtr();
};

/* The chair model's shared file, defined at the end of this file so the
 * constructor does not enter .text. Init loads it, Cleanup releases it. */
extern ChairModelFilePtr data_ov020_02114af0;
extern Matrix4x3 IDENTITY_MATRIX4X3;

/* Actor IDs from the ROM's actor table. */
enum ChairActorId {
    kPianoActorId = 0xf9, /* Mad Piano; State0 arms off it */
    kPlayerActorId = 0xbf,
};

/* Sound IDs, named by the state that plays them. */
enum ChairSfxId {
    kSfxWobble = 0x5f, /* State1: chair rocks the other way */
    kSfxWindup = 0x5c, /* State2: wind-up ends */
    kSfxHop = 0x5d,    /* State2: hop timer expires, aims at the player */
};

/* local extern: arm9 C helpers with no shared header yet. Vec3_Sub takes the
   shared Vector3 interface; this TU keeps NitroSDK VecFx32 views. VEC_Add
   also takes Vector3 (math/CalcLib.h declares it). */
extern "C" {
void Vec3_Sub(Vector3 *out, const Vector3 *a, const Vector3 *b);
void VEC_Add(Vector3 *dst, Vector3 *add, Vector3 *src);
int AngleDiff(int a, int b);
}


/* CHAIR registry factory. */
extern "C" daChair_c *daChair_c_classInit(void) {
    return new daChair_c();
}

s32 daChair_c::InitResources() {
    BMD_File *file = dExtSimpleModel_c::LoadFile(data_ov020_02114af0);
    mModel.SetFile(file, 1, -1);
    mShadowModel.InitCylinder();
    mWithMeshClsn.Init(this, 0x32000, 0x64000, 0, 0);
    mClsnOffset.x = 0;
    mClsnOffset.y = 0;
    mClsnOffset.z = 0;
    mCylinder.Init(this, mClsnOffset, 0x32000, 0x64000, 0x200004, 0);
    mState = 0;
    mHomePos.x = mPosX;
    mHomePos.y = mPosY;
    mHomePos.z = mPosZ;
    mShadowMat = IDENTITY_MATRIX4X3;
    return 1;
}

s32 daChair_c::Behavior() {
    func_0200f760(&mCylinder);
    switch (mState) {
    case 0:
        State0();
        break;
    case 1:
        State1();
        break;
    case 2:
        State2();
        break;
    case 3:
        State3();
        break;
    }
    UpdateModel();
    mCylinder.Clear();
    mCylinder.SetPosRelativeToActor(mClsnOffset);
    mCylinder.Update();
    return 1;
}

s32 daChair_c::Render() {
    mModel.Render(0);
    return 1;
}

s32 daChair_c::CleanupResources() {
    data_ov020_02114af0.Release();
    return 1;
}

void daChair_c::UpdateModel() {
    Matrix4x3_FromRotationZXYExt(&mModel.mat4x3, mAngleX, mAngleY, mAngleZ);
    mModel.mat4x3.m[9] = mPosX >> 3;
    mModel.mat4x3.m[10] = mPosY >> 3;
    mModel.mat4x3.m[11] = mPosZ >> 3;
    mShadowMat.m[9] = mPosX >> 3;
    mShadowMat.m[10] = mHomePos.y >> 3;
    mShadowMat.m[11] = mPosZ >> 3;
    DropShadowRadHeight(mShadowModel, mShadowMat, 0x32000, 0x1e000, 0xf);
}

void daChair_c::Break() {
    Particle::System::NewSimple(kPtclChairBreakA, mPosX, mPosY, mPosZ);
    Particle::System::NewSimple(kPtclChairBreakB, mPosX, mPosY, mPosZ);
    PoofDust();
    MarkForDestruction();
}

int daChair_c::ApproachStateValue(s16 *pos, s16 *vel, s32 target, s16 thresh, s32 accel, s16 mult) {
    short old = pos[0];
    pos[0] = old + vel[0];
    short now = pos[0];
    if (now == target ||
        ((now - target) * (old - target) < 0 && vel[0] > -thresh && vel[0] < thresh)) {
        pos[0] = target;
        vel[0] = 0;
        return 1;
    }
    if (now >= target)
        accel = (short)-accel;
    if ((short)vel[0] * (short)accel < 0)
        accel = (short)accel * (short)mult;
    vel[0] = vel[0] + accel;
    return 0;
}

void daChair_c::State0() {
    dActor_c *piano;

    mState = 1;
    mActionTimer = 1;
    mTargetAngle = 0;
    mStateValue0 = 0;
    mStateValue1 = 0;
    mStateValue2 = 0;
    /* The chair arms off the Mad Piano. */
    piano = dActor_c::FindWithActorID(kPianoActorId, 0);
    mTargetID = 0;
    if (piano == 0)
        return;
    if (Vec3_Dist(&mPos, &piano->mPos) < 0x12c000) {
        mTargetID = piano->uniqueID;
        mActionTimer = 0;
    }
}

void daChair_c::State1() {
    int targetPos[4];
    dActor_c *target;
    short delta;
    daPly_c *player;
    int wobble;
    int *av;

    target = dActor_c::FindWithID(mTargetID);
    if (target != 0) {
        /* The three words are loaded before either Vec3 call. */
        av = &target->mPosX;
        targetPos[0] = *av;
        targetPos[1] = av[1];
        targetPos[2] = av[2];
        if (Vec3_Dist(&mPos, (const Vector3 *)targetPos) >= 0xfa000)
            return;
        delta = (Vec3_HorzAngle(&mPos, (const Vector3 *)targetPos) - mAngleY + 0x2000);
        if (delta & 0x4000) {
            mTrackedAngle = &mAngleZ;
            if (delta > 0)
                mTargetAngle = 0x4000;
            else
                mTargetAngle = -0x4000;
        } else {
            mTrackedAngle = &mAngleX;
            if (delta < 0)
                mTargetAngle = 0x5800;
            else
                mTargetAngle = -0x4000;
        }
        if (mTargetAngle < 0)
            mStateValue0 = 0xfa24;
        else
            mStateValue0 = 0x5dc;
        mState = 3;
        return;
    }

    if (mActionTimer != 0) {
        player = ClosestPlayer();
        if (player != 0) {
            if (Vec3_Dist(&mPos, &player->mPos) < 0x1f4000)
                mActionTimer = 0;
        }
        mStateTimer = 0;
        return;
    }

    mStateTimer++;
    if (mStateTimer & 8) {
        if (mAngleX >= 0) {
            wobble = -4;
        } else {
            func_0201267c(kSfxWobble, &mCamSpacePos);
            wobble = 4;
        }
        mPosX -= wobble << 12;
        mPosZ -= wobble << 12;
        mAngleZ = (wobble * 0x32);
        mAngleX = mAngleZ;
    } else {
        mAngleZ = 0;
        mAngleX = mAngleZ;
    }

    if (mStateTimer < 0x1e)
        return;
    mState = 2;
    mStateValue0 = 0;
    mStateValue1 = 0;
    mStateValue2 = 0xc8;
    mActionTimer = 0x28;
    mStateTimer = 0;
}

void daChair_c::State2() {
    dBgCh_Actr_UpdateContinuous_Veneer(&mWithMeshClsn);
    mStateTimer++;

    if (mStateTimer <= 0x46) {
        if (mStateTimer < 0x32)
            mVertSpeed = 0x6000;
        else
            mVertSpeed = 0;
        ApproachStateValue(&mAngleX, &mStateValue0, -0xfa0, 0xc8, 0x14, 2);
        ApproachStateValue(&mAngleZ, &mStateValue2, 0, 0, 0x14, 1);
        if (mStateTimer == 0x46)
            func_0201267c(kSfxWindup, &mCamSpacePos);
    } else {
        if (mActionTimer != 0) {
            mActionTimer--;
            if (mActionTimer == 0) {
                dActor_c *player;
                VecFx32 *pp;
                Vector3 playerPos;
                Vector3 diff;
                s32 reach;
                func_0201267c(kSfxHop, &mCamSpacePos);
                player = ClosestPlayer();
                if (player == 0)
                    return;
                /* Copied through a plain vector. Copying player->mPosX
                 * field by field changes the code size. */
                pp = (VecFx32 *)&player->mPosX;
                playerPos.x = pp->x;
                playerPos.y = pp->y;
                playerPos.z = pp->z;
                Vec3_Sub(&diff, &playerPos, &mPos);
                mPrevAngleY = cstd::atan2(diff.x, diff.z);
                mPrevAngleX = cstd::atan2(diff.y, Vec3_HorzLen(&diff)) * -1;
                /* mSpeedX / mSpeedZ: lateral velocity beside mVertSpeed. */
                reach = FX_MUL(FX_CosIdx((u16)mPrevAngleX >> 4), 0x32000);
                mSpeedX = FX_MUL(reach, FX_SinIdx((u16)mPrevAngleY >> 4));
                mVertSpeed = FX_MUL(FX_SinIdx((u16)mPrevAngleX >> 4), -0x32000);
                mSpeedZ = FX_MUL(reach, FX_CosIdx((u16)mPrevAngleY >> 4));
            } else {
                if (mActionTimer > 0x14) {
                    mAngleY += 0x2710;
                }
            }
        } else {
            if (mWithMeshClsn.IsOnGround() != 0) {
                Break();
            } else if (mWithMeshClsn.IsOnWall() != 0) {
                dBgPi *wall = mWithMeshClsn.GetWallResult();
                VecFx32 normal;
                wall->surface.CopyNormalTo((Vector3 &)normal);
                if (GetSubtraction(mPrevAngleY, cstd::atan2(normal.x, normal.z)) > 0x4000)
                    Break();
            }
        }
    }

    VEC_Add(&mPos, (Vector3 *)&mSpeedX, &mPos);

    {
        u32 id = mCylinder.otherOwner;
        dActor_c *actor;
        int isPlayer;
        if (id == 0)
            return;
        actor = dActor_c::FindWithID(id);
        if (actor == 0)
            return;
        /* Direct comparison drops the ROM's store-then-load. */
        isPlayer = (actor->actorID == kPlayerActorId);
        if (isPlayer == 0)
            return;
        isPlayer = ((daPly_c *)actor)->mIsVanish;
        if (isPlayer != 0)
            return;
        {
            VecFx32 pos;
            pos.x = mPosX;
            pos.y = mPosY;
            pos.z = mPosZ;
            ((daPly_c *)actor)->Hurt(*(Vector3 *)&pos, 2, 0xc000, 1, 0, 1);
        }
    }
}

void daChair_c::State3() {
    s16 *tracked = mTrackedAngle;
    int angle;
    s16 *ip;
    s16 sample;
    s16 wrapped;
    int offset;

    ApproachStateValue(tracked, &mStateValue0, mTargetAngle, 0xfa0, 0x14, 2);
    angle = AngleDiff(*mTrackedAngle, 0);
    ip = mTrackedAngle;
    sample = FX_SinIdx((u16)*ip >> 4);
    wrapped = angle;
    offset = (s16)sample * (s16)0x50;
    if (ip != &mAngleX) {
        mClsnOffset.x = -offset;
        mClsnOffset.y = 0;
        mClsnOffset.z = 0;
    } else {
        mClsnOffset.x = 0;
        mClsnOffset.y = 0;
        mClsnOffset.z = offset;
    }
    if (wrapped >= 0x4000) {
        s16 past = (wrapped - 0x4000);
        s16 tip = FX_SinIdx((u16)past >> 4);
        mPosY = (s16)tip * (s16)0x28 + (mHomePos.y + 0x28000);
    } else {
        s16 tip = FX_SinIdx((u16)wrapped >> 4);
        mPosY = (s16)tip * (s16)0x28 + mHomePos.y;
    }
}

ChairModelFilePtr data_ov020_02114af0(ASSET_CHAIR_BMD);
