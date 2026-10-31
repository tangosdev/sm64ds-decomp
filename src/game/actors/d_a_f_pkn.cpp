//cpp
/* daFPkn_c -- the fire piranha plant (PAKUN2 / FIREPAKUN_S / FIREPAKUN),
 * ov084.
 *
 * This TU is .text 0x0212d248..0x0212eaf0: the destructor pair, nine
 * helpers and states, the seven vtable methods, then the three classInit
 * registry factories. The base is dEnemyBase_c; the daPkn_c run begins
 * immediately after, at 0x0212eaf0.
 *
 * Behavior dispatches mState: STATE_INIT picks the group leader,
 * STATE_WAIT emerges and idles, STATE_SPIT lobs a fireball, STATE_GROW is
 * the grow-in lunge (PAKUN2 only) and STATE_DEFEATED does nothing.
 * CheckClsnHits runs the body and mouth colliders; a hit spins the plant
 * down in place, and the group leader spawns the star once five members
 * are defeated.
 *
 * `#pragma defer_codegen off` is load-bearing: the destructor defined
 * first emits D1 then D0 in parse order, as the cartridge has them.
 * That destructor is the key function, so this object owns its vtable
 * and RTTI.
 */
/* leftovers:
 * - Player::Hurt / Bounce stay mangled. Each takes Fix12<int> by value
 *   and the member spelling of those calls stops matching.
 * - CheckClsnHits, StateGrow and StateSpit keep their int locals and
 *   gotos. The direct spelling at each of those sites stops matching.
 * - mFlags bit 0x10000000 and the sound ids stay numeric. They are named
 *   by the site that uses them; no flag or sound table gives them a name.
 */

#include "common.h"
#include "daFPkn_c.h"
#include "daPkn_c.h"
#include "SharedFilePtr.h"
#include "Player.h"
#include "Sound.h"
#include "Particle__System.h"
#include "nitro/fx/fx.h"
#include "math/Approach.h"

#pragma defer_codegen off

/* The tag SubVec3.c defines its parameters by. Vec3_Asr.c takes Vector3
   now, so the extern below uses that. */
struct Vec3 { int x, y, z; };

/* UpdateClsnOffset's stack locals: three bone-angle sums, then the
   position shifted down by 3. */
struct ClsnOffsetLocals {
    s16 acc[3];
    int tmp[3];
};

/* Actor ids, from symbols/actor_debug_names.tsv. */
enum {
    ACTOR_STAR = 178,
    ACTOR_PLAYER = 191,
    ACTOR_PAKUN = 250,
    ACTOR_FIREPAKUN = 251,
    ACTOR_FIREPAKUN_S = 252,
    ACTOR_PAKUN2 = 253
};

/* mState, as Behavior dispatches it. State 4 has no case body: StateWait sets
   it after a group member is defeated with mRespawnMode == 1. */
enum {
    STATE_INIT = 0,
    STATE_WAIT = 1,
    STATE_SPIT = 2,
    STATE_GROW = 3,
    STATE_DEFEATED = 4
};

/* dActor_c::mFlags bits written by actor code (see the bit table in
   dActor_c.h). */
enum {
    MFLAG_YOSHI_MOUTH_A = 0x20000,
    MFLAG_YOSHI_MOUTH_B = 0x40000
};

/* dCc_c::hitFlags bits, with the names include/dCc_c.h gives its table. */
enum {
    HIT_MEGA_CHARACTER = 0x10,
    HIT_DAMAGE = 0x66ff0          /* every hit kind that defeats the plant */
};

/* Sound ids for func_0201267c / func_02012694 (bank-3 veneers that play an
   id at the actor's camera-space position) and Sound::PlayBank0. Named by
   where they are played; the sounds themselves are not identified. */
enum {
    SND_HIT_S = 0xa,             /* bank 0: FIREPAKUN_S hit / squashed */
    SND_JUMPED_ON = 0xb6,        /* bank 0: FIREPAKUN_S jumped on */
    SND_GROW_BITE = 0xc0,        /* the attack frames of the grow-in lunge */
    SND_SPIT_END_S = 0xe3,       /* FIREPAKUN_S finishes its spit */
    SND_EMERGE_S = 0x104,        /* FIREPAKUN_S emerges */
    SND_FIREBALL_S = 0x105,      /* FIREPAKUN_S lobs the fireball */
    SND_SPUN_DOWN = 0x11f,       /* the spin-down ends */
    SND_SPIT_END = 0x120,
    SND_EMERGE = 0x121,
    SND_FIREBALL = 0x122,
    SND_MEGA_KILL = 0x1d,        /* a mega player lands the kill */
    SND_HIT = 0x1e               /* FIREPAKUN is hit */
};


/* daPkn_c::EnterLunge, declared on the class's own header: hands the
   spawned PAKUN the grow-in frame. */

extern "C" {
void func_02012694(u32 id, const Vector3 *pos);
short Vec3_HorzAngle(const Vector3 *a, const Vector3 *b);
int IsStarCollectedInCurLevel(u32 flag);
void SetStarMarker(int i, void *self, int v);
void Matrix4x3_FromRotationY(void *m, int angle);
void Vec3_Asr(Vector3 *d, Vector3 *s, int sh);
void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
void MulMat4x3Mat4x3(const int *a, const int *b, int *d);
void Vec3_LslInPlace(void *v, int sh);
void SubVec3(Vector3 *a, Vector3 *b, Vector3 *c);
void LoadBlueCoinModel(void *c);
void UnloadBlueCoinModel(void *);

/* Called by their mangled names: each carries a Fix12<int> by value and
   the member spelling DIFFs. */
u32 d_a_f_pknNewHold(u32 a, u32 b, Fix12i c, Fix12i d, Fix12i e, const void *f, void *g);

extern SharedFilePtr data_ov084_02130dfc;   /* the plant's model */
extern SharedFilePtr *data_ov084_021302f4[];/* the six shared animation handles */
extern SharedFilePtr data_ov002_0210da38;   /* the sleep-bubble model, out of ov002 */
extern SharedFilePtr data_ov084_02130df4;   /* idle animation */
extern SharedFilePtr data_ov084_02130e04;   /* spit animation */
extern SharedFilePtr data_ov084_02130e14;   /* grow-in lunge animation */
extern void *data_ov084_02130e1c[];         /* post-spit animation; decl_common.h's spelling */
extern SharedFilePtr data_ov084_02130e24;   /* death animation */
extern int data_ov084_0213029c[];           /* SpawnDeathBurst reach per frame */
extern int data_ov084_021302c4[];           /* SpawnDeathBurst height per frame */
extern u8 data_ov084_02130294[];            /* UpdateClsnOffset's five bone indices */
extern s32 data_020a0e68[];                 /* the shared scratch matrix */
extern s16 data_02082214[];                 /* the sin and cos table, two shorts a step */
}

/* One vtable store and four destructor calls, every one a consequence of
 * `struct daFPkn_c : dEnemyBase_c` and the members that declaration types,
 * destroyed in reverse declaration order, then dEnemyBase_c::~dEnemyBase_c.
 * That body is the evidence for the header: each member's size closes
 * exactly on the next one's offset. D0 ends in dEnemyBase_c's inline
 * operator delete, reachable because dEnemyBase_c is the immediate base. */
// @symbol _ZN8daFPkn_cD1Ev
// @symbol _ZN8daFPkn_cD0Ev
daFPkn_c::~daFPkn_c()
{
}

// @symbol _ZN8daFPkn_c15SpawnDeathSmokeEv
void daFPkn_c::SpawnDeathSmoke()
{
    Particle::System *particle;
    if (mAnmModel.file != data_ov084_02130e24.filePtr)
        return;

    mParticleHandle1 =
        Particle::System::New(mParticleHandle1, 0xfc, mPosX, mPosY + 0x1e000, mPosZ, 0, 0);
    if (mParticleHandle1 != 0) {
        particle = Particle::System::FromUniqueID(mParticleHandle1);
        if (particle != 0) {
            particle->callbackScale = (short)FX_MUL(mMaxScale, 0x2800);
        }
    }

    mParticleHandle2 =
        Particle::System::New(mParticleHandle2, 0xfd, mPosX, mPosY + 0x1e000, mPosZ, 0, 0);
    if (mParticleHandle2 == 0)
        return;
    particle = Particle::System::FromUniqueID(mParticleHandle2);
    if (particle == 0)
        return;
    particle->callbackScale = (short)FX_MUL(mMaxScale, 0x2800);
}

// @symbol _ZN8daFPkn_c15SpawnDeathBurstEv
void daFPkn_c::SpawnDeathBurst()
{
    Vector3 pos;
    int frame, burstReach, product;

    if (mAnmModel.file != data_ov084_02130e24.filePtr)
        return;
    frame = (int)((unsigned)(mAnmModel.currFrame << 4) >> 16); /* whole frames */
    if (frame >= 0xa)
        return;

    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    burstReach = data_ov084_0213029c[frame];
    product = burstReach * data_02082214[((u16)mAngleY >> 4) * 2];
    pos.x = pos.x + FX_MUL(product, mScale);
    product = burstReach * data_02082214[((u16)mAngleY >> 4) * 2 + 1];
    pos.z = pos.z + FX_MUL(product, mScale);
    pos.y = pos.y + mScale * data_ov084_021302c4[frame];
    mParticleHandle1 = Particle::System::New(mParticleHandle1, 0xfb, pos.x, pos.y, pos.z, 0, 0);
}

/* Empty. StateWait passes this in r0 and nothing reads it. */
// @symbol _ZN8daFPkn_c21OnGroupMemberDefeatedEv
void daFPkn_c::OnGroupMemberDefeated()
{
}

// @symbol _ZN8daFPkn_c16UpdateClsnOffsetEv
void daFPkn_c::UpdateClsnOffset()
{
    struct ClsnOffsetLocals locals;
    int scaledSinAcc0, sinAcc1, cosAcc1, scaledCosAcc0, isFirepakun;

    Matrix4x3_FromRotationY(&mAnmModel.mat4x3, mAngleY);
    mAnmModel.mat4x3.m[9] = mPosX >> 3;
    mAnmModel.mat4x3.m[10] = mPosY >> 3;
    mAnmModel.mat4x3.m[11] = mPosZ >> 3;

    if ((u32)(mState - STATE_SPIT) > 1) return;
    if (mScale != mMaxScale) return;

    locals.acc[0] = 0;
    locals.acc[1] = 0;
    locals.acc[2] = 0;
    {
        char *base = (char *)mAnmModel.data.bones;
        int i;
        for (i = 0; i < 5; i++) {
            u8* p = data_ov084_02130294 + i;
            locals.acc[0] = (s16)(locals.acc[0] + *(s16*)(base + *p * 0x34 + 0x1a));
            locals.acc[1] = (s16)(locals.acc[1] + *(s16*)(base + *p * 0x34 + 0x1c));
            locals.acc[2] = (s16)(locals.acc[2] + *(s16*)(base + *p * 0x34 + 0x1e));
        }
    }

    mClsnOffset.x = 0;
    mClsnOffset.y = 0;
    mClsnOffset.z = 0;

    Vec3_Asr((Vector3 *)locals.tmp, (Vector3 *)&mPosX, 3);
    Matrix4x3_FromTranslation(
        data_020a0e68,
        locals.tmp[0],
        locals.tmp[1],
        locals.tmp[2]
    );
    MulMat4x3Mat4x3(mAnmModel.data.transforms[6].m, data_020a0e68, data_020a0e68);
    mClsnOffset.x = ((Matrix4x3 *)data_020a0e68)->m[9];
    mClsnOffset.y = ((Matrix4x3 *)data_020a0e68)->m[10];
    mClsnOffset.z = ((Matrix4x3 *)data_020a0e68)->m[11];
    Vec3_LslInPlace(&mClsnOffset, 3);
    SubVec3((Vector3 *)&mClsnOffset, (Vector3 *)&mPosX, (Vector3 *)&mClsnOffset);

    scaledSinAcc0 = data_02082214[((u16)locals.acc[0] >> 4) * 2] * 0x32;
    sinAcc1 = data_02082214[((u16)locals.acc[1] >> 4) * 2];

    mClsnOffset.x = mClsnOffset.x + FX_MUL(scaledSinAcc0, sinAcc1);

    /* The int isFirepakun keeps the ROM's compare; a direct test DIFFs. */
    isFirepakun = (actorID == ACTOR_FIREPAKUN);
    if (isFirepakun != false) {
        scaledCosAcc0 = data_02082214[((u16)locals.acc[0] >> 4) * 2 + 1] * 0x32;
        mClsnOffset.y = mClsnOffset.y - (0x19000 - scaledCosAcc0);
    } else {
        scaledCosAcc0 = data_02082214[((u16)locals.acc[0] >> 4) * 2 + 1] * 0x32;
        mClsnOffset.y = mClsnOffset.y - (0x32000 - scaledCosAcc0);
    }

    cosAcc1 = data_02082214[((u16)locals.acc[1] >> 4) * 2 + 1];
    mClsnOffset.z = mClsnOffset.z + FX_MUL(scaledSinAcc0, cosAcc1);

    mClsnOffset.x = FX_MUL(mClsnOffset.x, mScale);
    mClsnOffset.y = FX_MUL(mClsnOffset.y, mScale);
    mClsnOffset.z = FX_MUL(mClsnOffset.z, mScale);
}

// @symbol _ZN8daFPkn_c13CheckClsnHitsEv
void daFPkn_c::CheckClsnHits()
{
    dActor_c *hitter;
    int cmp;
    int hitBits;
    Vector3 pos1;
    Vector3 pos2;
    u32 otherId;

    /* The two gotos share the defeat path; a goto-free nesting of it DIFFs. */
    otherId = mdCcAc_c.otherOwner;
    if (otherId == 0)
        goto second;

    hitBits = mdCcAc_c.hitFlags & HIT_DAMAGE;
    if (hitBits != 0) {
        cmp = (int)(actorID == ACTOR_FIREPAKUN);
        if (cmp != 0)
            func_02012694(SND_HIT, &mCamSpacePos);
        else
            Sound::PlayBank0(SND_HIT_S, mCamSpacePos);
    activate_path:
        cmp = (int)(actorID == ACTOR_FIREPAKUN_S);
        if (cmp != 0) {
            mCoinType = 1; /* the coin kind SpawnCoin drops */
            SpawnCoin();
            KillAndTrackInDeathTable();
            Sound::PlayBank0(SND_HIT_S, mCamSpacePos);
        } else {
            mState = STATE_WAIT;
            mSpinCount = 0xa;
            mSpinSpeed = 0x1f40;
            mdCcAc_c.flags |= 1;
            mFlags &= ~0x10000000;
            mAnmModel.SetAnim((BCA_File *)data_ov084_02130e24.filePtr, 0x40000000, 0x1000, 0);
            mSuppressDeathReward = 0;
            mParticleHandle1 = 0;
        }
        if ((mdCcAc_c.hitFlags & HIT_MEGA_CHARACTER) == 0)
            goto second;
        Sound::PlayBank0(SND_HIT_S, mCamSpacePos);
        hitter = dActor_c::FindWithID(mdCcAc_c.otherOwner);
        if (hitter == 0)
            goto second;
        ((Player *)hitter)->IncMegaKillCount();
        func_02012694(SND_MEGA_KILL, &mCamSpacePos);
        goto second;
    }

    hitter = dActor_c::FindWithID(otherId);
    if (hitter == 0)
        goto second;
    cmp = (int)(hitter->actorID == ACTOR_PLAYER);
    if (cmp == 0)
        goto second;
    if (((Player *)hitter)->mIsMetal != 0) {
        Sound::PlayBank0(SND_HIT_S, mCamSpacePos);
        goto activate_path;
    }
    cmp = (int)(actorID == ACTOR_FIREPAKUN_S);
    if (cmp != 0) {
        if (JumpedOnByPlayer(mdCcAc_c, *(Player *)hitter) != 0) {
            Sound::PlayBank0(SND_JUMPED_ON, mCamSpacePos);
            ((Player *)hitter)->Bounce(0x28000);
            goto activate_path;
        }
    }
    if (((Player *)hitter)->mIsVanish != 0)
        goto second;
    cmp = (int)(actorID == ACTOR_FIREPAKUN);
    if (cmp != 0)
        goto second;
    pos1.x = mPosX;
    pos1.y = mPosY;
    pos1.z = mPosZ;
    ((Player *)hitter)->Hurt(pos1, 2, 0xc000, 1, 0, 1);

second:
    otherId = mdCcAcPos_c.otherOwner;
    if (otherId == 0)
        return;
    hitter = dActor_c::FindWithID(otherId);
    if (hitter == 0)
        return;
    cmp = (int)(hitter->actorID == ACTOR_PLAYER);
    if (cmp == 0)
        return;

    hitBits = mdCcAcPos_c.hitFlags & HIT_DAMAGE;
    if (hitBits != 0) {
        if ((hitBits & HIT_MEGA_CHARACTER) != 0) {
            ((Player *)hitter)->IncMegaKillCount();
            func_02012694(SND_MEGA_KILL, &mCamSpacePos);
        } else {
            cmp = (int)(actorID == ACTOR_FIREPAKUN);
            if (cmp != 0)
                func_02012694(SND_HIT, &mCamSpacePos);
            else
                Sound::PlayBank0(SND_HIT_S, mCamSpacePos);
        }
        mState = STATE_WAIT;
        mSpinCount = 0xa;
        mSpinSpeed = 0x1f40;
        mdCcAc_c.flags |= 1;
        mFlags &= ~0x10000000;
        mAnmModel.SetAnim((BCA_File *)data_ov084_02130e24.filePtr, 0x40000000, 0x1000, 0);
        mSuppressDeathReward = 0;
        mParticleHandle1 = 0;
        return;
    }

    if (((Player *)hitter)->mIsMetal != 0)
        return;
    if (((Player *)hitter)->mIsVanish != 0)
        return;
    pos2.x = mPosX;
    pos2.y = mPosY;
    pos2.z = mPosZ;
    ((Player *)hitter)->Hurt(pos2, 2, 0xc000, 1, 0, 1);
}

// @symbol _ZN8daFPkn_c9StateGrowEv
void daFPkn_c::StateGrow()
{
    Player *player;
    Vector3 playerPos;
    short targetAngle;
    dActor_c *spawned;
    int *src;

    if (ApproachLinear(mScale, mMaxScale, mScaleRate) == 0) {
        goto tail;  /* an early return here DIFFs */
    }

    if (mAnmModel.WillHitFrame(0x10) || mAnmModel.WillHitFrame(0x20) ||
        mAnmModel.WillHitFrame(0x34) || mAnmModel.WillHitFrame(0x4b)) {
        func_0201267c(SND_GROW_BITE, &mCamSpacePos);
    }

    targetAngle = mAngleY;
    player = ClosestPlayer();
    /* Read through src before the null test; direct member reads DIFF. */
    src = &player->mPosX;
    playerPos.x = src[0];
    playerPos.y = src[1];
    playerPos.z = src[2];
    if (player != 0) {
        targetAngle = Vec3_HorzAngle(&mPos, &playerPos);
    }
    ApproachLinear(mAngleY, targetAngle, 0x400);

    if (mSuppressDeathReward == 1) {
        spawned = dActor_c::Spawn(ACTOR_PAKUN, 0, mPos, &mAngle, mAreaId, -1);
        if (spawned == 0) return;

        mSuppressDeathReward = 2;
        ((daPkn_c *)spawned)->EnterLunge((short)((unsigned int)(mAnmModel.currFrame << 4) >> 16));
        mdCcAc_c.flags |= 1;
        mdCcAcPos_c.flags |= 1;
        return;
    }

    MarkForDestruction();
    return;

tail:
    mdCcAc_c.flags |= 1;
}

// @symbol _ZN8daFPkn_c9StateSpitEv
void daFPkn_c::StateSpit()
{
    Vector3 pos;
    s16 targetAngle;
    int cmp;
    Player *player;

    if (ApproachLinear(mScale, mMaxScale, mScaleRate) == 0)
        goto cold;  /* the ROM places this branch last */

    /* The int flag cmp keeps the ROM's compare; a direct test DIFFs. */
    if (mAnmModel.Finished() != 0) {
        cmp = (int)(actorID == ACTOR_FIREPAKUN_S);
        if (cmp != 0)
            func_0201267c(SND_SPIT_END_S, &mCamSpacePos);
        else
            func_0201267c(SND_SPIT_END, &mCamSpacePos);
        mState = STATE_WAIT;
        mAnmModel.SetAnim((BCA_File *)(((SharedFilePtr *)data_ov084_02130e1c)->filePtr), 0, 0x1000,
                          0);
    } else {
        if ((u16)mStateTimer < 0x3a) {
            targetAngle = mAngleY;
            player = ClosestPlayer();
            if (player != 0)
                targetAngle = Vec3_HorzAngle(&mPos, &player->mPos);
            ApproachLinear(mAngleY, targetAngle, 0x400);
        }
    }

    if (mAnmModel.WillHitFrame(0x3a) == 0)
        return;

    cmp = (int)(actorID == ACTOR_FIREPAKUN_S);
    if (cmp != 0)
        func_0201267c(SND_FIREBALL_S, &mCamSpacePos);
    else
        func_0201267c(SND_FIREBALL, &mCamSpacePos);

    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    pos.x += FX_MUL((mMaxScale * 0x3c), data_02082214[((u16)mAngleY >> 4) * 2]);
    pos.z += FX_MUL((mMaxScale * 0x3c), data_02082214[((u16)mAngleY >> 4) * 2 + 1]);
    pos.y += mMaxScale * 0x5a;
    mAngleX = 0x1000;

    SpawnFireball(pos, (const Vector3_16 *)(&mAngleX), 0x1e000, 0xa000, 3);
    return;

cold:
    if (mScale <= (mMaxScale >> 1))
        return;
    mdCcAc_c.flags &= ~1;
    mFlags |= 0x10000000;
}

// @symbol _ZN8daFPkn_c9StateWaitEv
void daFPkn_c::StateWait()
{
    Vector3 coinPos;
    Vector3 coinPos2;
    daFPkn_c *leader;
    Player *player;
    int dist;
    int cmp;

    if (mSpinCount != 0) {
        mAngleY += mSpinSpeed;
        ApproachLinear(mSpinSpeed, 0, 0xc8);
        SpawnDeathBurst();
        if (mAnmModel.Finished() == 0)
            return;
        mSpinCount--;
        if (mSpinCount != 0)
            return;
        func_02012694(SND_SPUN_DOWN, &mCamSpacePos);
        mParticleHandle2 = 0;
        mParticleHandle1 = mParticleHandle2;
        return;
    }

    SpawnDeathSmoke();
    if (ApproachLinear(mScale, 0, mScaleRate) == 0)
        return;
    mFlags &= ~0x10000000;
    mdCcAc_c.flags |= 1;

    /* The int flag cmp keeps the ROM's compares; direct tests DIFF. */
    if (mEmerged != 0) {
        mEmerged = 0;
        cmp = 0;
        if (actorID == ACTOR_FIREPAKUN)
            cmp = 1;
        if (cmp != false) {
            leader = (daFPkn_c *)dActor_c::FindWithID(mGroupLeaderID);
            if (leader == 0)
                return;
            leader->mGroupAliveCount--;
            if (mSuppressDeathReward != 0)
                return;
            leader->mGroupDefeatedCount++;
            if (mAlive != 0) {
                coinPos.x = mPosX;
                coinPos.y = mPosY;
                coinPos.z = mPosZ;
                SpawnCoins(coinPos, 2, 0xa000, 0);
            }
            if (leader->mGroupDefeatedCount == 5) {
                dActor_c::Spawn(ACTOR_STAR, mStarID | 0x40, mPos, 0, mAreaId, -1);
                leader->KillAndTrackInDeathTable();
                KillAndTrackInDeathTable();
                return;
            }
            OnGroupMemberDefeated();
            if (mRespawnMode != 1) {
                KillAndTrackInDeathTable();
                return;
            }
            TrackInDeathTable();
            mState = STATE_DEFEATED;
            return;
        }
        cmp = actorID == ACTOR_PAKUN2;
        if (cmp == false)
            return;
        if (mSuppressDeathReward != 0)
            return;
        coinPos2.x = mPosX;
        coinPos2.y = mPosY;
        coinPos2.z = mPosZ;
        SpawnCoins(coinPos2, 1, 0xa000, 0);
        KillAndTrackInDeathTable();
        return;
    }

    dist = DistToCPlayer();
    cmp = actorID == ACTOR_FIREPAKUN;
    if (cmp != false) {
        leader = (daFPkn_c *)dActor_c::FindWithID(mGroupLeaderID);
        if (leader == 0)
            return;
    }
    if ((u16)mStateTimer <= 0x64)
        return;
    if (dist <= 0x64000)
        return;
    if (dist >= 0x320000)
        return;
    cmp = actorID == ACTOR_FIREPAKUN;
    if (cmp != false) {
        if (leader->mGroupAliveCount >= 2)
            return;
    }
    cmp = actorID == ACTOR_FIREPAKUN_S;
    if (cmp != false)
        func_0201267c(SND_EMERGE_S, &mCamSpacePos);
    else
        func_0201267c(SND_EMERGE, &mCamSpacePos);
    cmp = 1;
    mEmerged = 1;
    if (actorID != ACTOR_FIREPAKUN)
        cmp = 0;
    if (cmp != false)
        leader->mGroupAliveCount++;
    cmp = actorID == ACTOR_PAKUN2;
    if (cmp != false) {
        mState = STATE_GROW;
        mAnmModel.SetAnim((BCA_File *)data_ov084_02130e14.filePtr, 0x40000000, 0x1000, 0);
    } else {
        mState = STATE_SPIT;
        mAnmModel.SetAnim((BCA_File *)data_ov084_02130e04.filePtr, 0x40000000, 0x1000, 0);
        mAnmModel.currFrame = 0;
    }
    cmp = actorID == ACTOR_FIREPAKUN;
    if (cmp != false) {
        if (leader->mStarMarkerIdx >= 0) {
            int marker;
            if (IsStarCollectedInCurLevel(mStarID) != 0)
                marker = 3;
            else
                marker = 2;
            SetStarMarker(leader->mStarMarkerIdx, this, marker);
            leader->mMarkedMemberID = uniqueID;
        }
    }
    player = ClosestPlayer();
    if (player == 0)
        return;
    mAngleY = Vec3_HorzAngle(&mPos, &player->mPos);
}

// @symbol _ZN8daFPkn_c9StateInitEv
int daFPkn_c::StateInit()
{
    daFPkn_c *member;
    if (mRespawnMode == 0) {
        mStarMarkerIdx = -1;
        mGroupLeaderID = uniqueID;
        mMarkedMemberID = uniqueID;
        mRespawnMode = 1;
        member = 0;
        for (;;) {  /* a while loop over the assignment DIFFs */
            member = (daFPkn_c *)FindWithActorID(ACTOR_FIREPAKUN, member);
            if (member == 0) break;
            if (member != this) {
                member->mRespawnMode = 2;
                member->mGroupLeaderID = uniqueID;
            }
        }
    }
    mState = STATE_WAIT;
    return 1;
}

// @symbol _ZN8daFPkn_c16CleanupResourcesEv
int daFPkn_c::CleanupResources()
{
    data_ov084_02130dfc.Release();
    for (int i = 0; i < 6; i++)
        data_ov084_021302f4[i]->Release();
    data_ov002_0210da38.Release();
    UnloadBlueCoinModel(this);
    return 1;
}

// @symbol _ZN8daFPkn_c6RenderEv
/* The int inMouth keeps the ROM's flag test; a plain `||` DIFFs. */
int daFPkn_c::Render()
{
    int scale = mScale;
    int inMouth;
    if (scale == 0 || (inMouth = (mFlags & MFLAG_YOSHI_MOUTH_B) != 0, inMouth != 0)) {
        return 1;
    }
    Vector3 size;
    size.x = scale;
    size.y = scale;
    size.z = scale;
    mAnmModel.Render(&size);
    return 1;
}

// @symbol _ZN8daFPkn_c8BehaviorEv
int daFPkn_c::Behavior()
{
    MakeVanishLuigiWork(mdCcAc_c);
    /* The int flags inYoshiMouth and isFirepakunS keep the ROM's tests; direct tests DIFF. */
    int inYoshiMouth = (mFlags & (MFLAG_YOSHI_MOUTH_A | MFLAG_YOSHI_MOUTH_B)) != 0;
    if (inYoshiMouth != 0) {
        UpdateClsnOffset();
        return 1;
    }
    mAnmModel.Advance();
    int prevState = mState;
    switch (prevState) {
    case STATE_INIT:
        StateInit();
        break;
    case STATE_WAIT:
        StateWait();
        break;
    case STATE_SPIT:
        StateSpit();
        break;
    case STATE_GROW:
        StateGrow();
        break;
    case STATE_DEFEATED:
        break;
    }
    /* mStateTimer is s16; the ROM counts it unsigned, and `(u16)mStateTimer + 1` DIFFs. */
    {
        unsigned short *timer = (unsigned short *)&mStateTimer;
        *timer = *timer + 1;
        if (prevState != mState)
            *timer = 0;
    }
    CheckClsnHits();
    UpdateClsnOffset();
    mdCcAc_c.Clear();
    mdCcAc_c.radius = mScale * mClsnRadiusFactor;
    mdCcAc_c.height = mScale * mClsnHeightFactor;
    mdCcAc_c.Update();
    mdCcAcPos_c.Clear();
    int isFirepakunS = actorID == ACTOR_FIREPAKUN_S;
    if (isFirepakunS == 0
        && (unsigned int)(mState - STATE_SPIT) <= 1
        && mScale == mMaxScale) {
        mdCcAcPos_c.SetPosRelativeToActor(mClsnOffset);
        mdCcAcPos_c.Update();
    }
    return 1;
}

// @symbol _ZN8daFPkn_c13InitResourcesEv
int daFPkn_c::InitResources()
{
    int i;
    Vector3 clsnPos;
    int type;
    int cmp;

    mAnmModel.SetFile((BMD_File *)dExtSimpleModel_c::LoadFile(data_ov084_02130dfc), 1, -1);

    for (i = 0; i < 6; i++)
        dExtFrameCtrl_c::LoadFile(*data_ov084_021302f4[i]);

    dExtSimpleModel_c::LoadFile(data_ov002_0210da38);
    LoadBlueCoinModel(this);

    /* A real SetAnim call with a Fix12<int> speed DIFFs; see the file header. */
    mAnmModel.SetAnim((BCA_File *)data_ov084_02130df4.filePtr, 0x40000000, 0x1000, 0);

    mdCcAc_c.Init(this, 0, 0, 0x200001, HIT_DAMAGE & ~HIT_MEGA_CHARACTER);

    clsnPos.x = 0;
    clsnPos.y = 0;
    clsnPos.z = 0;
    mdCcAcPos_c.Init(this, clsnPos, 0x4b000, 0x64000, 0x200002, HIT_DAMAGE & ~HIT_MEGA_CHARACTER);

    mScale = 0;
    mRespawnMode = 0;
    mState = STATE_INIT;
    mGroupLeaderID = 0;
    mMarkedMemberID = 0;
    mGroupAliveCount = 0;
    mGroupDefeatedCount = 0;
    mEmerged = 0;
    mSpinCount = 0;
    mSuppressDeathReward = 1;
    mParticleHandle2 = 0;
    mParticleHandle1 = mParticleHandle2;

    type = actorID;
    /* type and cmp keep the ROM's compares; direct tests DIFF. */
    cmp = (type == ACTOR_FIREPAKUN_S);
    if (cmp != 0) {
        mClsnRadiusFactor = 0x3c;
        mClsnHeightFactor = 0xaa;
        mMaxScale = 0x800;
        mScaleRate = 0x52;
        mState = STATE_WAIT;
        mdCcAc_c.vulnFlags |= 0x8000;
    } else {
        cmp = (type == ACTOR_PAKUN2);
        if (cmp != 0) {
            mClsnRadiusFactor = 0x28;
            mClsnHeightFactor = 0xaa;
            mMaxScale = 0x1000;
            mScaleRate = 0xa4;
            mState = STATE_WAIT;
        } else {
            mClsnRadiusFactor = 0x28;
            mClsnHeightFactor = 0x96;
            mMaxScale = 0x2000;
            mScaleRate = 0x147;
            mdCcAcPos_c.radius = 0x64000;
            mdCcAcPos_c.height = 0x64000;
            if (GetBitInDeathTable() != 0)
                mAlive = 0;
            else
                mAlive = 1;
        }
    }

    mStarID = (unsigned char)(param1 & 0xf);
    return 1;
}

// @symbol _ZN8daFPkn_c16OnAimedAtWithEggEv
s32 daFPkn_c::OnAimedAtWithEgg() {
    if (mdCcAc_c.flags & 1)
        return mScale * 100;
    int reach = mScale * mClsnHeightFactor >> 1;
    int floor = mScale * 100;
    if (reach <= floor)
        reach = floor;
    return reach;
}

// @symbol _ZN8daFPkn_c13OnTurnIntoEggER6Player
void daFPkn_c::OnTurnIntoEgg(Player &player)
{
    GivePlayerCoins(player, 1, 0);
    KillAndTrackInDeathTable();
}

// @symbol _ZN8daFPkn_c13OnYoshiTryEatEv
/* Two steps; `return actorID == ACTOR_FIREPAKUN_S ? 4 : 0` DIFFs. */
s32 daFPkn_c::OnYoshiTryEat() {
    int r;
    if (actorID == ACTOR_FIREPAKUN_S)
        r = 1;
    else
        r = 0;
    if (r != 0)
        r = 4;
    else
        r = 0;
    return r;
}

// @symbol daFPkn_c_classInit_PAKUN2
extern "C" daFPkn_c *daFPkn_c_classInit_PAKUN2()
{
    return new daFPkn_c();
}

// @symbol daFPkn_c_classInit_FIREPAKUN_S
extern "C" daFPkn_c *daFPkn_c_classInit_FIREPAKUN_S()
{
    return new daFPkn_c();
}

// @symbol daFPkn_c_classInit_FIREPAKUN
extern "C" daFPkn_c *daFPkn_c_classInit_FIREPAKUN()
{
    return new daFPkn_c();
}
