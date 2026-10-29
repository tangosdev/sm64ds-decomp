//cpp
struct Vector3_16f;
namespace Particle {
struct Callback;
struct System {
    static int New(unsigned, unsigned, int, int, int, const ::Vector3_16f *, Callback *);
};
}
/* daSnowman_c — Mr. Blizzard (SNOWMAN, registry id 0xdf).
 *
 * param1's low byte is the path, the next byte is the kind. Kind 0 walks
 * that path and throws a snowball (SNOWBALL, 0xe0). Any other kind starts
 * buried and rises when a player comes near. Kind 2 wears Mario's cap
 * (OBJ_MARIO_CAP, 0x10d). When that one is gone it plants a kind-3 watcher
 * here (param 0x300) which turns into kind 2 the next time the player loses
 * the cap. Kind 3 does not render.
 *
 * The ten state handlers stay address-named: each one is a pointer-to-member
 * in a daSnowman_c::State, and the ROM symbol is the address.
 *
 * .text 0x02124090..0x02125f14, 35 functions. The registry factory
 * daSnowman_c_classInit (0x02125ec0) abuts the rest and is written last.
 *
 * deslop leftovers:
 * - func_ov081_0212423c: naming player mPosX/Y/Z is 0x190 -> 0x18c.
 * - func_ov081_021245e8: the same loads are 0xb8 -> 0xb4.
 * - func_ov081_02124b98: the same loads differ by 3 words; size stays 0x17c.
 * - func_ov081_021243cc: `if (actorID == PLAYER)` is 0x21c -> 0x210.
 *   KillByInvincibleChar as the method is 0x21c -> 0x228.
 * - func_ov081_021246a0: SpawnCoins as the method is 0xfc -> 0x104.
 * - func_ov081_02124894: SpawnCoins as the method is 0xf8 -> 0x100.
 * - dExtAnmModel_c::SetAnim as the method is +8. func_ov081_02124b08
 *   0x90 -> 0x98, 02124d14 0x3c -> 0x44, 02124ec0 0x60 -> 0x68,
 *   02125068 0x60 -> 0x68, 021250c8 0x138 -> 0x140, 02125208
 *   0x184 -> 0x18c, 0212538c 0xfc -> 0x104.
 * - func_ov081_021254d8: DropShadowRadHeight as the method is 0x2d8 -> 0x2e8.
 * - InitResources: dCcAcPos_c::Init as the method, measured together
 *   with dBgCh_Actr::Init, is 0x318 -> 0x328. dBgCh_Actr::Init alone
 *   stays 0x318 but mangles ciiP10Vector3_16S3_, not the ROM
 *   5Fix12IiES3_P10Vector3_16S5_.
 * - Behavior: cap->SetRanges does not compile. dActor_c has no SetRanges.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daSnowman_c_AssetId {
    ASSET_SNOWMAN_JUMP_ED_BCA = 0x041e,
    ASSET_SNOWMAN_JUMP_ST_BCA = 0x041f,
    ASSET_SNOWMAN_MODEL_BMD = 0x0420,
    ASSET_SNOWMAN_SLEEP_BCA = 0x0421,
    ASSET_SNOWMAN_THROW_BCA = 0x0422,
    ASSET_SNOWMAN_WAIT_BCA = 0x0423,
    ASSET_SNOW_BALL_BMD = 0x0424
};

#pragma defer_codegen off

#include "common.h"
#include "daSnowman_c.h"
#include "SharedFilePtr.h"
#include "PathPtr.h"
#include "SaveData.h"
#include "Player.h"
#include "Sound.h"
#include "math/Approach.h"

namespace cstd {
s16 atan2(s32, s32);
int fdiv(int numerator, int denominator);
}

/* Registry ids this file spawns or compares. */
enum {
    kPlayerId = 0xbf,     /* PLAYER */
    kSnowmanId = 0xdf,    /* SNOWMAN */
    kSnowballId = 0xe0,   /* SNOWBALL */
    kMarioCapId = 0x10d   /* OBJ_MARIO_CAP */
};

/* param1 values whose high byte is the kind. */
enum {
    kSpawnCapWearer = 0x200, /* kind 2 */
    kSpawnWatcher = 0x300    /* kind 3 */
};

extern "C" {
extern int RandomIntInternal(int *seed);
/* Sound::Play2D(2, id). This TU calls the wrapper, not Play2D. */
extern int data_0209e650;
extern s16 data_ov081_021289a4;
extern s16 data_ov081_021289a6;
extern s16 data_ov081_021289a8;
extern s16 Vec3_HorzAngle(const struct Vector3 *v0, const struct Vector3 *v1);
extern void ApproachAngle(void *p, int target, int a, int b, int c);
extern int AngleDiff(int a, int b);
extern void func_ov002_020aea30(void *self, void *actor, void *collision);
/* Sound::Play(3, id, pos) — PlayBank3. This TU calls the wrapper. */
extern void func_02012694(int a, void *p);
extern Vector3 data_ov081_02128998; /* cylinder offset (0, 0x1e000, 0) */
extern unsigned char data_0209f21c[];
extern void *data_0209f394[];
extern unsigned d_a_snowmanNewHold(unsigned a, unsigned b, int c, int d, int e, const void *f,
                                   void *g);
extern void Vec3_Sub(struct Vector3 *out, struct Vector3 *a, struct Vector3 *b);
extern int LenVec3(struct Vector3 *v);
extern Fix12i Vec3_Dist(const struct Vector3 *a, const struct Vector3 *b);
extern void Vec3_MulScalar(struct Vector3 *out, struct Vector3 *in, int s);
extern void SubVec3(struct Vector3 *a, struct Vector3 *b, struct Vector3 *c);
extern void Vec3_Asr(int *d, int *s, int sh);
extern void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationXYZExt(void *m, int x, int y, int z);
extern void MulMat4x3Mat4x3(void *dst, void *a, void *b);
extern void Matrix4x3_ApplyInPlaceToTranslation(void *m, int x, int y, int z);
extern struct Matrix4x3 data_020a0e68;
extern unsigned short DecIfAbove0_Short(unsigned short *p);
/* Parameter names keep this TU's @NNN. */
typedef void BgchInitAt_(void *self, void *actor, int b, int c, void *v16, int e);
extern int atan2_hold_d_a_snowman_1(int a, int b);
extern void Matrix4x3_FromRotationY(void *m, int ang);
extern int Vec3_HorzLen(void *v);
extern void Matrix4x3_ApplyInPlaceToRotationX(void *m, short ang);
extern void MulVec3Mat4x3(void *a, void *m, void *b);
}

/* File handles the static initializer constructs. The two models use
 * func_02017acc; the five animations use SharedFilePtr::Construct.
 * The wrapper names are long enough that their mangled constructor
 * symbols can be aliased onto those retail functions. */
struct daSnowmanModelFilePtr : SharedFilePtr {
    daSnowmanModelFilePtr(u32 fileID);
    ~daSnowmanModelFilePtr();
};
struct daSnowmanAnimationFilePtr : SharedFilePtr {
    daSnowmanAnimationFilePtr(u32 fileID);
    ~daSnowmanAnimationFilePtr();
};
typedef char daSnowmanModelFilePtr_size_must_be_8[sizeof(daSnowmanModelFilePtr) == 8 ? 1 : -1];
typedef char daSnowmanAnimationFilePtr_size_must_be_8[sizeof(daSnowmanAnimationFilePtr) == 8 ? 1 : -1];
typedef char daSnowman_State_size_must_be_0x10[sizeof(daSnowman_c::State) == 0x10 ? 1 : -1];

extern daSnowmanModelFilePtr data_ov081_02128d90; /* snowball model, file 0x424 */
extern daSnowmanModelFilePtr data_ov081_02128db0; /* body model, file 0x420 */
extern daSnowmanAnimationFilePtr gSnowmanWaitAnimationFile; /* wait, file 0x423 */
extern daSnowmanAnimationFilePtr data_ov081_02128db8; /* throw, file 0x422 */
extern daSnowmanAnimationFilePtr data_ov081_02128da8; /* sink, file 0x421 */
extern daSnowmanAnimationFilePtr gSnowmanJumpStAnimationFile; /* hop start, file 0x41f */
extern daSnowmanAnimationFilePtr gSnowmanJumpEdAnimationFile; /* hop land, file 0x41e */

/* The ten states, by the address of their .bss record, with the handlers
   the static initializer pairs into each as (enter, execute). */
extern daSnowman_c::State data_ov081_02128e14;   /* 02124ec0, 02124e64: face the player */
extern daSnowman_c::State data_ov081_02128e24;   /* 0212479c, 021246a0: melt away */
extern daSnowman_c::State data_ov081_02128e34;   /* 02124dfc, 02124d50: make a snowball */
extern daSnowman_c::State data_ov081_02128e44;   /* 02124d14, 02124b98: throw it */
extern daSnowman_c::State data_ov081_02128e54;   /* 0212538c, 02125208: walk the path */
extern daSnowman_c::State data_ov081_02128e64;   /* 02124b08, 021249f4: sink */
extern daSnowman_c::State data_ov081_02128e74;   /* 02125200, 021250c8: hop to a node */
extern daSnowman_c::State data_ov081_02128e84;   /* 02125068, 02125038: wait buried */
extern daSnowman_c::State data_ov081_02128e94;   /* 0212498c, 02124894: spin and fall */
extern daSnowman_c::State data_ov081_02128ea4;   /* 02124f7c, 02124f20: rise */

/* The touch handler tests the toucher's actor id as a 0/1 value. */
static inline int IsPlayer(dActor_c *a) {
    return a->actorID == kPlayerId;
}

// @symbol _ZN11daSnowman_cD1Ev
// @symbol _ZN11daSnowman_cD0Ev
daSnowman_c::~daSnowman_c()
{
}

// @symbol _ZN11daSnowman_c19func_ov081_02124134Ev
/* Drops the snowball and flings the cap. Kind 2 also plants a kind-3
   watcher at home. */
void daSnowman_c::func_ov081_02124134()
{
    dActor_c *a;
    unsigned int id = this->mSnowballID;
    if (id) {
        a = dActor_c::FindWithID(id);
        if (a) {
            a->mVertAccel = -0x2000;
            a->mTerminalVelocity = -0x28000;
            this->mSnowballID = 0;
        }
    }
    id = this->mCapUniqueID;
    if (id) {
        a = dActor_c::FindWithID(id);
        if (a) {
            a->mVertAccel = -0x2000;
            unsigned int rv = ((unsigned int)RandomIntInternal(&data_0209e650) >> 8) & 0xf;
            a->mPrevAngleX = 0;
            a->mPrevAngleY = rv << 0xc;
            a->mPrevAngleZ = 0;
            a->mSpeedX = 0;
            a->mVertSpeed = 0x14000;
            a->mSpeedZ = 0;
            a->mHorzSpeed = 0xa000;
            a->mCarryMatrix = 0;
        }
        func_02012790(0xa);
        this->mCapUniqueID = 0;
    }
    if (this->mType == 2) {
        dActor_c::Spawn(kSnowmanId, kSpawnWatcher, this->mHomePos, 0, this->mAreaId, -1);
    }
}

// @symbol _ZN11daSnowman_c19func_ov081_0212423cEi
/* Turns toward the closest visible player at the rate row idx gives. With
   idx 1 it also leans back while the player stays off to one side, and
   after 40 frames of that topples over. */
void daSnowman_c::func_ov081_0212423c(int idx)
{
    Player *player;
    struct Vector3 v;
    int ang;
    int off;
    s16 *maxStep;
    s16 *band;
    s16 *divisor;
    int lim;

    player = this->ClosestNonVanishPlayer();
    if (player == 0)
        return;

    /* Read through one base pointer, y and z first: naming the three
       fields directly reschedules the loads. */
    {
        int *pp = &player->mPosX;
        int py = pp[1];
        int pz = pp[2];
        v.x = pp[0];
        v.y = py;
        v.z = pz;
    }
    ang = Vec3_HorzAngle(&this->mPos, &v);

    /* Two rows of (divisor, band, maxStep), six bytes apart. Both rows in
       the ROM are 1, 0x1000, 0x1000; idx still selects which one. */
    off = idx * 6;
    maxStep = (s16 *)((char *)&data_ov081_021289a8 + off);
    band = (s16 *)((char *)&data_ov081_021289a6 + off);
    divisor = (s16 *)((char *)&data_ov081_021289a4 + off);

    ApproachAngle(&this->mPrevAngleY, ang, *divisor, *band, *maxStep);

    if (idx != 1) {
        this->mInitAngleY = 0;
        this->mTimer = 0;
    } else if (AngleDiff(ang, this->mAngleY) > 0x200) {
        lim = -0x2000;
        this->mInitAngleY -= 0x100;
        if (this->mInitAngleY < lim) {
            this->mInitAngleY = (s16)lim;
            this->mTimer++;
            if (this->mTimer > 0x28) {
                this->mStep = this->mAngleY - ang;
                func_ov081_02125488(&data_ov081_02128e94);
                return;
            }
        }
    } else {
        this->mTimer = 0;
        ApproachAngle(&this->mInitAngleY, 0, 1, 0x500, 0x500);
    }

    ApproachAngle(&this->mAngleX, this->mInitAngleY, *divisor, *band, *maxStep);
    this->mAngleY = this->mPrevAngleY;
}

// @symbol _ZN11daSnowman_c19func_ov081_021243ccEv
/* Reacts to whatever the collision cylinder touched this frame. */
void daSnowman_c::func_ov081_021243cc()
{
    Vector3 v;
    v = data_ov081_02128998;
    this->mdCcAcPos_c.SetPosRelativeToActor(v);

    u32 id = this->mdCcAcPos_c.otherOwner;
    if (id == 0) return;
    Player *found = (Player *)dActor_c::FindWithID(id);
    if (found == 0) return;

    int hit;
    s32 flags = this->mdCcAcPos_c.hitFlags;
    hit = 0;

    if (flags & 0x2000) {
        this->mDeathState = 2;
        func_ov002_020aea30(this, found, 0);
        this->mPrevAngleY = (u16)(this->HorzAngleToCPlayer() + 0x8000);
        hit = 1;
    }
    if (flags & 0x40000) {
        func_02012694(0xdb, &this->mCamSpacePosX);
        hit = 1;
        func_ov081_02125488(&data_ov081_02128e24);
    }
    if (IsPlayer(found)) {
        if (found->mIsVanish != 0) return;
        if (found->mIsMetal == 1) {
            this->mDeathState = 2;
            func_ov002_020aea30(this, found, 0);
            this->mPrevAngleY = (u16)(this->HorzAngleToCPlayer() + 0x8000);
            hit = 1;
        }
        if (flags & 0x10) {
            Vector3_16 vv;
            vv.x = (s16)-0x1200;
            vv.y = 0;
            vv.z = 0;
            KillByInvincibleChar(vv, *(Player *)found, 0);
            func_02012694(0x1d, &this->mCamSpacePosX);
            hit = 1;
        }
        if ((flags & 0x40) && (s32)found->param1 == 2) {
            this->mDeathState = 2;
            func_ov002_020aea30(this, found, 0);
            this->mDeathSound = 2;
            this->mPrevAngleY = (u16)(this->HorzAngleToCPlayer() + 0x8000);
            hit = 1;
        }
        if (hit == 0) {
            Vector3 hv;
            hv = this->mPos;
            found->Hurt(hv, 2, 0xc000, 1, 0, 1);
        }
    }
    if (hit != 1) return;
    func_ov081_02124134();
}

// @symbol _ZN11daSnowman_c19func_ov081_021245e8Ev
/* Distance to the closest player; leaves that player's index behind. */
int daSnowman_c::func_ov081_021245e8()
{
    int min = 0x2710000;
    int i = 0;
    Vector3 v;
    this->mClosestPlayerIdx = -1;
    if ((int)data_0209f21c[0] > 0) {
        do {
            Player *obj = (Player *)data_0209f394[i];
            if (obj != 0) {
                int dist;
                int *s = &obj->mPosX;   /* one base pointer; see 0212423c */
                v.x = s[0];
                v.y = s[1];
                v.z = s[2];
                dist = Vec3_Dist(&this->mPos, &v);
                if (i == 0) {
                    min = dist;
                    this->mClosestPlayerIdx = 0;
                } else if (dist < min) {
                    min = dist;
                    this->mClosestPlayerIdx = i;
                }
            }
            i++;
        } while (i < (int)data_0209f21c[0]);
    }
    return min;
}

// @symbol _ZN11daSnowman_c19func_ov081_021246a0Ev
/* State "melt", execute: shrink and sink, then pay out coins and die. */
int daSnowman_c::func_ov081_021246a0()
{
    Vector3 v;
    if (this->mType != 0 || this->mWithMeshClsn.IsOnGround() != 0) {
        ApproachLinear(this->mScaleX, 0x1700, 0x50);
        ApproachLinear(this->mScaleZ, 0x1700, 0x50);
        ApproachLinear(this->mScaleY, 0, 0x50);
        ApproachLinear(this->mSinkOffsetY, -0x4000, 0x199);
        /* Every read of mStateTimer here is ldrh: it is counted down
           through DecIfAbove0_Short, which takes it unsigned. */
        if ((u16)this->mStateTimer == 0) this->mStateTimer = 0x14;
        if (this->mScaleY < 0x50) {
            if (this->mCapPhase == 0) {
                v.x = this->mPosX;
                v.y = this->mPosY;
                v.z = this->mPosZ;
                SpawnCoins(v, this->mExtraCoinCount + 1, 0xa000, 0);
            }
            func_ov081_02124134();
            this->KillAndTrackInDeathTable();
        }
    }
    return 1;
}

// @symbol _ZN11daSnowman_c19func_ov081_0212479cEv
/* State "melt", enter. */
int daSnowman_c::func_ov081_0212479c()
{
    this->mInitAngleY = 0;
    this->mTimer = 0;
    this->mStateTimer = 0;
    if (this->mType == 0)
        this->mVertAccel = -0x2000;
    if (this->mCapUniqueID != 0) {
        dActor_c *a = dActor_c::FindWithID(this->mCapUniqueID);
        if (a != 0) {
            a->mVertAccel = -0x2000;
            int rv = ((unsigned int)RandomIntInternal(&data_0209e650) >> 8) & 0xf;
            a->mPrevAngleX = 0;
            a->mPrevAngleY = rv << 0xc;
            a->mPrevAngleZ = 0;
            a->mHorzSpeed = 0xa000;
            a->mSpeedX = 0;
            a->mVertSpeed = 0x14000;
            a->mSpeedZ = 0;
            a->mCarryMatrix = 0;
        }
        func_02012790(0xa);
        this->mCapUniqueID = 0;
    }
    if (this->mType == 2)
        dActor_c::Spawn(kSnowmanId, kSpawnWatcher, this->mHomePos, 0, this->mAreaId, -1);
    return 1;
}

// @symbol _ZN11daSnowman_c19func_ov081_02124894Ev
/* State "spin and fall", execute. */
int daSnowman_c::func_ov081_02124894()
{
    Vector3 v;
    /* Source written as (>0 ? sub : add) so mwccarm inversion emits the
       ROM's le/add arm first, then the unconditional sub arm. */
    if (this->mStep > 0)
        this->mInitAngleY -= 0x200;
    else
        this->mInitAngleY += 0x200;

    this->mPrevAngleY += this->mInitAngleY;
    ApproachAngle(&this->mAngleX, -0x2800, 1, 0x500, 0x500);
    this->mTimer++;
    if (this->mTimer > 0x28) {
        if (this->mCapPhase == 0) {
            v.x = this->mPosX;
            v.y = this->mPosY;
            v.z = this->mPosZ;
            SpawnCoins(v, this->mExtraCoinCount + 1, 0xa000, 0);
        }
        func_ov081_02124134();
        this->KillAndTrackInDeathTable();
    }
    return 1;
}

// @symbol _ZN11daSnowman_c19func_ov081_0212498cEv
/* State "spin and fall", enter. */
int daSnowman_c::func_ov081_0212498c()
{
    int id = this->mSnowballID;
    if (id != 0) {
        dActor_c *a = dActor_c::FindWithID(id);
        if (a != 0) {
            a->mVertAccel = -0x2000;
            a->mTerminalVelocity = -0x28000;
            this->mSnowballID = 0;
        }
    }
    func_02012694(0xdb, &this->mCamSpacePosX);
    this->mInitAngleY = 0;
    this->mTimer = 0;
    return 1;
}

// @symbol _ZN11daSnowman_c19func_ov081_021249f4Ev
/* State "sink", execute. */
int daSnowman_c::func_ov081_021249f4()
{
    struct Vector3 pos;
    int t;

    pos = this->mHomePos;
    this->mParticleID1 =
        Particle::System::New(this->mParticleID1, 0x11d, pos.x, pos.y, pos.z, 0, 0);
    pos.y = pos.y + 0x1e000;
    this->mParticleID2 =
        Particle::System::New(this->mParticleID2, 0x11e, pos.x, pos.y, pos.z, 0, 0);

    if (this->mHomePos.y > this->mPosY) {
        ApproachAngle(&this->mAngleX, this->mInitAngleY, 1, 0x1000, 0x1000);
    }
    t = this->mHomePos.y - 0x118000;
    if (t > this->mPosY) {
        this->mPosY = t;
        this->mVertSpeed = 0;
        this->mVertAccel = 0;
        func_ov081_02125488(&data_ov081_02128e84);
    }
    this->mPrevAngleY += 0x2000;
    this->mAngleY = this->mPrevAngleY;
    return 1;
}

// @symbol _ZN11daSnowman_c19func_ov081_02124b08Ev
/* State "sink", enter. */
int daSnowman_c::func_ov081_02124b08()
{
    unsigned int id = this->mSnowballID;
    if (id != 0) {
        dActor_c *a = dActor_c::FindWithID(id);
        if (a != 0) {
            a->mVertAccel = -0x2000;
            a->mTerminalVelocity = -0x28000;
            this->mSnowballID = 0;
        }
    }
    this->mVertSpeed = 0xa000;
    this->mVertAccel = -0x4000;
    this->mInitAngleY = 0;
    this->mAnmModel.SetAnim((BCA_File *)data_ov081_02128da8.filePtr, 0, 0x1000, 0);
    return 1;
}

// @symbol _ZN11daSnowman_c19func_ov081_02124b98Ev
/* State "throw", execute: lets go on frame 10, aimed at the closest player. */
int daSnowman_c::func_ov081_02124b98()
{
    Vector3 in, out, v[2];
    dActor_c *target;
    Player *player;

    if (this->mSnowballID != 0) {
        target = dActor_c::FindWithID(this->mSnowballID);
        if (target != 0) {
            target->mPosX = this->mHandPos.x;
            target->mPosY = this->mHandPos.y;
            target->mPosZ = this->mHandPos.z;
            if (this->mAnmModel.WillHitFrame(0xa) != 0) {
                in.x = 0; in.y = 0; in.z = 0x1e000;
                out.x = 0; out.y = 0; out.z = 0;
                v[0].x = 0; v[0].y = 0; v[0].z = 0;
                player = this->ClosestPlayer();
                if (player != 0) {
                    int angle;
                    int *q = &player->mPosX;   /* one base pointer; see 0212423c */
                    v[1].x = q[0];
                    v[1].y = q[1];
                    v[1].z = q[2];
                    v[0].x = v[1].x - this->mHandPos.x;
                    v[0].y = v[1].y - this->mHandPos.y;
                    v[0].z = v[1].z - this->mHandPos.z;
                    angle = cstd::atan2(v[0].x, v[0].z);
                    Matrix4x3_FromRotationY(&data_020a0e68, angle);
                    Matrix4x3_ApplyInPlaceToRotationX(
                        &data_020a0e68, (short)(-cstd::atan2(v[0].y, Vec3_HorzLen(&v[0]))));
                    MulVec3Mat4x3(&in, &data_020a0e68, &out);
                }
                out.y += 0x14000;
                target->mSpeedX = out.x;
                target->mVertSpeed = out.y;
                target->mSpeedZ = out.z;
                target->mVertAccel = -0x2000;
                this->mSnowballID = 0;
            }
        }
    }
    if (this->mAnmModel.Finished() != 0) {
        func_ov081_02125488(&data_ov081_02128e14);
    }
    return 1;
}

// @symbol _ZN11daSnowman_c19func_ov081_02124d14Ev
/* State "throw", enter. */
int daSnowman_c::func_ov081_02124d14()
{
    this->mAnmModel.SetAnim((BCA_File *)data_ov081_02128db8.filePtr, 0x40000000, 0x1000, 0);
    return 1;
}

// @symbol _ZN11daSnowman_c19func_ov081_02124d50Ev
/* State "make a snowball", execute. */
int daSnowman_c::func_ov081_02124d50()
{
    dActor_c *ball = dActor_c::FindWithID(this->mSnowballID);
    if (ball != 0 && ball->mVertAccel == 0) {
        func_ov081_0212423c(1);
        ball->mPosX = this->mHandPos.x;
        ball->mPosY = this->mHandPos.y;
        ball->mPosZ = this->mHandPos.z;
    }

    if ((u16)this->mStateTimer == 0) {
        func_ov081_02125488(&data_ov081_02128e44);
        return 1;
    }

    if (func_ov081_021245e8() > 0x320000) {
        func_ov081_02125488(&data_ov081_02128e64);
    }
    return 1;
}

// @symbol _ZN11daSnowman_c19func_ov081_02124dfcEv
/* State "make a snowball", enter: spawns it in the hand. */
int daSnowman_c::func_ov081_02124dfc()
{
    dActor_c *a = dActor_c::Spawn(kSnowballId, 0, this->mHandPos, 0, this->mAreaId, -1);
    if (a)
        this->mSnowballID = a->uniqueID;
    this->mStateTimer = 0x64;
    this->mStep = 0;
    return 1;
}

// @symbol _ZN11daSnowman_c19func_ov081_02124e64Ev
/* State "face the player", execute. */
int daSnowman_c::func_ov081_02124e64()
{
    func_ov081_0212423c(0);
    if ((u16)this->mStateTimer == 0)
        func_ov081_02125488(&data_ov081_02128e34);
    if (func_ov081_021245e8() > 0x320000)
        func_ov081_02125488(&data_ov081_02128e64);
    return 1;
}

// @symbol _ZN11daSnowman_c19func_ov081_02124ec0Ev
/* State "face the player", enter. */
int daSnowman_c::func_ov081_02124ec0()
{
    this->mAnmModel.SetAnim((BCA_File *)gSnowmanWaitAnimationFile.filePtr, 0, 0x1000, 0);
    if (this->mType == 2)
        this->mFlags = 0x18000002;
    else
        this->mFlags = 0x10000002;
    this->mStateTimer = 0x14;
    return 1;
}

// @symbol _ZN11daSnowman_c19func_ov081_02124f20Ev
/* State "rise", execute. */
int daSnowman_c::func_ov081_02124f20()
{
    if (this->mVertSpeed < 0) {
        if (this->mHomePos.y > this->mPosY) {
            this->mVertSpeed = 0;
            this->mVertAccel = 0;
            this->mPosY = this->mHomePos.y;
            func_ov081_02125488(&data_ov081_02128e14);
        }
    }
    func_ov081_0212423c(0);
    return 1;
}

// @symbol _ZN11daSnowman_c19func_ov081_02124f7cEv
/* State "rise", enter. */
int daSnowman_c::func_ov081_02124f7c()
{
    struct Vector3 pos;

    this->mVertSpeed = 0x3c000;
    this->mVertAccel = -0x4000;
    func_02012694(0xdc, &this->mCamSpacePosX);
    pos.x = this->mHomePos.x;
    pos.y = this->mHomePos.y;
    pos.z = this->mHomePos.z;
    this->mParticleID1 =
        Particle::System::New(this->mParticleID1, 0x11a, pos.x, pos.y, pos.z, 0, 0);
    pos.y = pos.y + 0x1e000;
    this->mParticleID2 =
        Particle::System::New(this->mParticleID2, 0x11b, pos.x, pos.y, pos.z, 0, 0);
    return 1;
}

// @symbol _ZN11daSnowman_c19func_ov081_02125038Ev
/* State "wait buried", execute: rise when a player comes near. */
int daSnowman_c::func_ov081_02125038()
{
    if (func_ov081_021245e8() < 0x258000)
        func_ov081_02125488(&data_ov081_02128ea4);
    return 1;
}

// @symbol _ZN11daSnowman_c19func_ov081_02125068Ev
/* State "wait buried", enter. */
int daSnowman_c::func_ov081_02125068()
{
    int v = this->mType;
    if (v != 2 && v != 3) {
        this->mFlags = 3;
    } else if (v == 2) {
        this->mFlags = 0x8000002;
    }
    this->mAnmModel.SetAnim((BCA_File *)gSnowmanWaitAnimationFile.filePtr, 0, 0x1000, 0);
    return 1;
}

// @symbol _ZN11daSnowman_c19func_ov081_021250c8Ev
/* State "hop to a node", execute. */
int daSnowman_c::func_ov081_021250c8()
{
    struct Vector3 node;
    struct Vector3 diff;
    struct Vector3 scaled;
    int len;

    if (this->mWithMeshClsn.IsOnGround()) {
        func_02012694(0xe5, &this->mCamSpacePosX);
        func_ov081_02125488(&data_ov081_02128e54);
        this->mAnmModel.SetAnim((BCA_File *)gSnowmanJumpEdAnimationFile.filePtr, 0x40000000, 0x1000,
                                0);
        return 1;
    }

    PathPtr p;
    p.FromID(this->mPathId);
    p.GetNode(node, this->mPathNodeIndex);
    node.y = this->mPosY;
    Vec3_Sub(&diff, &this->mPos, &node);
    len = LenVec3(&diff);
    ApproachAngle(&this->mPrevAngleY, Vec3_HorzAngle(&this->mPos, &node), 1, 0x1000, 0x500);
    if (len == 0 || len <= this->mHopStep)
        return 1;
    {
        int q = cstd::fdiv(this->mHopStep, len);
        Vec3_MulScalar(&scaled, &diff, q);
        SubVec3(&this->mPos, &scaled, &this->mPos);
    }
    return 1;
}

// @symbol _ZN11daSnowman_c19func_ov081_02125200Ev
/* State "hop to a node", enter: nothing to do. */
int daSnowman_c::func_ov081_02125200()
{
    return 1;
}

// @symbol _ZN11daSnowman_c19func_ov081_02125208Ev
/* State "walk the path", execute. */
int daSnowman_c::func_ov081_02125208()
{
    struct Vector3 node;

    if ((u16)this->mStateTimer == 0) {
        if (this->mStep == 1) {
            this->mVertSpeed = 0x14000;
            this->mVertAccel = -0x4000;
            this->mStep = 2;
            func_02012694(0xdd, &this->mCamSpacePosX);
        }

        PathPtr p;
        p.FromID(this->mPathId);
        p.GetNode(node, this->mPathNodeIndex);
        ApproachAngle(&this->mPrevAngleY, Vec3_HorzAngle(&this->mPos, &node), 1, 0x1000, 0x500);

        if (AngleDiff(this->mPrevAngleY, Vec3_HorzAngle(&this->mPos, &node)) < 0x100 &&
            this->mWithMeshClsn.IsOnGround()) {
            int st = this->mStep;
            if (st != 0) {
                if (st == 1)
                    func_02012694(0xe5, &this->mCamSpacePosX);
                this->mStep++;
                if (this->mStep < 0xb)
                    return 1;
            }
            this->mStep = 0;
            this->mVertSpeed = 0x3a000;
            this->mVertAccel = -0x4000;
            this->mAnmModel.SetAnim((BCA_File *)gSnowmanJumpStAnimationFile.filePtr, 0x40000000,
                                    0x1000, 0);
            func_02012694(0xdd, &this->mCamSpacePosX);
            func_ov081_02125488(&data_ov081_02128e74);
        }
    }
    return 1;
}

// @symbol _ZN11daSnowman_c19func_ov081_0212538cEv
/* State "walk the path", enter: step onto the next node when close enough. */
int daSnowman_c::func_ov081_0212538c()
{
    struct Vector3 node;
    struct Vector3 diff;
    int len;

    PathPtr pp;
    pp.FromID(this->mPathId);
    pp.GetNode(node, this->mPathNodeIndex);
    node.y = this->mPosY;
    Vec3_Sub(&diff, &this->mPos, &node);
    len = LenVec3(&diff);
    this->mHopStep = 0xa000;
    this->mStep = 0;
    if (len == 0 || len <= this->mHopStep) {
        this->mPosX = node.x;
        this->mPosY = node.y;
        this->mPosZ = node.z;
        this->mPathNodeIndex++;
        if (this->mPathNodeIndex >= this->mPathNodeCount)
            this->mPathNodeIndex = 0;
        this->mStep = 1;
    }
    this->mStateTimer = 0xa;
    this->mAnmModel.SetAnim((BCA_File *)gSnowmanWaitAnimationFile.filePtr, 0, 0x1000, 0);
    return 1;
}

// @symbol _ZN11daSnowman_c19func_ov081_02125488EPNS_5StateE
/* Enters a state: records it, then runs its enter handler if it has one. */
int daSnowman_c::func_ov081_02125488(State *state)
{
    this->mState = state;
    if (this->mState->enter == 0)
        return 1;
    return (this->*this->mState->enter)();
}

// @symbol _ZN11daSnowman_c19func_ov081_021254d8Ev
/* Poses the model, then carries the cap on bone 5 and the snowball on
   bone 3, and drops the shadow while standing on the path. */
void daSnowman_c::func_ov081_021254d8()
{
    int src[3], dst[3];
    int t;
    dActor_c *actor;

    this->mAnmModel.UpdateVerts();

    src[0] = this->mPosX;
    t = this->mPosY;
    src[1] = t;
    src[2] = this->mPosZ;
    src[1] = t + this->mSinkOffsetY;
    Vec3_Asr(dst, src, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, dst[0], dst[1], dst[2]);

    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68,
        this->mAngleX, this->mAngleY, this->mAngleZ);
    this->mAnmModel.mat4x3 = data_020a0e68;

    if (this->mCapUniqueID != 0) {
        this->mCapPos.x = 0;
        this->mCapPos.y = 0;
        this->mCapPos.z = 0;
        actor = dActor_c::FindWithID(this->mCapUniqueID);
        if (actor != 0) {
            MulMat4x3Mat4x3(&this->mAnmModel.data.transforms[5], &this->mAnmModel.mat4x3,
                            &this->mCapMatrix);

            this->mCapPos.x = data_020a0e68.m[9];
            this->mCapPos.y = data_020a0e68.m[10];
            this->mCapPos.z = data_020a0e68.m[11];
            this->mCapPos.x <<= 3;
            this->mCapPos.y <<= 3;
            this->mCapPos.z <<= 3;
            actor->mPosX = this->mCapPos.x;
            actor->mPosY = this->mCapPos.y;
            actor->mPosZ = this->mCapPos.z;

            Matrix4x3_FromTranslation(&data_020a0e68, 0, 0x4000, 0);
            Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68, 0, 0x2000, 0);
            MulMat4x3Mat4x3(&data_020a0e68, &this->mCapMatrix, &this->mCapMatrix);
            actor->mCarryMatrix = &this->mCapMatrix;
        }
    }

    this->mHandPos.x = 0;
    this->mHandPos.y = 0;
    this->mHandPos.z = 0;
    data_020a0e68 = this->mAnmModel.mat4x3;
    MulMat4x3Mat4x3(&this->mAnmModel.data.transforms[3], &data_020a0e68, &data_020a0e68);
    Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, 0xc000, -0x1000, 0);

    this->mHandPos.x = data_020a0e68.m[9];
    this->mHandPos.y = data_020a0e68.m[10];
    this->mHandPos.z = data_020a0e68.m[11];
    this->mHandPos.x <<= 3;
    this->mHandPos.y <<= 3;
    this->mHandPos.z <<= 3;

    if (this->mType != 0)
        return;

    Matrix4x3_FromTranslation(&data_020a0e68,
        this->mPosX >> 3,
        (this->mPosY - 0xe000) >> 3,
        this->mPosZ >> 3);
    this->mShadowMatrix = data_020a0e68;
    DropShadowRadHeight(this->mShadowModel, this->mShadowMatrix, 0x78000, 0xc8000, 0xf);
}

// @symbol _ZN11daSnowman_c16CleanupResourcesEv
int daSnowman_c::CleanupResources()
{
    data_ov081_02128d90.Release();
    data_ov081_02128db0.Release();
    gSnowmanWaitAnimationFile.Release();
    data_ov081_02128db8.Release();
    data_ov081_02128da8.Release();
    gSnowmanJumpStAnimationFile.Release();
    gSnowmanJumpEdAnimationFile.Release();
    return 1;
}

// @symbol _ZN11daSnowman_c16OnPendingDestroyEv
void daSnowman_c::OnPendingDestroy()
{
}

// @symbol _ZN11daSnowman_c6RenderEv
int daSnowman_c::Render()
{
    int s = mType;
    if (s == 3) return 1;
    if (s == 2) {
        if (mCapUniqueID)
            mAnmModel.HideMaterial(0, 2);
        else
            mAnmModel.ShowMaterial(0, 2);
    }
    mAnmModel.dExtSimpleModel_c::Render(&mScale);
    return 1;
}

// @symbol _ZN11daSnowman_c8BehaviorEv
int daSnowman_c::Behavior()
{
    Player *p;
    dActor_c *cap;
    if (mType == 3) {
        switch (mCapPhase) {
        case 0:
            if (SaveData::HasPlayerLostCap()) mCapPhase = 1;
            else mCapPhase = 2;
            break;
        case 1:
            if (!SaveData::HasPlayerLostCap()) mCapPhase = 2;
            break;
        case 2:
            if (SaveData::HasPlayerLostCap()) mType = 2;
            break;
        }
        return 1;
    }
    if (UpdateKillByInvincibleChar(mWithMeshClsn, mAnmModel, 3))
        return 1;
    if (mDeathState != 0) {
        if (UpdateDeath(mWithMeshClsn) && mType == 2) {
            dActor_c::Spawn(kSnowmanId, kSpawnWatcher, mHomePos, 0, mAreaId, -1);
            mType = 0;
        }
        if (mDeathState == 0 && mDeathSound != 0) {
            func_02012694(0x166, &mCamSpacePosX);
            mDeathSound = 0;
        }
        func_ov081_021254d8();
        return 1;
    }
    if (mType == 2
        && mState != &data_ov081_02128e94
        && mState != &data_ov081_02128e24
        && SaveData::HasPlayerLostCap()
        && mCapUniqueID == 0) {

        p = ClosestPlayer();
        if (p != 0) {
            int param = 0xc;
            param = param | (p->mCharacter << 8);
            cap = dActor_c::Spawn(kMarioCapId, param, mPos, 0, mAreaId, -1);
            if (cap != 0) {
                cap->SetRanges(0x64000, 0xc8000, 0x1000000, 0x1000000);
                mCapUniqueID = cap->uniqueID;
            }
        }

    }
    mAngleX = mPrevAngleX;
    mAngleY = mPrevAngleY;
    mAngleZ = mPrevAngleZ;
    DecIfAbove0_Short((unsigned short *)&mStateTimer);
    if (mState->execute != 0)
        (this->*mState->execute)();
    UpdatePos(&mdCcAcPos_c);
    if (mType == 0)
        UpdateWMClsn(mWithMeshClsn, 0);
    func_ov081_021254d8();
    if (mState != &data_ov081_02128e84
        && mState != &data_ov081_02128e64
        && mState != &data_ov081_02128e94
        && mState != &data_ov081_02128e24)
        func_ov081_021243cc();
    mdCcAcPos_c.Clear();
    {
        p = ClosestPlayer();
        if (p != 0 && p->mIsVanish == 0)
            mdCcAcPos_c.Update();
    }
    mAnmModel.speed = 0x1000;
    mAnmModel.Advance();
    return 1;
}

// @symbol _ZN11daSnowman_c13InitResourcesEv
int daSnowman_c::InitResources()
{
    Vector3 v;

    mAnmModel.SetFile((BMD_File *)dExtSimpleModel_c::LoadFile(data_ov081_02128db0), 1, -1);
    dExtSimpleModel_c::LoadFile(data_ov081_02128d90);
    mShadowModel.InitCylinder();
    dExtFrameCtrl_c::LoadFile(gSnowmanWaitAnimationFile);
    dExtFrameCtrl_c::LoadFile(data_ov081_02128db8);
    dExtFrameCtrl_c::LoadFile(data_ov081_02128da8);
    dExtFrameCtrl_c::LoadFile(gSnowmanJumpStAnimationFile);
    dExtFrameCtrl_c::LoadFile(gSnowmanJumpEdAnimationFile);

    mPathId = (s32)param1 & 0xff;
    mType = ((s32)param1 & 0xff00) >> 8;
    if (mPathId == 0xff)
        mPathId = 0;
    if (mType == 0xff)
        mType = 0;

    mHomePos.x = mPosX;
    mHomePos.y = mPosY;
    mHomePos.z = mPosZ;

    if (GetBitInDeathTable() != 0) {
        if (mType == 2) {
            if (SaveData::HasPlayerLostCap() != 0) {
                dActor_c::Spawn(kSnowmanId, kSpawnCapWearer, mHomePos, 0, mAreaId, -1);
            } else {
                dActor_c::Spawn(kSnowmanId, kSpawnWatcher, mHomePos, 0, mAreaId, -1);
            }
        }
        if (mType != 3)
            return 0;
    }

    mSnowballID = 0;
    mCapUniqueID = 0;

    if (mType == 0) {
        PathPtr pp;
        pp.FromID(mPathId);
        mPathNodeCount = pp.NumNodes();
    }

    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    mTerminalVelocity = -0xc8000;

    v = data_ov081_02128998;
    mdCcAcPos_c.Init(this, v, 0x3c000, 0x96000, 0x200004, 0x42050);

    mAngleY = mPrevAngleY;
    mInitAngleY = mAngleY;
    mWithMeshClsn.Init(this, 0x14000, 0xf000, 0, 0);

    mCoinType = 1;
    mExtraCoinCount = 2;

    if (mType == 0) {
        PathPtr pp;
        pp.FromID(mPathId);
        pp.GetNode(mPos, mPathNodeIndex);
        mPathNodeIndex = 1;
        mVertAccel = -0x2000;
        mFlags = 0x10000000;
        func_ov081_02125488(&data_ov081_02128e54);
    } else {
        mPosY -= 0x118000;
        func_ov081_02125488(&data_ov081_02128e84);
        if (mType == 3) {
            mCoinType = 0;
            mExtraCoinCount = 0;
            mFlags = 2;
        }
        if (mType == 2)
            mFlags = 0x8000002;
        else
            mFlags = 3;
    }

    return 1;
}

// @symbol _ZN11daSnowman_c16OnAimedAtWithEggEv
/* Slot 29. Callers add this to the actor's Y before the egg-aim particles. */
s32 daSnowman_c::OnAimedAtWithEgg() {
    return 0x98000;
}

// @symbol daSnowman_c_classInit
extern "C" daSnowman_c *daSnowman_c_classInit()
{
    return new daSnowman_c();
}

/* Formerly the handwritten __sinit_ov081_02128154. Definition order is
 * the retail initializer's construction order. */
daSnowmanModelFilePtr data_ov081_02128d90(ASSET_SNOW_BALL_BMD);
daSnowmanModelFilePtr data_ov081_02128db0(ASSET_SNOWMAN_MODEL_BMD);
daSnowmanAnimationFilePtr gSnowmanWaitAnimationFile(ASSET_SNOWMAN_WAIT_BCA);
daSnowmanAnimationFilePtr data_ov081_02128db8(ASSET_SNOWMAN_THROW_BCA);
daSnowmanAnimationFilePtr data_ov081_02128da8(ASSET_SNOWMAN_SLEEP_BCA);
daSnowmanAnimationFilePtr gSnowmanJumpStAnimationFile(ASSET_SNOWMAN_JUMP_ST_BCA);
daSnowmanAnimationFilePtr gSnowmanJumpEdAnimationFile(ASSET_SNOWMAN_JUMP_ED_BCA);

daSnowman_c::State data_ov081_02128e54(&daSnowman_c::func_ov081_0212538c, &daSnowman_c::func_ov081_02125208);
daSnowman_c::State data_ov081_02128e74(&daSnowman_c::func_ov081_02125200, &daSnowman_c::func_ov081_021250c8);
daSnowman_c::State data_ov081_02128e84(&daSnowman_c::func_ov081_02125068, &daSnowman_c::func_ov081_02125038);
daSnowman_c::State data_ov081_02128ea4(&daSnowman_c::func_ov081_02124f7c, &daSnowman_c::func_ov081_02124f20);
daSnowman_c::State data_ov081_02128e14(&daSnowman_c::func_ov081_02124ec0, &daSnowman_c::func_ov081_02124e64);
daSnowman_c::State data_ov081_02128e34(&daSnowman_c::func_ov081_02124dfc, &daSnowman_c::func_ov081_02124d50);
daSnowman_c::State data_ov081_02128e44(&daSnowman_c::func_ov081_02124d14, &daSnowman_c::func_ov081_02124b98);
daSnowman_c::State data_ov081_02128e64(&daSnowman_c::func_ov081_02124b08, &daSnowman_c::func_ov081_021249f4);
daSnowman_c::State data_ov081_02128e94(&daSnowman_c::func_ov081_0212498c, &daSnowman_c::func_ov081_02124894);
daSnowman_c::State data_ov081_02128e24(&daSnowman_c::func_ov081_0212479c, &daSnowman_c::func_ov081_021246a0);
