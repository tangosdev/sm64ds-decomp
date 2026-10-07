//cpp
/* daFPknBall_c (FPAKUN_BALL, actor 254): the fireball the fire piranha plant
 * spits. daFPkn_c hands it to dActor_c::SpawnFireball with param1 = 3. It
 * flies along its heading until it has covered mMaxDistance (1500 units),
 * burns a Player it touches, and is knocked away when its collider reports
 * hitFlags bit 0x10 (mega character, by dCc_c.h's unverified table).
 * On expiry, a wall or water it puffs into dust and destroys itself.
 * ov002 0x020f8858..0x020f934c, 10 functions; the last is the registry
 * factory daFPknBall_c_classInit (0x020f9304), `new daFPknBall_c()`.
 *
 * NAME: RTTI names daFPknBall_c. One out-of-line destructor under
 * #pragma defer_codegen off emits D1, D0, then a homeless D2, and .text
 * follows source order. common.h is first so the flat Matrix4x3 is what
 * the particle helper copies.
 *
 * Known limits:
 * - The Particle::System calls, dCcAc_c::Init, dBgCh_Actr::Init and
 *   dActor_c::DropShadowRadHeight pass Fix12<int> by value, so they stay
 *   mangled calls (dActor_c.h notes why such methods are never defined as
 *   members). The dBgCh_Actr::UpdateContinuous veneer, TouchesWater,
 *   SaveData::IsCharacterUnlocked, func_02012694 and func_ov002_020ad660
 *   have no declaration in a header.
 * - The file-local POD Vec3F is not types.h's Vector3, which is not a POD;
 *   the struct copies in Behavior are kept on the POD spelling from the
 *   byte-matching recovery. The choice is per function; re-measure before
 *   assuming it applies elsewhere.
 * - The Bool enum and the volatile sparkPos are codegen scaffolding that
 *   Behavior needs, kept from the byte-matching recovery and not re-measured
 *   in this pass. They mean nothing to the reader.
 * - Behavior adds dActor_c virtual slot 29 (the header calls it
 *   OnAimedAtWithEgg) to the spark's Y. That name does not fit this use, so
 *   the call goes through the local Obj table.
 * - func_ov002_020f897c sets a byte at +0x42b on the actor with ID 279 (0x117)
 *   so a second cap is not dropped; nothing here names that field.
 * - mStateTimer is incremented through a u16 cast: the ROM loads it unsigned.
 */

#pragma defer_codegen off

#include "common.h"
#include "Player.h"
#include "Sound.h"
#include "daFPknBall_c.h"

bool ApproachLinear(short &value, short target, short step);
int ApproachLinear(int &value, int target, int step);

typedef struct { s32 x, y, z; } Vec3;

struct Vector3_16f;

struct Vec3F { int x, y, z; };
enum Bool { FALSE, TRUE };

/* Only slot 29 is called; the rest keep it at the right vtable offset. */
struct Obj {
    virtual int m00(); virtual int m01(); virtual int m02(); virtual int m03();
    virtual int m04(); virtual int m05(); virtual int m06(); virtual int m07();
    virtual int m08(); virtual int m09(); virtual int m10(); virtual int m11();
    virtual int m12(); virtual int m13(); virtual int m14(); virtual int m15();
    virtual int m16(); virtual int m17(); virtual int m18(); virtual int m19();
    virtual int m20(); virtual int m21(); virtual int m22(); virtual int m23();
    virtual int m24(); virtual int m25(); virtual int m26(); virtual int m27();
    virtual int m28(); virtual int m29();
};

/* Actor IDs, from symbols/actor_debug_names.tsv. */
enum {
    ACTOR_FPAKUN_BALL = 254,
    ACTOR_KOOPA = 279,
    ACTOR_OBJ_MARIO_CAP = 269,
    ACTOR_COIN = 288,
    ACTOR_PLAYER = 191
};

/* dActor_c::mFlags bits the framework leaves to actor code. */
enum {
    MFLAG_YOSHI_MOUTH_A = 0x20000,
    MFLAG_YOSHI_MOUTH_B = 0x40000
};

/* The ball's param1 & 7, named for what Behavior does with it. Variant 3 is
 * what daFPkn_c spawns; other values are not distinguished here. */
enum {
    VARIANT_DROPS_ITEM = 0,
    VARIANT_KEEPS_HEADING = 3,
    VARIANT_COIN_DROP = 4
};

/* dCc_c::hitFlags bit, per the table in dCc_c.h. */
enum {
    HIT_MEGA_CHARACTER = 0x10
};

extern "C" {
extern int RandomIntInternal(int* seed);
extern int data_0209e650;
extern int _ZN8SaveData19IsCharacterUnlockedEj(u32 c);
extern u32 _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(u32 a, u32 b, Fix12i c, Fix12i d, Fix12i e, const struct Vector3_16f* f);
extern void* _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(u32 a, u32 b, Fix12i c, Fix12i d, Fix12i e, const void* f, void* g);
extern void Vec3_Asr(Vec3* d, Vec3* s, int sh);
extern void Matrix4x3_FromTranslation(Matrix4x3* m, s32 x, s32 y, s32 z);
extern void _ZN8dActor_c19DropShadowRadHeightER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_j(void* thiz, void* sm, void* mtx, Fix12i f, Fix12i a, u32 b);
extern s32 data_ov002_02100320[];
extern s32 data_ov002_02100334[];
extern s32 data_ov002_02100348[];
extern Matrix4x3 data_020a0e68;
int func_ov002_020ad660(void* cc, void* pp, void* r5p, int flags);
s16 Vec3_HorzAngle(const void* a, const void* b);
void _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(u32 id, s32 x, s32 y, s32 z);
void func_02012694(u32 id, const void* v);
void dBgCh_Actr_UpdateContinuous_Veneer(void* p);
int _ZNK10dBgCh_Actr12TouchesWaterEv(void* self);
extern s16 data_02082214[];
extern void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void* thiz, void* actor, int fix12, int t, unsigned int a, unsigned int b);
extern void _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(void* thiz, void* actor, int fix12, int t, void* vec, int last);
}

// @symbol _ZN12daFPknBall_cD1Ev
// @symbol _ZN12daFPknBall_cD0Ev
/* D1: own vptr, then the members in reverse declaration order (dExtShadowModel_c,
 * dBgCh_Actr, dCcAc_c), then dEnemyBase_c's destructor. D0 is the same body
 * plus the inherited operator delete; it has no source of its own. */
daFPknBall_c::~daFPknBall_c()
{
}

// @symbol _ZN12daFPknBall_c19func_ov002_020f88ecEv
/* Roll 0..9; on 0..3 spawn a coin (actor 288) at the ball's position and zero
 * its velocity words. Called when a variant-4 ball ends by range or water. */
void daFPknBall_c::func_ov002_020f88ec()
{
    dActor_c* coin;
    if (((unsigned int)RandomIntInternal(&data_0209e650) >> 0x10) % 10 >= 4) return;
    coin = dActor_c::Spawn(ACTOR_COIN, 0, *(Vector3*)&mPosX, 0, mAreaId, -1);
    if (coin != 0) {
        coin->unk_0a4 = 0;
        coin->mVertSpeed = 0;
        coin->unk_0ac = 0;
    }
}

// @symbol _ZN12daFPknBall_c19func_ov002_020f897cEv
/* Death drop of a variant-0 ball. Rolls 0..9. On 4..9, gives up if any
 * FPAKUN_BALL already exists. Otherwise, if the closest player's param1 is 3
 * and the actor 279 has not yet handed one out, spawns a cap (actor 269) for a
 * random unlocked character 0..2 and marks actor 279's byte at +0x42b. If none
 * of that happens, a roll below 4 spawns a coin as func_ov002_020f88ec does. */
void daFPknBall_c::func_ov002_020f897c()
{
    u8 roll = (u8)(((u32)RandomIntInternal(&data_0209e650) >> 0x10) % 10);
    if (roll >= 4) {
        if (ClosestWithActorID(ACTOR_FPAKUN_BALL) != 0) return;
    }
    {
        Player* player = ClosestPlayer();
        char* koopa;
        if (player != 0 && player->param1 == 3 &&
            (koopa = (char*)dActor_c::FindWithActorID(ACTOR_KOOPA, 0)) != 0 &&
            *(u8*)(koopa + 0x42b) == 0) {
            int c;
            int idx;
            int unlocked = 0;
            for (c = 0; c < 3; c++) {
                if (_ZN8SaveData19IsCharacterUnlockedEj(c) != 0) {
                    unlocked = (unlocked | (1 << c)) & 0xff;
                }
            }
            do {
                idx = ((u32)RandomIntInternal(&data_0209e650) >> 0x10) % 3;
            } while ((unlocked & (1 << idx)) == 0);
            if (dActor_c::Spawn(ACTOR_OBJ_MARIO_CAP, (idx << 8) | 0xb,
                    *(Vector3*)&mPosX, (Vector3_16*)&mAngleX,
                    mAreaId, -1) != 0) {
                *(u8*)(koopa + 0x42b) = 1;
                return;
            }
        }
    }
    if (roll >= 4) return;
    {
        dActor_c* coin = dActor_c::Spawn(ACTOR_COIN, 0,
            *(Vector3*)&mPosX, 0, mAreaId, -1);
        if (coin != 0) {
            coin->unk_0a4 = 0;
            coin->mVertSpeed = 0;
            coin->unk_0ac = 0;
        }
    }
}

// @symbol _ZN12daFPknBall_c19func_ov002_020f8b24Ev
/* Per-frame visuals: refresh the two trail particles at the ball's position
 * (x mirrored when mMirrored is set), move the shadow matrix to that
 * position >> 3, and draw the drop shadow. */
void daFPknBall_c::func_ov002_020f8b24()
{
    Vec3 pos;
    Vec3 shadowPos;

    {
        s32 x = mPosX;
        pos.x = x;
        pos.y = mPosY;
        pos.z = mPosZ;
        if (mMirrored != 0)
            pos.x = x * (u32)-1;
    }
    pos.y = pos.y + data_ov002_02100320[mVariant];

    mTrailEffect = _ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
        mTrailEffect, data_ov002_02100334[mVariant], pos.x, pos.y, pos.z, 0);

    mSparkEffect = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        (u32)mSparkEffect, data_ov002_02100348[mVariant], pos.x, pos.y, pos.z, 0, 0);

    {
        s32 x = mPosX;
        pos.x = x;
        pos.y = mPosY;
        pos.z = mPosZ;
        if (mMirrored != 0)
            pos.x = x * (u32)-1;
    }
    Vec3_Asr(&shadowPos, &pos, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, shadowPos.x, shadowPos.y, shadowPos.z);
    mShadowMat = data_020a0e68;
    _ZN8dActor_c19DropShadowRadHeightER17dExtShadowModel_cR9Matrix4x35Fix12IiES5_j(
        this, &mShadowModel, &mShadowMat, 0x28000, 0x64000, 0xf);
}

// @symbol _ZN12daFPknBall_c6RenderEv
int daFPknBall_c::Render()
{
    return 1;
}

// @symbol _ZN12daFPknBall_c8BehaviorEv
/* _ZN12daFPknBall_c8BehaviorEv at 0x020f8c94.
 *
 * Order of business: with the first Yoshi-mouth flag set only the visuals run;
 * with the second nothing runs. Then the clsn result: 2 puffs into dust, any
 * other non-zero result just refreshes the visuals. Then aim (at the closest
 * player unless the variant is 3), handle whatever hit the collider, steer,
 * move, and die on range, wall or water.
 *
 * The file-local POD Vec3F stands in for types.h's Vector3, which is not a
 * POD: a struct copy of it scalarises differently, and this function needs the
 * POD's. The choice is per function and has to be measured (see the note in
 * dEnemyBase_c.cpp, where the real Vector3 costs nothing).
 */
int daFPknBall_c::Behavior() {
    struct Vec3F dustPos1;
    volatile struct Vec3F sparkPos;
    struct Vec3F dustPos2, dustPos3, dustArg1, dustArg2, dustArg3;
    int flags;
    int clsnResult;
    void* hitActor;
    int flatSpeed;
    int hitByID;

    flags = mFlags;
    {
        enum Bool b = (enum Bool)((flags & MFLAG_YOSHI_MOUTH_A) != 0);
        if (b != FALSE) {
            func_ov002_020f8b24();
            return 1;
        }
    }
    {
        enum Bool b = (enum Bool)((flags & MFLAG_YOSHI_MOUTH_B) != 0);
        if (b != FALSE) {
            return 1;
        }
    }

    clsnResult = func_ov002_020ad660(this, &mWithMeshClsn, 0, 2);
    if (clsnResult != 0) {
        if (clsnResult == 2) {
            int x = mPosX;
            dustPos1.x = x;
            dustPos1.y = mPosY;
            dustPos1.z = mPosZ;
            if (mMirrored != 0)
                dustPos1.x = x * (u32)-1;
            dustPos1.y += 0x50000;
            ((int*)&dustArg1)[0] = ((int*)&dustPos1)[0];
            ((int*)&dustArg1)[1] = ((int*)&dustPos1)[1];
            ((int*)&dustArg1)[2] = ((int*)&dustPos1)[2];
            DisappearPoofDustAt(*(Vector3*)&dustArg1);
        } else {
            func_ov002_020f8b24();
        }
        return 1;
    }

    mTargetPlayer = ClosestPlayer();
    if (mTargetPlayer != 0 && mVariant != VARIANT_KEEPS_HEADING) {
        mTargetAngleY = Vec3_HorzAngle(&mPosX, &mTargetPlayer->mPosX);
    } else {
        mTargetAngleY = mAngleY;
    }

    hitByID = mdCcAc_c.otherOwner;
    if (hitByID != 0) {
        if ((mdCcAc_c.hitFlags & 0x8000) == 0) {
            hitActor = dActor_c::FindWithID((u32)hitByID);
            if (hitActor != 0) {
                if (mdCcAc_c.hitFlags & HIT_MEGA_CHARACTER) {
                    /* Knocked away from the hitter: a short hop into death state 8. */
                    mFlags &= ~0x10000001;
                    mPrevAngleY = Vec3_HorzAngle((char*)hitActor + 0x5c, &mPosX);
                    mHorzSpeed = 0xa000;
                    mVertSpeed = 0x28000;
                    mDeathTimer = 0x1e;
                    mSpinRateX = 0;
                    mSpinRateY = 0;
                    mSpinRateZ = 0;
                    mDeathState = 8;
                    mVertAccel = -0x2000;
                    mTerminalVelocity = -0x32000;
                    Sound::PlayBank0(9, *(Vector3*)&mCamSpacePosX);
                    sparkPos.x = mPosX;
                    sparkPos.y = mPosY;
                    sparkPos.z = mPosZ;
                    {
                        int slot29 = ((Obj*)this)->m29();
                        int px, py, pz;
                        py = sparkPos.y;
                        px = sparkPos.x;
                        py = py + slot29;
                        pz = sparkPos.z;
                        sparkPos.y = py;
                        _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x43, px, py, pz);
                    }
                    _ZN8Particle6System9NewSimpleEj5Fix12IiES2_S2_(0x44, sparkPos.x, sparkPos.y, sparkPos.z);
                    func_ov002_020f8b24();
                    mdCcAc_c.Clear();
                    return 1;
                }
                {
                    enum Bool hitPlayer = (enum Bool)(((Player*)hitActor)->actorID == ACTOR_PLAYER);
                    if (hitPlayer != FALSE && ((Player*)hitActor)->mIsMetal == 0 && ((Player*)hitActor)->mIsVanish == 0) {
                        ((Player*)hitActor)->Burn();
                        {
                            int x = mPosX;
                            dustPos2.x = x;
                            dustPos2.y = mPosY;
                            dustPos2.z = mPosZ;
                            if (mMirrored != 0)
                                dustPos2.x = x * (u32)-1;
                            dustPos2.y += 0x50000;
                            ((int*)&dustArg2)[0] = ((int*)&dustPos2)[0];
                            ((int*)&dustArg2)[1] = ((int*)&dustPos2)[1];
                            ((int*)&dustArg2)[2] = ((int*)&dustPos2)[2];
                            DisappearPoofDustAt(*(Vector3*)&dustArg2);
                        }
                        if (mVariant == VARIANT_COIN_DROP)
                            func_02012694(0x157, &mCamSpacePosX);
                        MarkForDestruction();
                    }
                }
            }
        } else {
            mdCcAc_c.flags |= 1;
        }
    }

    dBgCh_Actr_UpdateContinuous_Veneer(&mWithMeshClsn);
    if (ApproachLinear(mHorzSpeed, mTargetSpeed, 0x999) != 0) {
        ApproachLinear(mPrevAngleY, mTargetAngleY, 0x200);
    }

    /* mPrevAngleY is the heading (ApproachLinear steers it toward
     * mTargetAngleY) and mPrevAngleX the pitch; data_02082214 holds (sin, cos)
     * pairs in fix12. */
    flatSpeed = (mHorzSpeed * data_02082214[((u16)mPrevAngleX >> 4) * 2 + 1]) / 4096;
    unk_0a4 = (flatSpeed * data_02082214[((u16)mPrevAngleY >> 4) * 2]) / 4096;
    mVertSpeed = (-mHorzSpeed * data_02082214[((u16)mPrevAngleX >> 4) * 2]) / 4096;
    unk_0ac = (flatSpeed * data_02082214[((u16)mPrevAngleY >> 4) * 2 + 1]) / 4096;
    mPosX += unk_0a4;
    mPosY += mVertSpeed;
    mPosZ += unk_0ac;

    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    mDistanceFlown += mHorzSpeed;

    if (mDistanceFlown > mMaxDistance
        || mWithMeshClsn.IsOnWall() != 0
        || _ZNK10dBgCh_Actr12TouchesWaterEv(&mWithMeshClsn) != 0) {
        u8 variant = mVariant;
        if (variant == VARIANT_DROPS_ITEM) {
            func_ov002_020f897c();
        } else if (variant == VARIANT_COIN_DROP && mWithMeshClsn.IsOnWall() == 0) {
            func_ov002_020f88ec();
        }
        {
            int x = mPosX;
            dustPos3.x = x;
            dustPos3.y = mPosY;
            dustPos3.z = mPosZ;
            if (mMirrored != 0)
                dustPos3.x = x * (u32)-1;
            dustPos3.y += 0x50000;
            ((int*)&dustArg3)[0] = ((int*)&dustPos3)[0];
            ((int*)&dustArg3)[1] = ((int*)&dustPos3)[1];
            ((int*)&dustArg3)[2] = ((int*)&dustPos3)[2];
            DisappearPoofDustAt(*(Vector3*)&dustArg3);
        }
        if (mVariant == VARIANT_COIN_DROP)
            func_02012694(0x157, &mCamSpacePosX);
        MarkForDestruction();
    }

    /* The ROM loads this halfword unsigned, so not the s16 mStateTimer += 1. */
    *(u16*)&mStateTimer += 1;
    func_ov002_020f8b24();
    return 1;
}

// @symbol _ZN12daFPknBall_c13InitResourcesEv
int daFPknBall_c::InitResources()
{
    if (mShadowModel.InitCylinder() == 0)
        return 0;
    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(&mdCcAc_c, this, 0x28000, 0x50000, 0x200002, 0);
    _ZN10dBgCh_Actr4InitEP8dActor_c5Fix12IiES3_P10Vector3_16S5_(&mWithMeshClsn, this, 0x32000, 0x32000, 0, 0);
    mWithMeshClsn.StartDetectingWater();
    mStateTimer = 0;
    unk_36a = 0;
    mDistanceFlown = 0;
    mMaxDistance = 0x5dc000;
    mVariant = param1 & 7;
    mTrailEffect = 0;
    mSparkEffect = 0;
    {
        unsigned char v = mVariant;
        if (v != VARIANT_DROPS_ITEM && v != VARIANT_COIN_DROP) {
            mdCcAc_c.vulnFlags |= 0x8000;
        }
    }
    return 1;
}

// @symbol _ZN12daFPknBall_c13OnYoshiTryEatEv
s32 daFPknBall_c::OnYoshiTryEat() {
    unsigned char b = mVariant;
    if (b != VARIANT_DROPS_ITEM && b != VARIANT_COIN_DROP)
        return 5;
    return 0;
}

/* Reconstructed source-style name: SM64DS proves daFPknBall_c through RTTI,
 * allocation size, vtable identity, and the FPAKUN_BALL registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: Fireball_Spawn. */
// @symbol daFPknBall_c_classInit
extern "C" daFPknBall_c *daFPknBall_c_classInit()
{
    return new daFPknBall_c();
}
