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
 * RTTI names daFPknBall_c. One out-of-line destructor under defer_codegen
 * off emits D1, D0, then a homeless D2, and .text follows source order.
 * common.h is first so the particle helper copies the flat Matrix4x3.
 */
// What stays, and why:
// - Vec3_Asr, func_02012694 and func_ov002_020ad660 have no shared header,
//   so they keep a local extern "C" block.
// - Behavior's position copies use the file-local POD Vec3F. types.h's
//   Vector3 is not a POD.
// - The Bool enum and the volatile sparkPos stay. Behavior needs both.
// - Behavior adds virtual slot 29 to the spark's Y through the local Obj
//   table. The header name OnAimedAtWithEgg does not fit that use.
// - TryDropItem writes a byte at +0x42b on actor 279 so a second
//   cap is not dropped. Nothing here names that field.
// - mStateTimer is incremented through a u16 cast. The ROM loads it unsigned.

#pragma defer_codegen off

#include "common.h"
#include "Player.h"
#include "SaveData.h"
#include "Sound.h"
#include "daFPknBall_c.h"
#include "dBgCh_Actr.h"
#include "math/Approach.h"
#include "math/CalcLib.h"
#include "math/MtxStack.h"
#include "Particle__System.h"

typedef struct {
    s32 x, y, z;
} Vec3;

struct Vec3F {
    int x, y, z;
};
enum Bool { FALSE, TRUE };

/* Only slot 29 is called; the rest keep it at the right vtable offset. */
struct Obj {
    virtual int m00();
    virtual int m01();
    virtual int m02();
    virtual int m03();
    virtual int m04();
    virtual int m05();
    virtual int m06();
    virtual int m07();
    virtual int m08();
    virtual int m09();
    virtual int m10();
    virtual int m11();
    virtual int m12();
    virtual int m13();
    virtual int m14();
    virtual int m15();
    virtual int m16();
    virtual int m17();
    virtual int m18();
    virtual int m19();
    virtual int m20();
    virtual int m21();
    virtual int m22();
    virtual int m23();
    virtual int m24();
    virtual int m25();
    virtual int m26();
    virtual int m27();
    virtual int m28();
    virtual int m29();
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
enum { MFLAG_YOSHI_MOUTH_A = 0x20000, MFLAG_YOSHI_MOUTH_B = 0x40000 };

/* The ball's param1 & 7, named for what Behavior does with it. Variant 3 is
 * what daFPkn_c spawns; other values are not distinguished here. */
enum { VARIANT_DROPS_ITEM = 0, VARIANT_KEEPS_HEADING = 3, VARIANT_COIN_DROP = 4 };

/* dCc_c::hitFlags bit, per the table in dCc_c.h. */
enum { HIT_MEGA_CHARACTER = 0x10 };

extern int data_0209e650;
extern s32 data_ov002_02100320[];
extern s32 data_ov002_02100334[];
extern s32 data_ov002_02100348[];
extern s16 data_02082214[];

/* local extern: arm9/ov002 C helpers with no shared header yet. Vec3_Asr's
   definition takes a plain triple, spelled here as this file's Vec3. */
extern "C" {
void Vec3_Asr(Vec3 *d, Vec3 *s, int sh);
int func_ov002_020ad660(void *cc, void *pp, void *r5p, int flags);
void func_02012694(unsigned int, const Vector3 *);
}

// @symbol _ZN12daFPknBall_cD1Ev
// @symbol _ZN12daFPknBall_cD0Ev
/* D1: own vptr, then the members in reverse declaration order (dExtShadowModel_c,
 * dBgCh_Actr, dCcAc_c), then dEnemyBase_c's destructor. D0 is the same body
 * plus the inherited operator delete; it has no source of its own. */
daFPknBall_c::~daFPknBall_c() {}

// @symbol _ZN12daFPknBall_c11TryDropCoinEv
/* Roll 0..9; on 0..3 spawn a coin (actor 288) at the ball's position and zero
 * its velocity words. Called when a variant-4 ball ends by range or water. */
void daFPknBall_c::TryDropCoin() {
    dActor_c *coin;
    if (((unsigned int)RandomIntInternal(&data_0209e650) >> 0x10) % 10 >= 4)
        return;
    coin = dActor_c::Spawn(ACTOR_COIN, 0, mPos, 0, mAreaId, -1);
    if (coin != 0) {
        coin->mSpeedX = 0;
        coin->mVertSpeed = 0;
        coin->mSpeedZ = 0;
    }
}

// @symbol _ZN12daFPknBall_c11TryDropItemEv
/* Death drop of a variant-0 ball. Rolls 0..9. On 4..9, gives up if any
 * FPAKUN_BALL already exists. Otherwise, if the closest player's param1 is 3
 * and the actor 279 has not yet handed one out, spawns a cap (actor 269) for a
 * random unlocked character 0..2 and marks actor 279's byte at +0x42b. If none
 * of that happens, a roll below 4 spawns a coin as TryDropCoin does. */
void daFPknBall_c::TryDropItem() {
    u8 roll = (u8)(((u32)RandomIntInternal(&data_0209e650) >> 0x10) % 10);
    if (roll >= 4) {
        if (ClosestWithActorID(ACTOR_FPAKUN_BALL) != 0)
            return;
    }
    {
        Player *player = ClosestPlayer();
        char *koopa;
        if (player != 0 && player->param1 == 3 &&
            (koopa = (char *)dActor_c::FindWithActorID(ACTOR_KOOPA, 0)) != 0 &&
            *(u8 *)(koopa + 0x42b) == 0) {
            int c;
            int idx;
            int unlocked = 0;
            for (c = 0; c < 3; c++) {
                if (SaveData::IsCharacterUnlocked(c) != 0) {
                    unlocked = (unlocked | (1 << c)) & 0xff;
                }
            }
            do {
                idx = ((u32)RandomIntInternal(&data_0209e650) >> 0x10) % 3;
            } while ((unlocked & (1 << idx)) == 0);
            if (dActor_c::Spawn(ACTOR_OBJ_MARIO_CAP, (idx << 8) | 0xb, mPos, &mAngle, mAreaId,
                                -1) != 0) {
                *(u8 *)(koopa + 0x42b) = 1;
                return;
            }
        }
    }
    if (roll >= 4)
        return;
    {
        dActor_c *coin = dActor_c::Spawn(ACTOR_COIN, 0, mPos, 0, mAreaId, -1);
        if (coin != 0) {
            coin->mSpeedX = 0;
            coin->mVertSpeed = 0;
            coin->mSpeedZ = 0;
        }
    }
}

// @symbol _ZN12daFPknBall_c13UpdateVisualsEv
/* Per-frame visuals: refresh the two trail particles at the ball's position
 * (x mirrored when mMirrored is set), move the shadow matrix to that
 * position >> 3, and draw the drop shadow. */
void daFPknBall_c::UpdateVisuals() {
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

    mTrailEffect = Particle::System::NewUnkCallback818(mTrailEffect, data_ov002_02100334[mVariant],
                                                       pos.x, pos.y, pos.z, 0);

    mSparkEffect = (void *)Particle::System::New((u32)mSparkEffect, data_ov002_02100348[mVariant],
                                                 pos.x, pos.y, pos.z, 0, 0);

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
    DropShadowRadHeight(mShadowModel, mShadowMat, 0x28000, 0x64000, 0xf);
}

// @symbol _ZN12daFPknBall_c6RenderEv
int daFPknBall_c::Render() {
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
 * d_enemy_base.cpp, where the real Vector3 costs nothing).
 */
int daFPknBall_c::Behavior() {
    struct Vec3F dustPos1;
    volatile struct Vec3F sparkPos;
    struct Vec3F dustPos2, dustPos3, dustArg1, dustArg2, dustArg3;
    int flags;
    int clsnResult;
    void *hitActor;
    int flatSpeed;
    int hitByID;

    flags = mFlags;
    {
        enum Bool b = (enum Bool)((flags & MFLAG_YOSHI_MOUTH_A) != 0);
        if (b != FALSE) {
            UpdateVisuals();
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
            ((int *)&dustArg1)[0] = ((int *)&dustPos1)[0];
            ((int *)&dustArg1)[1] = ((int *)&dustPos1)[1];
            ((int *)&dustArg1)[2] = ((int *)&dustPos1)[2];
            DisappearPoofDustAt(*(Vector3 *)&dustArg1);
        } else {
            UpdateVisuals();
        }
        return 1;
    }

    mTargetPlayer = ClosestPlayer();
    if (mTargetPlayer != 0 && mVariant != VARIANT_KEEPS_HEADING) {
        mTargetAngleY = Vec3_HorzAngle(&mPos, &mTargetPlayer->mPos);
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
                    mPrevAngleY = Vec3_HorzAngle(&((dActor_c *)hitActor)->mPos, &mPos);
                    mHorzSpeed = 0xa000;
                    mVertSpeed = 0x28000;
                    mDeathTimer = 0x1e;
                    mSpinRateX = 0;
                    mSpinRateY = 0;
                    mSpinRateZ = 0;
                    mDeathState = 8;
                    mVertAccel = -0x2000;
                    mTerminalVelocity = -0x32000;
                    Sound::PlayBank0(9, mCamSpacePos);
                    sparkPos.x = mPosX;
                    sparkPos.y = mPosY;
                    sparkPos.z = mPosZ;
                    {
                        int slot29 = ((Obj *)this)->m29();
                        int px, py, pz;
                        py = sparkPos.y;
                        px = sparkPos.x;
                        py = py + slot29;
                        pz = sparkPos.z;
                        sparkPos.y = py;
                        Particle::System::NewSimple(0x43, px, py, pz);
                    }
                    Particle::System::NewSimple(0x44, sparkPos.x, sparkPos.y, sparkPos.z);
                    UpdateVisuals();
                    mdCcAc_c.Clear();
                    return 1;
                }
                {
                    enum Bool hitPlayer =
                        (enum Bool)(((Player *)hitActor)->actorID == ACTOR_PLAYER);
                    if (hitPlayer != FALSE && ((Player *)hitActor)->mIsMetal == 0 &&
                        ((Player *)hitActor)->mIsVanish == 0) {
                        ((Player *)hitActor)->Burn();
                        {
                            int x = mPosX;
                            dustPos2.x = x;
                            dustPos2.y = mPosY;
                            dustPos2.z = mPosZ;
                            if (mMirrored != 0)
                                dustPos2.x = x * (u32)-1;
                            dustPos2.y += 0x50000;
                            ((int *)&dustArg2)[0] = ((int *)&dustPos2)[0];
                            ((int *)&dustArg2)[1] = ((int *)&dustPos2)[1];
                            ((int *)&dustArg2)[2] = ((int *)&dustPos2)[2];
                            DisappearPoofDustAt(*(Vector3 *)&dustArg2);
                        }
                        if (mVariant == VARIANT_COIN_DROP)
                            func_02012694(0x157, &mCamSpacePos);
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
    mSpeedX = (flatSpeed * data_02082214[((u16)mPrevAngleY >> 4) * 2]) / 4096;
    mVertSpeed = (-mHorzSpeed * data_02082214[((u16)mPrevAngleX >> 4) * 2]) / 4096;
    mSpeedZ = (flatSpeed * data_02082214[((u16)mPrevAngleY >> 4) * 2 + 1]) / 4096;
    mPosX += mSpeedX;
    mPosY += mVertSpeed;
    mPosZ += mSpeedZ;

    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    mDistanceFlown += mHorzSpeed;

    if (mDistanceFlown > mMaxDistance || mWithMeshClsn.IsOnWall() != 0 ||
        mWithMeshClsn.TouchesWater() != 0) {
        u8 variant = mVariant;
        if (variant == VARIANT_DROPS_ITEM) {
            TryDropItem();
        } else if (variant == VARIANT_COIN_DROP && mWithMeshClsn.IsOnWall() == 0) {
            TryDropCoin();
        }
        {
            int x = mPosX;
            dustPos3.x = x;
            dustPos3.y = mPosY;
            dustPos3.z = mPosZ;
            if (mMirrored != 0)
                dustPos3.x = x * (u32)-1;
            dustPos3.y += 0x50000;
            ((int *)&dustArg3)[0] = ((int *)&dustPos3)[0];
            ((int *)&dustArg3)[1] = ((int *)&dustPos3)[1];
            ((int *)&dustArg3)[2] = ((int *)&dustPos3)[2];
            DisappearPoofDustAt(*(Vector3 *)&dustArg3);
        }
        if (mVariant == VARIANT_COIN_DROP)
            func_02012694(0x157, &mCamSpacePos);
        MarkForDestruction();
    }

    /* The ROM loads this halfword unsigned, so not the s16 mStateTimer += 1. */
    *(u16 *)&mStateTimer += 1;
    UpdateVisuals();
    return 1;
}

// @symbol _ZN12daFPknBall_c13InitResourcesEv
int daFPknBall_c::InitResources() {
    if (mShadowModel.InitCylinder() == 0)
        return 0;
    mdCcAc_c.Init(this, 0x28000, 0x50000, 0x200002, 0);
    mWithMeshClsn.Init(this, 0x32000, 0x32000, 0, 0);
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

/* FPAKUN_BALL registry factory. */
// @symbol daFPknBall_c_classInit
extern "C" daFPknBall_c *daFPknBall_c_classInit() {
    return new daFPknBall_c();
}
