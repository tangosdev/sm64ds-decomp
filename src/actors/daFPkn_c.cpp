//cpp
/* daFPkn_c -- the fire piranha plant (PAKUN2 / FIREPAKUN / FIREPAKUN_S).
 * ov084 .text 0x0212d248..0x0212ea18, eighteen functions: D1, D0, nine
 * helpers and states, then the seven vtable methods.
 *
 * NAME: daFPkn_c is the cartridge's RTTI spelling. The word before the
 * vtable address point 0x02130b28 (0x02130b24) relocates to _ZTI8daFPkn_c
 * at 0x02130ac0, which reads [__si_class_type_info, _ZTS8daFPkn_c
 * (0x02130ab4, "8daFPkn_c"), _ZTI12dEnemyBase_c (ov002 0x021081c0)].
 *
 * THE DESTRUCTOR IS THE KEY FUNCTION. It is defined first under
 * `#pragma defer_codegen off`, so mwccarm emits each function as it is
 * parsed and the file is written in ROM order: D1 (0x0212d248) and D0
 * (0x0212d288) first, then a D2 the cartridge has no home for (licensed
 * as deadstrip), and this object carries _ZTV8daFPkn_c and the RTTI chain
 * as vague linkage. The ROM keeps the table; the promotion is text-only.
 *
 * The run's left neighbour is daRedBombhei_c_classInit (0x0212d200), another
 * class's factory. The three daFPkn_c classInit factories start at
 * 0x0212ea18 and stay one-function C sources.
 *
 * Known limits:
 * - ModelAnim::SetAnim, dCcAc_c::Init, dCcAcPos_c::Init,
 *   Particle::System::New, dActor_c::SpawnFireball and SpawnCoins,
 *   Player::Hurt and Player::Bounce are called by their mangled names. Each
 *   symbol carries a Fix12<int> by value; a Fix12<int> local for SetAnim's
 *   speed changes InitResources.
 * - The shared files and tables keep their address names; the static
 *   initializer and the classInit sources name them too.
 */

#include "decl_common.h"
#include "daFPkn_c.h"
#include "SharedFilePtr.h"
#include "Player.h"
#include "Sound.h"

#pragma defer_codegen off

/* Second word is the loaded BCA. Same two words as daPkn_c's PknSharedFile. */
struct PknSharedFile {
    int id;
    void *file;
};
typedef char PknSharedFile_size_must_be_0x8[sizeof(PknSharedFile) == 0x8 ? 1 : -1];

/* UpdateClsnOffset's stack locals: three bone-angle sums, then the
   position shifted down by 3. */
struct Locals {
    s16 acc[3];
    int tmp[3];
};

int ApproachLinear(int &value, int target, int step);
int ApproachLinear(short &value, short target, short step);

extern "C" {
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *self, void *f, int a, int b, unsigned short cc);
void _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(void *self, void *a, int r, int h, unsigned int e, unsigned int g);
void _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(void *self, void *a, const Vector3 *v, int r, int h, unsigned int e, unsigned int g);
u32 _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
    u32 a, u32 b, Fix12i c, Fix12i d, Fix12i e, const void *f, void *g);
void *_ZN8Particle6System12FromUniqueIDEj(u32 id);
void _ZN6Player6BounceE5Fix12IiE(void *p, int fix);
int _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(void *self, const void *pos, u32 a, int fix, u32 b, u32 c, u32 d);
void _ZN8dActor_c13SpawnFireballERK7Vector3PK10Vector3_165Fix12IiES7_j(
    void *self, const void *pos, const void *v16, int a, int b, u32 g);
void _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(void *self, const void *pos, unsigned int a, int b, short c);

void LoadBlueCoinModel(void *c);
void UnloadBlueCoinModel(void *);
void func_02012694(u32 id, void *pos);
void func_0201267c(unsigned int id, const void *pos);
short Vec3_HorzAngle(const void *a, const void *b);
int IsStarCollectedInCurLevel(unsigned int flag);
void SetStarMarker(int i, void *self, int v);
void Matrix4x3_FromRotationY(void *m, int angle);
void Vec3_Asr(void *d, void *s, int sh);
void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
void MulMat4x3Mat4x3(void *d, void *a, void *b);
void Vec3_LslInPlace(void *v, int sh);
void SubVec3(void *a, void *b, void *c);

extern SharedFilePtr data_ov084_02130dfc;   /* the plant's model */
extern SharedFilePtr *data_ov084_021302f4[];
extern SharedFilePtr data_ov002_0210da38;
extern PknSharedFile data_ov084_02130df4;   /* idle animation */
extern PknSharedFile data_ov084_02130e24;   /* death animation */
extern PknSharedFile data_ov084_02130e14;   /* lunge animation */
extern PknSharedFile data_ov084_02130e04;   /* spit animation */
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
    void* o;
    if (mModelAnim.file != data_ov084_02130e24.file)
        return;

    mParticleHandle1 = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        mParticleHandle1, 0xfc, mPosX, mPosY + 0x1e000, mPosZ, 0, 0);
    if (mParticleHandle1 != 0) {
        o = _ZN8Particle6System12FromUniqueIDEj(mParticleHandle1);
        if (o != 0) {
            *(int*)((char*)o + 0x50) = (short)(Fix12i)(((long long)mMaxScale * 0x2800 + 0x800) >> 12);
        }
    }

    mParticleHandle2 = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        mParticleHandle2, 0xfd, mPosX, mPosY + 0x1e000, mPosZ, 0, 0);
    if (mParticleHandle2 == 0)
        return;
    o = _ZN8Particle6System12FromUniqueIDEj(mParticleHandle2);
    if (o == 0)
        return;
    *(int*)((char*)o + 0x50) = (short)(Fix12i)(((long long)mMaxScale * 0x2800 + 0x800) >> 12);
}

// @symbol _ZN8daFPkn_c15SpawnDeathBurstEv
void daFPkn_c::SpawnDeathBurst()
{
    Vector3 pos;
    int idx, fac, m;

    if (mModelAnim.file != data_ov084_02130e24.file)
        return;
    idx = (int)((unsigned)(mModelAnim.currFrame << 4) >> 16);
    if (idx >= 0xa)
        return;

    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    fac = data_ov084_0213029c[idx];
    m = fac * data_02082214[((u16)mAngleY >> 4) * 2];
    pos.x = pos.x + (int)(((long long)m * mScale + 0x800) >> 12);
    m = fac * data_02082214[((u16)mAngleY >> 4) * 2 + 1];
    pos.z = pos.z + (int)(((long long)m * mScale + 0x800) >> 12);
    pos.y = pos.y + mScale * data_ov084_021302c4[idx];
    mParticleHandle1 = _ZN8Particle6System3NewEjj5Fix12IiES2_S2_PK11Vector3_16fPNS_8CallbackE(
        mParticleHandle1, 0xfb, pos.x, pos.y, pos.z, 0, 0);
}

/* Empty. StateWait passes this in r0 and nothing reads it. */
// @symbol _ZN8daFPkn_c21OnGroupMemberDefeatedEv
void daFPkn_c::OnGroupMemberDefeated()
{
}

// @symbol _ZN8daFPkn_c16UpdateClsnOffsetEv
void daFPkn_c::UpdateClsnOffset()
{
    struct Locals locals;
    int sx, sy, sz, v, flag;

    Matrix4x3_FromRotationY(&mModelAnim.mat4x3, mAngleY);
    mModelAnim.mat4x3.m[9] = mPosX >> 3;
    mModelAnim.mat4x3.m[10] = mPosY >> 3;
    mModelAnim.mat4x3.m[11] = mPosZ >> 3;

    if ((u32)(mState - 2) > 1) return;
    if (mScale != mMaxScale) return;

    locals.acc[0] = 0;
    locals.acc[1] = 0;
    locals.acc[2] = 0;
    {
        char* base = (char *)mModelAnim.data.bones;
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

    Vec3_Asr(locals.tmp, &mPosX, 3);
    Matrix4x3_FromTranslation(
        data_020a0e68,
        locals.tmp[0],
        locals.tmp[1],
        locals.tmp[2]
    );
    MulMat4x3Mat4x3(&mModelAnim.data.transforms[6], data_020a0e68, data_020a0e68);
    mClsnOffset.x = data_020a0e68[0x24 / 4];
    mClsnOffset.y = data_020a0e68[0x28 / 4];
    mClsnOffset.z = data_020a0e68[0x2c / 4];
    Vec3_LslInPlace(&mClsnOffset, 3);
    SubVec3(&mClsnOffset, &mPosX, &mClsnOffset);

    sx = data_02082214[((u16)locals.acc[0] >> 4) * 2] * 0x32;
    sy = data_02082214[((u16)locals.acc[1] >> 4) * 2];

    mClsnOffset.x = mClsnOffset.x + (int)(((s64)sx * sy + 0x800) >> 12);

    /* The int flag keeps the ROM's compare; a direct test DIFFs. */
    flag = (actorID == 0xfb);
    if (flag != false) {
        v = data_02082214[((u16)locals.acc[0] >> 4) * 2 + 1] * 0x32;
        mClsnOffset.y = mClsnOffset.y - (0x19000 - v);
    } else {
        v = data_02082214[((u16)locals.acc[0] >> 4) * 2 + 1] * 0x32;
        mClsnOffset.y = mClsnOffset.y - (0x32000 - v);
    }

    sz = data_02082214[((u16)locals.acc[1] >> 4) * 2 + 1];
    mClsnOffset.z = mClsnOffset.z + (int)(((s64)sx * sz + 0x800) >> 12);

    mClsnOffset.x = (int)(((s64)mClsnOffset.x * mScale + 0x800) >> 12);
    mClsnOffset.y = (int)(((s64)mClsnOffset.y * mScale + 0x800) >> 12);
    mClsnOffset.z = (int)(((s64)mClsnOffset.z * mScale + 0x800) >> 12);
}

// @symbol _ZN8daFPkn_c13CheckClsnHitsEv
void daFPkn_c::CheckClsnHits()
{
    dActor_c *actor;
    int t;
    int flags;
    Vector3 pos1;
    Vector3 pos2;
    u32 id;

    /* The two gotos share the defeat path; a goto-free nesting of it DIFFs. */
    id = mdCcAc_c.otherOwner;
    if (id == 0)
        goto second;

    flags = mdCcAc_c.hitFlags & 0x66ff0;
    if (flags != 0) {
        t = (int)(actorID == 0xfb);
        if (t != 0)
            func_02012694(0x1e, &mCamSpacePosX);
        else
            Sound::PlayBank0(0xa, (Vector3 &)mCamSpacePosX);
    activate_path:
        t = (int)(actorID == 0xfc);
        if (t != 0) {
            unk_108 = 1;
            SpawnCoin();
            KillAndTrackInDeathTable();
            Sound::PlayBank0(0xa, (Vector3 &)mCamSpacePosX);
        } else {
            mState = 1;
            mSpinCount = 0xa;
            mSpinSpeed = 0x1f40;
            mdCcAc_c.flags |= 1;
            mFlags &= ~0x10000000;
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, data_ov084_02130e24.file, 0x40000000, 0x1000, 0);
            mSuppressDeathReward = 0;
            mParticleHandle1 = 0;
        }
        if ((mdCcAc_c.hitFlags & 0x10) == 0)
            goto second;
        Sound::PlayBank0(0xa, (Vector3 &)mCamSpacePosX);
        actor = dActor_c::FindWithID(mdCcAc_c.otherOwner);
        if (actor == 0)
            goto second;
        ((Player *)actor)->IncMegaKillCount();
        func_02012694(0x1d, &mCamSpacePosX);
        goto second;
    }

    actor = dActor_c::FindWithID(id);
    if (actor == 0)
        goto second;
    t = (int)(actor->actorID == 0xbf);
    if (t == 0)
        goto second;
    if (((Player *)actor)->mIsMetal != 0) {
        Sound::PlayBank0(0xa, (Vector3 &)mCamSpacePosX);
        goto activate_path;
    }
    t = (int)(actorID == 0xfc);
    if (t != 0) {
        if (JumpedOnByPlayer(mdCcAc_c, *(Player *)actor) != 0) {
            Sound::PlayBank0(0xb6, (Vector3 &)mCamSpacePosX);
            _ZN6Player6BounceE5Fix12IiE(actor, 0x28000);
            goto activate_path;
        }
    }
    if (((Player *)actor)->mIsVanish != 0)
        goto second;
    t = (int)(actorID == 0xfb);
    if (t != 0)
        goto second;
    pos1.x = mPosX;
    pos1.y = mPosY;
    pos1.z = mPosZ;
    _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(actor, &pos1, 2, 0xc000, 1, 0, 1);

second:
    id = mdCcAcPos_c.otherOwner;
    if (id == 0)
        return;
    actor = dActor_c::FindWithID(id);
    if (actor == 0)
        return;
    t = (int)(actor->actorID == 0xbf);
    if (t == 0)
        return;

    flags = mdCcAcPos_c.hitFlags & 0x66ff0;
    if (flags != 0) {
        if ((flags & 0x10) != 0) {
            ((Player *)actor)->IncMegaKillCount();
            func_02012694(0x1d, &mCamSpacePosX);
        } else {
            t = (int)(actorID == 0xfb);
            if (t != 0)
                func_02012694(0x1e, &mCamSpacePosX);
            else
                Sound::PlayBank0(0xa, (Vector3 &)mCamSpacePosX);
        }
        mState = 1;
        mSpinCount = 0xa;
        mSpinSpeed = 0x1f40;
        mdCcAc_c.flags |= 1;
        mFlags &= ~0x10000000;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, data_ov084_02130e24.file, 0x40000000, 0x1000, 0);
        mSuppressDeathReward = 0;
        mParticleHandle1 = 0;
        return;
    }

    if (((Player *)actor)->mIsMetal != 0)
        return;
    if (((Player *)actor)->mIsVanish != 0)
        return;
    pos2.x = mPosX;
    pos2.y = mPosY;
    pos2.z = mPosZ;
    _ZN6Player4HurtERK7Vector3j5Fix12IiEjjj(actor, &pos2, 2, 0xc000, 1, 0, 1);
}

// @symbol _ZN8daFPkn_c9StateGrowEv
void daFPkn_c::StateGrow()
{
    Player *player;
    Vector3 v;
    short angle;
    dActor_c *spawned;
    int *p;

    if (ApproachLinear(mScale, mMaxScale, mScaleRate) == 0) {
        goto tail;  /* an early return here DIFFs */
    }

    if (mModelAnim.WillHitFrame(0x10) ||
        mModelAnim.WillHitFrame(0x20) ||
        mModelAnim.WillHitFrame(0x34) ||
        mModelAnim.WillHitFrame(0x4b)) {
        func_0201267c(0xc0, &mCamSpacePosX);
    }

    angle = mAngleY;
    player = ClosestPlayer();
    /* Read through p before the null test; direct member reads DIFF. */
    p = &player->mPosX;
    v.x = p[0];
    v.y = p[1];
    v.z = p[2];
    if (player != 0) {
        angle = Vec3_HorzAngle(&mPosX, &v);
    }
    ApproachLinear(mAngleY, angle, 0x400);

    if (mSuppressDeathReward == 1) {
        spawned = dActor_c::Spawn(0xfa, 0, (Vector3 &)mPosX, (Vector3_16 *)&mAngleX, mAreaId, -1);
        if (spawned == 0) return;

        mSuppressDeathReward = 2;
        func_ov084_0212ec04((char*)spawned, (short)((unsigned int)(mModelAnim.currFrame << 4) >> 16));
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
    s16 ang;
    int b;
    Player *player;

    if (ApproachLinear(mScale, mMaxScale, mScaleRate) == 0)
        goto cold;  /* the ROM places this branch last */

    /* The int flag b keeps the ROM's compare; a direct test DIFFs. */
    if (mModelAnim.Finished() != 0) {
        b = (int)(actorID == 0xfc);
        if (b != 0)
            func_0201267c(0xe3, &mCamSpacePosX);
        else
            func_0201267c(0x120, &mCamSpacePosX);
        mState = 1;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, data_ov084_02130e1c[1], 0, 0x1000, 0);
    } else {
        if ((u16)mStateTimer < 0x3a) {
            ang = mAngleY;
            player = ClosestPlayer();
            if (player != 0)
                ang = Vec3_HorzAngle(&mPosX, &player->mPosX);
            ApproachLinear(mAngleY, ang, 0x400);
        }
    }

    if (mModelAnim.WillHitFrame(0x3a) == 0)
        return;

    b = (int)(actorID == 0xfc);
    if (b != 0)
        func_0201267c(0x105, &mCamSpacePosX);
    else
        func_0201267c(0x122, &mCamSpacePosX);

    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    pos.x += (s32)(((s64)(mMaxScale * 0x3c) * data_02082214[((u16)mAngleY >> 4) * 2] + 0x800) >> 12);
    pos.z += (s32)(((s64)(mMaxScale * 0x3c) * data_02082214[((u16)mAngleY >> 4) * 2 + 1] + 0x800) >> 12);
    pos.y += mMaxScale * 0x5a;
    mAngleX = 0x1000;

    _ZN8dActor_c13SpawnFireballERK7Vector3PK10Vector3_165Fix12IiES7_j(
        this, &pos, &mAngleX, 0x1e000, 0xa000, 3);
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
    Vector3 buf1;
    Vector3 buf2;
    daFPkn_c *other;
    Player *player;
    int dist;
    int b;

    if (mSpinCount != 0) {
        mAngleY += mSpinSpeed;
        ApproachLinear(mSpinSpeed, 0, 0xc8);
        SpawnDeathBurst();
        if (mModelAnim.Finished() == 0)
            return;
        mSpinCount--;
        if (mSpinCount != 0)
            return;
        func_02012694(0x11f, &mCamSpacePosX);
        mParticleHandle2 = 0;
        mParticleHandle1 = mParticleHandle2;
        return;
    }

    SpawnDeathSmoke();
    if (ApproachLinear(mScale, 0, mScaleRate) == 0)
        return;
    mFlags &= ~0x10000000;
    mdCcAc_c.flags |= 1;

    /* The int flag b keeps the ROM's compares; direct tests DIFF. */
    if (mEmerged != 0) {
        mEmerged = 0;
        b = 0;
        if (actorID == 0xfb)
            b = 1;
        if (b != false) {
            other = (daFPkn_c *)dActor_c::FindWithID(mGroupLeaderID);
            if (other == 0)
                return;
            other->mGroupAliveCount--;
            if (mSuppressDeathReward != 0)
                return;
            other->mGroupDefeatedCount++;
            if (mAlive != 0) {
                buf1.x = mPosX;
                buf1.y = mPosY;
                buf1.z = mPosZ;
                _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(this, &buf1, 2, 0xa000, 0);
            }
            if (other->mGroupDefeatedCount == 5) {
                dActor_c::Spawn(0xb2, mStarID | 0x40, (Vector3 &)mPosX, 0, mAreaId, -1);
                other->KillAndTrackInDeathTable();
                KillAndTrackInDeathTable();
                return;
            }
            OnGroupMemberDefeated();
            if (mRespawnMode != 1) {
                KillAndTrackInDeathTable();
                return;
            }
            TrackInDeathTable();
            mState = 4;
            return;
        }
        b = actorID == 0xfd;
        if (b == false)
            return;
        if (mSuppressDeathReward != 0)
            return;
        buf2.x = mPosX;
        buf2.y = mPosY;
        buf2.z = mPosZ;
        _ZN8dActor_c10SpawnCoinsERK7Vector3j5Fix12IiEs(this, &buf2, 1, 0xa000, 0);
        KillAndTrackInDeathTable();
        return;
    }

    dist = DistToCPlayer();
    b = actorID == 0xfb;
    if (b != false) {
        other = (daFPkn_c *)dActor_c::FindWithID(mGroupLeaderID);
        if (other == 0)
            return;
    }
    if ((u16)mStateTimer <= 0x64)
        return;
    if (dist <= 0x64000)
        return;
    if (dist >= 0x320000)
        return;
    b = actorID == 0xfb;
    if (b != false) {
        if (other->mGroupAliveCount >= 2)
            return;
    }
    b = actorID == 0xfc;
    if (b != false)
        func_0201267c(0x104, &mCamSpacePosX);
    else
        func_0201267c(0x121, &mCamSpacePosX);
    b = 1;
    mEmerged = 1;
    if (actorID != 0xfb)
        b = 0;
    if (b != false)
        other->mGroupAliveCount++;
    b = actorID == 0xfd;
    if (b != false) {
        mState = 3;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, data_ov084_02130e14.file, 0x40000000, 0x1000, 0);
    } else {
        mState = 2;
        _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&mModelAnim, data_ov084_02130e04.file, 0x40000000, 0x1000, 0);
        mModelAnim.currFrame = 0;
    }
    b = actorID == 0xfb;
    if (b != false) {
        if (other->mStarMarkerIdx >= 0) {
            int v;
            if (IsStarCollectedInCurLevel(mStarID) != 0)
                v = 3;
            else
                v = 2;
            SetStarMarker(other->mStarMarkerIdx, this, v);
            other->mMarkedMemberID = uniqueID;
        }
    }
    player = ClosestPlayer();
    if (player == 0)
        return;
    mAngleY = Vec3_HorzAngle(&mPosX, &player->mPosX);
}

// @symbol _ZN8daFPkn_c9StateInitEv
int daFPkn_c::StateInit()
{
    daFPkn_c *p;
    if (mRespawnMode == 0) {
        mStarMarkerIdx = -1;
        mGroupLeaderID = uniqueID;
        mMarkedMemberID = uniqueID;
        mRespawnMode = 1;
        p = 0;
        for (;;) {  /* a while loop over the assignment DIFFs */
            p = (daFPkn_c *)FindWithActorID(0xfb, p);
            if (p == 0) break;
            if (p != this) {
                p->mRespawnMode = 2;
                p->mGroupLeaderID = uniqueID;
            }
        }
    }
    mState = 1;
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
/* The int b keeps the ROM's flag test; a plain `||` DIFFs. */
int daFPkn_c::Render()
{
    int v = mScale;
    int b;
    if (v == 0 || (b = (mFlags & 0x40000) != 0, b != 0)) {
        return 1;
    }
    Vector3 s;
    s.x = v;
    s.y = v;
    s.z = v;
    mModelAnim.Render(&s);
    return 1;
}

// @symbol _ZN8daFPkn_c8BehaviorEv
int daFPkn_c::Behavior()
{
    MakeVanishLuigiWork(mdCcAc_c);
    /* The int flags b and b2 keep the ROM's tests; direct tests DIFF. */
    int b = (mFlags & 0x60000) != 0;
    if (b != 0) {
        UpdateClsnOffset();
        return 1;
    }
    mModelAnim.Advance();
    int s = mState;
    switch (s) {
    case 0:
        StateInit();
        break;
    case 1:
        StateWait();
        break;
    case 2:
        StateSpit();
        break;
    case 3:
        StateGrow();
        break;
    case 4:
        break;
    }
    /* mStateTimer is s16; the ROM counts it unsigned, and `(u16)mStateTimer + 1` DIFFs. */
    {
        unsigned short *p = (unsigned short *)&mStateTimer;
        *p = *p + 1;
        if (s != mState)
            *p = 0;
    }
    CheckClsnHits();
    UpdateClsnOffset();
    mdCcAc_c.Clear();
    mdCcAc_c.radius = mScale * mClsnRadiusFactor;
    mdCcAc_c.height = mScale * mClsnHeightFactor;
    mdCcAc_c.Update();
    mdCcAcPos_c.Clear();
    int b2 = actorID == 0xfc;
    if (b2 == 0
        && (unsigned int)(mState - 2) <= 1
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
    Vector3 v;
    int id;
    int cond;

    mModelAnim.SetFile((BMD_File *)Model::LoadFile(data_ov084_02130dfc), 1, -1);

    for (i = 0; i < 6; i++)
        Animation::LoadFile(*data_ov084_021302f4[i]);

    Model::LoadFile(data_ov002_0210da38);
    LoadBlueCoinModel(this);

    /* A real SetAnim call with a Fix12<int> speed DIFFs; see the file header. */
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        &mModelAnim, data_ov084_02130df4.file, 0x40000000, 0x1000, 0);

    _ZN7dCcAc_c4InitEP8dActor_c5Fix12IiES3_jj(
        &mdCcAc_c, this, 0, 0, 0x200001, 0x66fe0);

    v.x = 0;
    v.y = 0;
    v.z = 0;
    _ZN10dCcAcPos_c4InitEP8dActor_cRK7Vector35Fix12IiES6_jj(
        &mdCcAcPos_c, this, &v, 0x4b000, 0x64000, 0x200002, 0x66fe0);

    mScale = 0;
    mRespawnMode = 0;
    mState = 0;
    mGroupLeaderID = 0;
    mMarkedMemberID = 0;
    mGroupAliveCount = 0;
    mGroupDefeatedCount = 0;
    mEmerged = 0;
    mSpinCount = 0;
    mSuppressDeathReward = 1;
    mParticleHandle2 = 0;
    mParticleHandle1 = mParticleHandle2;

    id = actorID;
    /* id and cond keep the ROM's compares; direct tests DIFF. */
    cond = (id == 0xfc);
    if (cond != 0) {
        mClsnRadiusFactor = 0x3c;
        mClsnHeightFactor = 0xaa;
        mMaxScale = 0x800;
        mScaleRate = 0x52;
        mState = 1;
        mdCcAc_c.vulnFlags |= 0x8000;
    } else {
        cond = (id == 0xfd);
        if (cond != 0) {
            mClsnRadiusFactor = 0x28;
            mClsnHeightFactor = 0xaa;
            mMaxScale = 0x1000;
            mScaleRate = 0xa4;
            mState = 1;
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
/* Two steps; `return actorID == 0xfc ? 4 : 0` DIFFs. */
s32 daFPkn_c::OnYoshiTryEat() {
    int r;
    if (actorID == 0xfc)
        r = 1;
    else
        r = 0;
    if (r != 0)
        r = 4;
    else
        r = 0;
    return r;
}
