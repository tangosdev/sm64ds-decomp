//cpp
/**
 * daChair_c -- Big Boo's Haunt haunted chair (CHAIR 326), ov020.
 *
 * RTTI ov020:0x021149d8 names the class. State0 looks up actor 0xf9
 * (PIANO) and, if that piano is within 300.0, stores its uniqueID.
 * State1 rattles until the piano or a player is close, then tips
 * toward it (State3) or hops off (State2). State2 launches at the
 * player and breaks on the ground, or on a wall faced the wrong way.
 * State3 rocks mAngleX or mAngleZ toward mTargetAngle and slides the
 * cylinder with the tip.
 *
 * common.h comes before daChair_c.h. InitResources copies
 * IDENTITY_MATRIX4X3 into mShadowMat, and only the flat s32 m[12]
 * spelling is the twelve-word copy.
 *
 * Leftover: dCcAcPos_c::Init, dBgCh_Actr::Init, DropShadowRadHeight,
 *   Particle::System::NewSimple and Player::Hurt stay scalar externs.
 *   Calling the header with Fix12<int> by value pools the constants
 *   (InitResources and UpdateModel grew .rodata; remeasured).
 *   dBgCh_Actr::Init's header spells Fix12i, which mangles as int,
 *   not 5Fix12IiE.
 * Leftover: GetWallResult has no dBgCh_Actr method. The result's
 *   SurfaceInfo sits at +4; CopyNormalTo is the real method.
 * Leftover: the ROM calls dBgCh_Actr_UpdateContinuous_Veneer
 *   (0x020383fc), not UpdateContinuous.
 * Leftover: func_0201267c is the bank-3 player just after
 *   Sound::PlayBank3 (0x02012664). This TU calls 0x0201267c.
 *   func_0200f760 sets the cylinder movement bit from the closest
 *   player's vanish flag. Neither symbol has a header name.
 * Leftover: cstd::atan2's symbol carries 5Fix12IiE. A scalar
 *   namespace declaration would mangle as ii.
 * Leftover: dActor_c has no Pos(). Callers pass &mPosX.
 *   State2 still copies the player's position through a plain Vec3;
 *   player->mPosX by name changes that function's size.
 * Leftover: State2 compares mStateTimer and mActionTimer through a
 *   view based at this+0x300. The same words are updated through
 *   (int)&mStateTimer / (int)&mActionTimer. mAngleY += 0x2710
 *   changes State2's size; the int-cast form matches.
 *   unk_0a4 and unk_0ac stay those names (lateral velocity beside
 *   mVertSpeed; the base header still spells them unk_).
 * Leftover: data_ov020_02114af0 is the BMD SharedFilePtr this TU
 *   loads and releases. ov020 sinit constructs it. g_profile_CHAIR
 *   is not in this TU.
 */

#include "common.h"
#include "daChair_c.h"
#include "SharedFilePtr.h"
#include "SurfaceInfo.h"
#include "Player.h"

#pragma opt_common_subs off

extern SharedFilePtr data_ov020_02114af0;
extern Matrix4x3 IDENTITY_MATRIX4X3;

typedef struct { s32 x, y, z; } Vec3;

/* mStateTimer at +0x9e and mActionTimer at +0xa0, addressed from
 * this+0x300 rather than from the fields. */
typedef struct {
    u8 _pad[0x9e];
    u16 counter;
    u16 timer;
} State300;

#define ST ((State300 *)((char *)this + 0x300))

extern "C" {
void func_0200f760(void *self, void *cyl);
void func_0201267c(unsigned int id, void *pos);
void dBgCh_Actr_UpdateContinuous_Veneer(void *clsn);
void Vec3_Sub(Vec3 *res, Vec3 *v0, Vec3 *v1);
s16 _ZN4cstd5atan2E5Fix12IiES1_(s32 y, s32 x);
s32 Vec3_HorzLen(const Vector3 *v);
void *_ZNK10dBgCh_Actr13GetWallResultEv(void *clsn);
void AddVec3(Vec3 *dst, Vec3 *add, Vec3 *src);
int Vec3_Dist(const Vector3 *a, const Vector3 *b);
short Vec3_HorzAngle(const Vector3 *a, const Vector3 *b);
int AngleDiff(int a, int b);
void Matrix4x3_FromRotationZXYExt(void *m, int x, int y, int z);

void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
    void *self, dActor_c *actor, int radius, int height,
    Vector3_16 *rot, Vector3_16 *unk);
void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
    void *self, dActor_c *actor, const Vector3 *offset,
    int radius, int height, u32 flags, u32 vulnFlags);
void _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
    void *self, void *shadow, void *matrix, int radius, int height, u32 flags);
void *_ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(
    unsigned id, int x, int y, int z);
int _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(
    void *player, const void *pos, u32 kind, int damage, u32 a, u32 b, u32 c);

extern s16 data_02082214[];
}

// @symbol daChair_c_classInit
/* CHAIR registry factory. `return new daChair_c()` matches; the
 * synthesized ctor stores the vtable because this TU defines it.
 * classInit is reconstructed, not a preserved identifier. */
extern "C" daChair_c *daChair_c_classInit(void)
{
    return new daChair_c();
}

// @symbol _ZN9daChair_c13InitResourcesEv
s32 daChair_c::InitResources()
{
    BMD_File *file = (BMD_File *)Model::LoadFile(data_ov020_02114af0);
    mModel.SetFile(file, 1, -1);
    mShadowModel.InitCylinder();
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(
        &mWithMeshClsn, this, 0x32000, 0x64000, 0, 0);
    mClsnOffset.x = 0;
    mClsnOffset.y = 0;
    mClsnOffset.z = 0;
    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
        &mCylinder, this, &mClsnOffset, 0x32000, 0x64000, 0x200004, 0);
    mState = 0;
    mHomePos.x = mPosX;
    mHomePos.y = mPosY;
    mHomePos.z = mPosZ;
    mShadowMat = IDENTITY_MATRIX4X3;
    return 1;
}

// @symbol _ZN9daChair_c8BehaviorEv
s32 daChair_c::Behavior()
{
    func_0200f760(this, &mCylinder);
    switch (mState) {
    case 0: State0(); break;
    case 1: State1(); break;
    case 2: State2(); break;
    case 3: State3(); break;
    }
    UpdateModel();
    mCylinder.Clear();
    mCylinder.SetPosRelativeToActor(mClsnOffset);
    mCylinder.Update();
    return 1;
}

// @symbol _ZN9daChair_c6RenderEv
s32 daChair_c::Render()
{
    mModel.Render(0);
    return 1;
}

// @symbol _ZN9daChair_c16CleanupResourcesEv
s32 daChair_c::CleanupResources()
{
    data_ov020_02114af0.Release();
    return 1;
}

// @symbol _ZN9daChair_c11UpdateModelEv
void daChair_c::UpdateModel()
{
    Matrix4x3_FromRotationZXYExt(&mModel.mat4x3, mAngleX, mAngleY, mAngleZ);
    mModel.mat4x3.m[9] = mPosX >> 3;
    mModel.mat4x3.m[10] = mPosY >> 3;
    mModel.mat4x3.m[11] = mPosZ >> 3;
    mShadowMat.m[9] = mPosX >> 3;
    mShadowMat.m[10] = mHomePos.y >> 3;
    mShadowMat.m[11] = mPosZ >> 3;
    _ZN8dActor_c19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        this, &mShadowModel, &mShadowMat, 0x32000, 0x1e000, 0xf);
}

// @symbol _ZN9daChair_c5BreakEv
void daChair_c::Break()
{
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x25, mPosX, mPosY, mPosZ);
    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x26, mPosX, mPosY, mPosZ);
    PoofDust();
    MarkForDestruction();
}

// @symbol _ZN9daChair_c18ApproachStateValueEPsS0_isis
int daChair_c::ApproachStateValue(s16 *pos, s16 *vel, s32 target,
                                  s16 thresh, s32 accel, s16 mult)
{
    short old = pos[0];
    pos[0] = old + vel[0];
    short now = pos[0];
    if (now == target
        || ((now - target) * (old - target) < 0
            && vel[0] > -thresh && vel[0] < thresh)) {
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

// @symbol _ZN9daChair_c6State0Ev
void daChair_c::State0()
{
    dActor_c *piano;

    mState = 1;
    mActionTimer = 1;
    mTargetAngle = 0;
    mStateValue0 = 0;
    mStateValue1 = 0;
    mStateValue2 = 0;
    /* 0xf9 is PIANO (249). The chair arms off the Mad Piano. */
    piano = dActor_c::FindWithActorID(0xf9, 0);
    mTargetID = 0;
    if (piano == 0)
        return;
    if (Vec3_Dist((const Vector3 *)&mPosX, (const Vector3 *)&piano->mPosX) < 0x12c000) {
        mTargetID = piano->uniqueID;
        mActionTimer = 0;
    }
}

// @symbol _ZN9daChair_c6State1Ev
void daChair_c::State1()
{
    int targetPos[4];
    dActor_c *target;
    short delta;
    Player *player;
    int wobble;
    int *av;
    unsigned short *p39e;

    target = dActor_c::FindWithID(mTargetID);
    if (target != 0) {
        /* int-cast address. The three words are loaded before either
         * Vec3 call. */
        av = (int *)(int)((char *)&target->mPosX);
        targetPos[0] = *av;
        targetPos[1] = av[1];
        targetPos[2] = av[2];
        if (Vec3_Dist((const Vector3 *)&mPosX, (const Vector3 *)targetPos) >= 0xfa000)
            return;
        delta = (short)(Vec3_HorzAngle((const Vector3 *)&mPosX, (const Vector3 *)targetPos)
                        - mAngleY + 0x2000);
        if (delta & 0x4000) {
            mTrackedAngle = &mAngleZ;
            if (delta > 0)
                mTargetAngle = 0x4000;
            else
                mTargetAngle = (short)-0x4000;
        } else {
            mTrackedAngle = &mAngleX;
            if (delta < 0)
                mTargetAngle = 0x5800;
            else
                mTargetAngle = (short)-0x4000;
        }
        if (mTargetAngle < 0)
            mStateValue0 = (short)0xfa24;
        else
            mStateValue0 = 0x5dc;
        mState = 3;
        return;
    }

    if (mActionTimer != 0) {
        player = ClosestPlayer();
        if (player != 0) {
            if (Vec3_Dist((const Vector3 *)&mPosX, (const Vector3 *)&player->mPosX) < 0x1f4000)
                mActionTimer = 0;
        }
        mStateTimer = 0;
        return;
    }

    /* int-cast address, then a separate add. */
    p39e = (unsigned short *)(int)((char *)&mStateTimer);
    *p39e = (unsigned short)(*p39e + 1);
    if (mStateTimer & 8) {
        if (mAngleX >= 0) {
            wobble = -4;
        } else {
            func_0201267c(0x5f, &mCamSpacePosX);
            wobble = 4;
        }
        /* int-cast address. */
        {
            int *px = (int *)(int)&mPosX;
            *px = *px - (wobble << 12);
        }
        {
            int *pz = (int *)(int)&mPosZ;
            *pz = *pz - (wobble << 12);
        }
        mAngleZ = (short)(wobble * 0x32);
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

// @symbol _ZN9daChair_c6State2Ev
void daChair_c::State2()
{
    dBgCh_Actr_UpdateContinuous_Veneer(&mWithMeshClsn);
    (*(u16 *)((int)&mStateTimer))++;

    if (ST->counter <= 0x46) {
        if (ST->counter < 0x32)
            mVertSpeed = 0x6000;
        else
            mVertSpeed = 0;
        ApproachStateValue(&mAngleX, &mStateValue0, -0xfa0, 0xc8, 0x14, 2);
        ApproachStateValue(&mAngleZ, &mStateValue2, 0, 0, 0x14, 1);
        if (ST->counter == 0x46)
            func_0201267c(0x5c, &mCamSpacePosX);
    } else {
        if (ST->timer != 0) {
            (*(u16 *)((int)&mActionTimer))--;
            if (ST->timer == 0) {
                dActor_c *player;
                Vec3 *pp;
                Vec3 playerPos;
                Vec3 diff;
                s32 reach;
                func_0201267c(0x5d, &mCamSpacePosX);
                player = ClosestPlayer();
                if (player == 0)
                    return;
                /* Copied through a plain vector. player->mPosX by name
                 * changes the code size. */
                pp = (Vec3 *)((char *)&player->mPosX);
                playerPos.x = pp->x;
                playerPos.y = pp->y;
                playerPos.z = pp->z;
                Vec3_Sub(&diff, &playerPos, (Vec3 *)&mPosX);
                mPrevAngleY = _ZN4cstd5atan2E5Fix12IiES1_(diff.x, diff.z);
                mPrevAngleX = _ZN4cstd5atan2E5Fix12IiES1_(
                                  diff.y, Vec3_HorzLen((const Vector3 *)&diff)) * -1;
                /* unk_0a4 / unk_0ac: lateral velocity beside mVertSpeed.
                 * Unsigned table index; mPrevAngleX/Y are s16. */
                reach = (s32)(((long long)data_02082214[((*(u16 *)&mPrevAngleX) >> 4) * 2 + 1]
                               * 0x32000 + 0x800) >> 12);
                unk_0a4 = (s32)(((long long)reach
                                 * data_02082214[((*(u16 *)&mPrevAngleY) >> 4) * 2] + 0x800) >> 12);
                mVertSpeed = (s32)(((long long)data_02082214[((*(u16 *)&mPrevAngleX) >> 4) * 2]
                                    * -0x32000 + 0x800) >> 12);
                unk_0ac = (s32)(((long long)reach
                                 * data_02082214[((*(u16 *)&mPrevAngleY) >> 4) * 2 + 1] + 0x800) >> 12);
            } else {
                if (ST->timer > 0x14) {
                    /* (int)&mAngleY. mAngleY += 0x2710 changes the code size. */
                    *(s16 *)((int)&mAngleY) += 0x2710;
                }
            }
        } else {
            if (mWithMeshClsn.IsOnGround() != 0) {
                Break();
            } else if (mWithMeshClsn.IsOnWall() != 0) {
                void *wall = _ZNK10dBgCh_Actr13GetWallResultEv(&mWithMeshClsn);
                Vec3 normal;
                ((SurfaceInfo *)((char *)wall + 4))->CopyNormalTo(
                    *(Vector3 *)&normal);
                if (GetSubtraction(mPrevAngleY,
                        _ZN4cstd5atan2E5Fix12IiES1_(normal.x, normal.z))
                    > 0x4000)
                    Break();
            }
        }
    }

    AddVec3((Vec3 *)&mPosX, (Vec3 *)&unk_0a4, (Vec3 *)&mPosX);

    {
        u32 id = mCylinder.otherOwner;
        dActor_c *actor;
        int isPlayer;
        if (id == 0)
            return;
        actor = dActor_c::FindWithID(id);
        if (actor == 0)
            return;
        /* 0xbf is PLAYER (191). */
        isPlayer = (int)(actor->actorID == 0xbf);
        if (isPlayer == 0)
            return;
        isPlayer = ((Player *)actor)->mIsVanish;
        if (isPlayer != 0)
            return;
        {
            Vec3 pos;
            pos.x = mPosX;
            pos.y = mPosY;
            pos.z = mPosZ;
            _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(
                actor, &pos, 2, 0xc000, 1, 0, 1);
        }
    }
}

// @symbol _ZN9daChair_c6State3Ev
void daChair_c::State3()
{
    s16 *tracked = mTrackedAngle;
    int angle;
    s16 *ip;
    s16 sample;
    s16 wrapped;
    int offset;

    ApproachStateValue(tracked, &mStateValue0, mTargetAngle, 0xfa0, 0x14, 2);
    angle = AngleDiff(*mTrackedAngle, 0);
    ip = mTrackedAngle;
    sample = data_02082214[((u16)*ip >> 4) * 2];
    wrapped = (s16)angle;
    offset = (s16)sample * (s16)0x50;
    if (ip != &mAngleX) {
        mClsnOffset.x = -offset;
        mClsnOffset.y = 0;
        mClsnOffset.z = 0;
    } else {
        mClsnOffset.x = 0;
        mClsnOffset.y = 0;
        mClsnOffset.z = offset;
        goto place;
    }
place:
    if (wrapped >= 0x4000) {
        s16 past = (s16)(wrapped - 0x4000);
        s16 tip = data_02082214[((u16)past >> 4) * 2];
        mPosY = (s16)tip * (s16)0x28 + (mHomePos.y + 0x28000);
    } else {
        s16 tip = data_02082214[((u16)wrapped >> 4) * 2];
        mPosY = (s16)tip * (s16)0x28 + mHomePos.y;
    }
}
