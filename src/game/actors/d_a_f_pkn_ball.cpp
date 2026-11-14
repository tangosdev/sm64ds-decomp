//cpp
/* daFPknBall_c (FPAKUN_BALL, actor 254): the fireball the fire piranha plant
 * spits. daFPkn_c hands it to dActor_c::SpawnFireball with param1 = 3. It
 * flies along its heading until it has covered mMaxDistance (1500 units),
 * burns a daPly_c it touches, and is knocked away when its collider reports
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
// - Behavior tests the two Yoshi-mouth flags and the hitter's actor ID
//   through int flag locals. Tested inline, the branches change shape.
// - TryDropItem writes a byte at +0x42b on actor 279 so a second
//   cap is not dropped. Nothing here names that field.

#pragma defer_codegen off

#include "common.h"
#include "daKpa_c.h"
#include "daPly_c.h"
#include "SaveData.h"
#include "Sound.h"
#include "daFPknBall_c.h"
#include "daKpa_c.h"
#include "dBgCh_Actr.h"
#include "math/Approach.h"
#include "math/CalcLib.h"
#include "math/MtxStack.h"
#include "Particle__System.h"
#include "ParticleIds.h"
#include "nitro/fx/fx.h"
#include "LevelRng.h"
#include "nitro/fx/fx_trig.h"

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

extern s32 data_ov002_02100320[];
extern s32 data_ov002_02100334[];
extern s32 data_ov002_02100348[];

/* local extern: arm9/ov002 C helpers with no shared header yet. Vec3_Asr is
   not among them any more -- math/CalcLib.h, included above, declares it. */
int func_ov002_020ad660(void *cc, void *pp, void *r5p, int flags);

void func_02012694(unsigned int, const Vector3 *);

/* D1: own vptr, then the members in reverse declaration order (dExtShadowModel_c,
 * dBgCh_Actr, dCcAc_c), then dEnemyBase_c's destructor. D0 is the same body
 * plus the inherited operator delete; it has no source of its own. */
daFPknBall_c::~daFPknBall_c() {}

/* Roll 0..9; on 0..3 spawn a coin (actor 288) at the ball's position and zero
 * its velocity words. Called when a variant-4 ball ends by range or water. */
void daFPknBall_c::TryDropCoin() {
    dActor_c *coin;
    if (((unsigned int)RandomIntInternal(&data_0209e650.value) >> 0x10) % 10 >= 4)
        return;
    coin = dActor_c::Spawn(ACTOR_COIN, 0, mPos, 0, mAreaId, -1);
    if (coin != 0) {
        coin->mSpeedX = 0;
        coin->mVertSpeed = 0;
        coin->mSpeedZ = 0;
    }
}

/* Death drop of a variant-0 ball. Rolls 0..9. On 4..9, gives up if any
 * FPAKUN_BALL already exists. Otherwise, if the closest player's param1 is 3
 * and the actor 279 has not yet handed one out, spawns a cap (actor 269) for a
 * random unlocked character 0..2 and sets Bowser's mCapActorAlive. If none
 * of that happens, a roll below 4 spawns a coin as TryDropCoin does. */
void daFPknBall_c::TryDropItem() {
    u8 roll = (((u32)RandomIntInternal(&data_0209e650.value) >> 0x10) % 10);
    if (roll >= 4) {
        if (ClosestWithActorID(ACTOR_FPAKUN_BALL) != 0)
            return;
    }
    {
        daPly_c *player = ClosestPlayer();
        daKpa_c *koopa;
        if (player != 0 && player->param1 == 3 &&
            (koopa = (daKpa_c *)dActor_c::FindWithActorID(ACTOR_KOOPA, 0)) != 0 &&
            koopa->mCapActorAlive == 0) {
            int c;
            int idx;
            int unlocked = 0;
            for (c = 0; c < 3; c++) {
                if (SaveData::IsCharacterUnlocked(c) != 0) {
                    unlocked = (unlocked | (1 << c)) & 0xff;
                }
            }
            do {
                idx = ((u32)RandomIntInternal(&data_0209e650.value) >> 0x10) % 3;
            } while ((unlocked & (1 << idx)) == 0);
            if (dActor_c::Spawn(ACTOR_OBJ_MARIO_CAP, (idx << 8) | 0xb, mPos, &mAngle, mAreaId,
                                -1) != 0) {
                koopa->mCapActorAlive = 1;
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

/* Per-frame visuals: refresh the two trail particles at the ball's position
 * (x mirrored when mMirrored is set), move the shadow matrix to that
 * position >> 3, and draw the drop shadow. */
void daFPknBall_c::UpdateVisuals() {
    Vector3 pos;
    Vector3 shadowPos;

    pos = mPos;
    if (mMirrored != 0)
        pos.x *= -1;
    pos.y += data_ov002_02100320[mVariant];

    mTrailEffect = Particle::System::NewCleanParticle(mTrailEffect, data_ov002_02100334[mVariant],
                                                      pos.x, pos.y, pos.z, 0);

    mSparkEffect = (void *)Particle::System::New((u32)mSparkEffect, data_ov002_02100348[mVariant],
                                                 pos.x, pos.y, pos.z, 0, 0);

    pos = mPos;
    if (mMirrored != 0)
        pos.x *= -1;
    Vec3_Asr(&shadowPos, &pos, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, shadowPos.x, shadowPos.y, shadowPos.z);
    mShadowMat = data_020a0e68;
    DropShadowRadHeight(mShadowModel, mShadowMat, 0x28000, 0x64000, 0xf);
}

int daFPknBall_c::Render() {
    return 1;
}

/* _ZN12daFPknBall_c8BehaviorEv at 0x020f8c94.
 *
 * Order of business: with the first Yoshi-mouth flag set only the visuals run;
 * with the second nothing runs. Then the clsn result: 2 puffs into dust, any
 * other non-zero result just refreshes the visuals. Then aim (at the closest
 * player unless the variant is 3), handle whatever hit the collider, steer,
 * move, and die on range, wall or water.
 *
 * Each dust puff copies its position once more before the call (dustArgN):
 * passing dustPosN straight to DisappearPoofDustAt is 0x3c bytes short.
 */
int daFPknBall_c::Behavior() {
    Vector3 dustPos1;
    Vector3 sparkPos;
    Vector3 dustPos2, dustPos3, dustArg1, dustArg2, dustArg3;
    int flags;
    int clsnResult;
    void *hitActor;
    int flatSpeed;
    int hitByID;

    flags = mFlags;
    {
        int b = (flags & MFLAG_YOSHI_MOUTH_A) != 0;
        if (b) {
            UpdateVisuals();
            return 1;
        }
    }
    {
        int b = (flags & MFLAG_YOSHI_MOUTH_B) != 0;
        if (b)
            return 1;
    }

    clsnResult = func_ov002_020ad660(this, &mWithMeshClsn, 0, 2);
    if (clsnResult != 0) {
        if (clsnResult == 2) {
            dustPos1 = mPos;
            if (mMirrored != 0)
                dustPos1.x *= -1;
            dustPos1.y += 0x50000;
            dustArg1 = dustPos1;
            DisappearPoofDustAt(dustArg1);
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
            hitActor = dActor_c::FindWithID(hitByID);
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
                    sparkPos = mPos;
                    sparkPos.y += OnAimedAtWithEgg();
                    Particle::System::NewSimple(kPtclKnockAwayA, sparkPos.x, sparkPos.y,
                                                sparkPos.z);
                    Particle::System::NewSimple(kPtclKnockAwayB, sparkPos.x, sparkPos.y,
                                                sparkPos.z);
                    UpdateVisuals();
                    mdCcAc_c.Clear();
                    return 1;
                }
                {
                    int hitPlayer = ((daPly_c *)hitActor)->actorID == ACTOR_PLAYER;
                    if (hitPlayer && ((daPly_c *)hitActor)->mIsMetal == 0 &&
                        ((daPly_c *)hitActor)->mIsVanish == 0) {
                        ((daPly_c *)hitActor)->Burn();
                        {
                            dustPos2 = mPos;
                            if (mMirrored != 0)
                                dustPos2.x *= -1;
                            dustPos2.y += 0x50000;
                            dustArg2 = dustPos2;
                            DisappearPoofDustAt(dustArg2);
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
     * mTargetAngleY) and mPrevAngleX the pitch; FX_SinCosTable_ holds (sin, cos)
     * pairs in fix12. */
    flatSpeed = (mHorzSpeed * FX_CosIdx((u16)mPrevAngleX >> 4)) / 4096;
    mSpeedX = (flatSpeed * FX_SinIdx((u16)mPrevAngleY >> 4)) / 4096;
    mVertSpeed = (-mHorzSpeed * FX_SinIdx((u16)mPrevAngleX >> 4)) / 4096;
    mSpeedZ = (flatSpeed * FX_CosIdx((u16)mPrevAngleY >> 4)) / 4096;
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
            dustPos3 = mPos;
            if (mMirrored != 0)
                dustPos3.x *= -1;
            dustPos3.y += 0x50000;
            dustArg3 = dustPos3;
            DisappearPoofDustAt(dustArg3);
        }
        if (mVariant == VARIANT_COIN_DROP)
            func_02012694(0x157, &mCamSpacePos);
        MarkForDestruction();
    }

    mStateTimer += 1;
    UpdateVisuals();
    return 1;
}

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

s32 daFPknBall_c::OnYoshiTryEat() {
    unsigned char b = mVariant;
    if (b != VARIANT_DROPS_ITEM && b != VARIANT_COIN_DROP)
        return 5;
    return 0;
}

/* FPAKUN_BALL registry factory. */
daFPknBall_c *daFPknBall_c_classInit() {
    return new daFPknBall_c();
}
