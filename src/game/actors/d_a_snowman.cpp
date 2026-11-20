//cpp
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
 * - RotateTowardPlayer: naming player mPos.x/.y/.z is 0x190 -> 0x18c.
 * - FindClosestPlayerDist: the same loads are 0xb8 -> 0xb4.
 * - StateThrowSnowballExecute: the same loads differ by 3 words; size stays 0x17c.
 * - CheckAttacks: `if (actorID == PLAYER)` is 0x21c -> 0x210.
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
#include "Particle__System.h"
#include "ParticleIds.h"
#include "daSnowman_c.h"
#include "SharedFilePtr.h"
#include "PathPtr.h"
#include "SaveData.h"
#include "daPly_c.h"
#include "Sound.h"
#include "math/Approach.h"
#include "LevelRng.h"
#include "nitro/fx/fx_cp.h"
#include "math/MtxStack.h"
#include "math/CalcLib.h"

namespace cstd {
s16 atan2(s32, s32);
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

/* Sound::Play2D(2, id). This TU calls the wrapper, not Play2D. */
/* How fast RotateTowardPlayer turns, as ApproachAngle's three limits. */
struct daSnowmanTurnRate {
    s16 divisor;
    s16 band;
    s16 maxStep;
};
extern daSnowmanTurnRate data_ov081_021289a4[2];
extern "C" int AngleDiff(int a, int b);
/* Sound::Play(3, id, pos) — PlayBank3. This TU calls the wrapper. */
void func_02012694(unsigned int a, const Vector3 *p);
/* Cylinder offset (0, 0x1e000, 0): a POD twin, since a Vector3 global
 * would carry its constructor into .bss. */
struct SnowmanCylinderOffset {
    fx32 x, y, z;
};
extern SnowmanCylinderOffset data_ov081_02128998;
extern unsigned char gNumPlayers;
extern daPly_c *gPlayers[];
extern union Matrix4x3 gScratchMtx;

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

daSnowman_c::~daSnowman_c()
{
}

/* Drops the snowball and flings the cap. Kind 2 also plants a kind-3
   watcher at home. */
void daSnowman_c::ReleaseHeldObjects() {
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
            unsigned int rv = ((unsigned int)RandomIntInternal(&data_0209e650.value) >> 8) & 0xf;
            a->mPrevAngle.x = 0;
            a->mPrevAngle.y = rv << 0xc;
            a->mPrevAngle.z = 0;
            a->mSpeed.x = 0;
            a->mSpeed.y = 0x14000;
            a->mSpeed.z = 0;
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

/* Turns toward the closest visible player at the rate row idx gives. With
   idx 1 it also leans back while the player stays off to one side, and
   after 40 frames of that topples over. */
void daSnowman_c::RotateTowardPlayer(int idx) {
    daPly_c *player;
    struct Vector3 v;
    short ang;
    int lim;

    player = this->ClosestNonVanishPlayer();
    if (player == 0)
        return;

    /* Read through one base pointer, y and z first: naming the three
       fields directly reschedules the loads. */
    {
        int *pp = &player->mPos.x;
        int py = pp[1];
        int pz = pp[2];
        v.x = pp[0];
        v.y = py;
        v.z = pz;
    }
    ang = Vec3_HorzAngle(&this->mPos, &v);

    /* Both rows in the ROM are 1, 0x1000, 0x1000; idx still selects one. */
    ApproachAngle(&this->mPrevAngle.y, ang, data_ov081_021289a4[idx].divisor,
                  data_ov081_021289a4[idx].band, data_ov081_021289a4[idx].maxStep);

    if (idx != 1) {
        this->mInitAngleY = 0;
        this->mTimer = 0;
    } else if (AngleDiff(ang, this->mAngle.y) > 0x200) {
        lim = -0x2000;
        this->mInitAngleY -= 0x100;
        if (this->mInitAngleY < lim) {
            this->mInitAngleY = lim;
            this->mTimer++;
            if (this->mTimer > 40) {
                this->mStep = this->mAngle.y - ang;
                SetState(&data_ov081_02128e94);
                return;
            }
        }
    } else {
        this->mTimer = 0;
        ApproachAngle(&this->mInitAngleY, 0, 1, 0x500, 0x500);
    }

    ApproachAngle(&this->mAngle.x, this->mInitAngleY, data_ov081_021289a4[idx].divisor,
                  data_ov081_021289a4[idx].band, data_ov081_021289a4[idx].maxStep);
    this->mAngle.y = this->mPrevAngle.y;
}

/* Reacts to whatever the collision cylinder touched this frame. */
void daSnowman_c::CheckAttacks() {
    Vector3 v;
    v.x = data_ov081_02128998.x;
    v.y = data_ov081_02128998.y;
    v.z = data_ov081_02128998.z;
    this->mdCcAcPos_c.SetPosRelativeToActor(v);

    u32 id = this->mdCcAcPos_c.otherOwner;
    if (id == 0) return;
    daPly_c *found = (daPly_c *)dActor_c::FindWithID(id);
    if (found == 0) return;

    int hit;
    s32 flags = this->mdCcAcPos_c.hitFlags;
    hit = 0;

    if (flags & 0x2000) {
        this->mDeathState = 2;
        KillByAttack(found, 0);
        this->mPrevAngle.y = (this->HorzAngleToCPlayer() + 0x8000);
        hit = 1;
    }
    if (flags & 0x40000) {
        func_02012694(0xdb, &this->mCamSpacePos);
        hit = 1;
        SetState(&data_ov081_02128e24);
    }
    if (IsPlayer(found)) {
        if (found->mIsVanish != 0) return;
        if (found->mIsMetal == 1) {
            this->mDeathState = 2;
            KillByAttack(found, 0);
            this->mPrevAngle.y = (this->HorzAngleToCPlayer() + 0x8000);
            hit = 1;
        }
        if (flags & 0x10) {
            Vector3_16 vv;
            vv.x = -0x1200;
            vv.y = 0;
            vv.z = 0;
            KillByInvincibleChar(vv, *found, 0);
            func_02012694(0x1d, &this->mCamSpacePos);
            hit = 1;
        }
        if ((flags & 0x40) && found->param1 == 2) {
            this->mDeathState = 2;
            KillByAttack(found, 0);
            this->mDeathSound = 2;
            this->mPrevAngle.y = (this->HorzAngleToCPlayer() + 0x8000);
            hit = 1;
        }
        if (hit == 0) {
            Vector3 hv;
            hv = this->mPos;
            found->Hurt(hv, 2, 0xc000, 1, 0, 1);
        }
    }
    if (hit != 1) return;
    ReleaseHeldObjects();
}

/* Distance to the closest player; leaves that player's index behind. */
int daSnowman_c::FindClosestPlayerDist() {
    int min = 0x2710000;
    int i = 0;
    Vector3 v;
    this->mClosestPlayerIdx = -1;
    if ((int)gNumPlayers > 0) {
        do {
            daPly_c *obj = gPlayers[i];
            if (obj != 0) {
                int dist;
                int *s = &obj->mPos.x; /* one base pointer; see 0212423c */
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
        } while (i < gNumPlayers);
    }
    return min;
}

/* State "melt", execute: shrink and sink, then pay out coins and die. */
int daSnowman_c::StateMeltExecute() {
    Vector3 v;
    if (this->mType != 0 || this->mWithMeshClsn.IsOnGround() != 0) {
        ApproachLinear(this->mScale.x, 0x1700, 0x50);
        ApproachLinear(this->mScale.z, 0x1700, 0x50);
        ApproachLinear(this->mScale.y, 0, 0x50);
        ApproachLinear(this->mSinkOffsetY, -0x4000, 0x199);
        /* Every read of mStateTimer here is ldrh: it is counted down
           through DecIfAbove0_Short, which takes it unsigned. */
        if (this->mStateTimer == 0)
            this->mStateTimer = 20;
        if (this->mScale.y < 0x50) {
            if (this->mCapPhase == 0) {
                v.x = this->mPos.x;
                v.y = this->mPos.y;
                v.z = this->mPos.z;
                SpawnCoins(v, this->mExtraCoinCount + 1, 0xa000, 0);
            }
            ReleaseHeldObjects();
            this->KillAndTrackInDeathTable();
        }
    }
    return 1;
}

/* State "melt", enter. */
int daSnowman_c::StateMeltEnter() {
    this->mInitAngleY = 0;
    this->mTimer = 0;
    this->mStateTimer = 0;
    if (this->mType == 0)
        this->mVertAccel = -0x2000;
    if (this->mCapUniqueID != 0) {
        dActor_c *a = dActor_c::FindWithID(this->mCapUniqueID);
        if (a != 0) {
            a->mVertAccel = -0x2000;
            int rv = ((unsigned int)RandomIntInternal(&data_0209e650.value) >> 8) & 0xf;
            a->mPrevAngle.x = 0;
            a->mPrevAngle.y = rv << 0xc;
            a->mPrevAngle.z = 0;
            a->mHorzSpeed = 0xa000;
            a->mSpeed.x = 0;
            a->mSpeed.y = 0x14000;
            a->mSpeed.z = 0;
            a->mCarryMatrix = 0;
        }
        func_02012790(0xa);
        this->mCapUniqueID = 0;
    }
    if (this->mType == 2)
        dActor_c::Spawn(kSnowmanId, kSpawnWatcher, this->mHomePos, 0, this->mAreaId, -1);
    return 1;
}

/* State "spin and fall", execute. */
int daSnowman_c::StateDeathExecute() {
    Vector3 v;
    /* Source written as (>0 ? sub : add) so mwccarm inversion emits the
       ROM's le/add arm first, then the unconditional sub arm. */
    if (this->mStep > 0)
        this->mInitAngleY -= 0x200;
    else
        this->mInitAngleY += 0x200;

    this->mPrevAngle.y += this->mInitAngleY;
    ApproachAngle(&this->mAngle.x, -0x2800, 1, 0x500, 0x500);
    this->mTimer++;
    if (this->mTimer > 40) {
        if (this->mCapPhase == 0) {
            v.x = this->mPos.x;
            v.y = this->mPos.y;
            v.z = this->mPos.z;
            SpawnCoins(v, this->mExtraCoinCount + 1, 0xa000, 0);
        }
        ReleaseHeldObjects();
        this->KillAndTrackInDeathTable();
    }
    return 1;
}

/* State "spin and fall", enter. */
int daSnowman_c::StateDeathEnter() {
    int id = this->mSnowballID;
    if (id != 0) {
        dActor_c *a = dActor_c::FindWithID(id);
        if (a != 0) {
            a->mVertAccel = -0x2000;
            a->mTerminalVelocity = -0x28000;
            this->mSnowballID = 0;
        }
    }
    func_02012694(0xdb, &this->mCamSpacePos);
    this->mInitAngleY = 0;
    this->mTimer = 0;
    return 1;
}

/* State "sink", execute. */
int daSnowman_c::StateBurrowExecute() {
    struct Vector3 pos;
    int t;

    pos = this->mHomePos;
    this->mParticleID1 =
        Particle::System::New(this->mParticleID1, kPtclSnowmanSinkA, pos.x, pos.y, pos.z, 0, 0);
    pos.y = pos.y + 0x1e000;
    this->mParticleID2 =
        Particle::System::New(this->mParticleID2, kPtclSnowmanSinkB, pos.x, pos.y, pos.z, 0, 0);

    if (this->mHomePos.y > this->mPos.y) {
        ApproachAngle(&this->mAngle.x, this->mInitAngleY, 1, 0x1000, 0x1000);
    }
    t = this->mHomePos.y - 0x118000;
    if (t > this->mPos.y) {
        this->mPos.y = t;
        this->mSpeed.y = 0;
        this->mVertAccel = 0;
        SetState(&data_ov081_02128e84);
    }
    this->mPrevAngle.y += 0x2000;
    this->mAngle.y = this->mPrevAngle.y;
    return 1;
}

/* State "sink", enter. */
int daSnowman_c::StateBurrowEnter() {
    unsigned int id = this->mSnowballID;
    if (id != 0) {
        dActor_c *a = dActor_c::FindWithID(id);
        if (a != 0) {
            a->mVertAccel = -0x2000;
            a->mTerminalVelocity = -0x28000;
            this->mSnowballID = 0;
        }
    }
    this->mSpeed.y = 0xa000;
    this->mVertAccel = -0x4000;
    this->mInitAngleY = 0;
    this->mAnmModel.SetAnim((BCA_File *)data_ov081_02128da8.filePtr, 0, 0x1000, 0);
    return 1;
}

/* State "throw", execute: lets go on frame 10, aimed at the closest player. */
int daSnowman_c::StateThrowSnowballExecute() {
    Vector3 in, out, v[2];
    dActor_c *target;
    daPly_c *player;

    if (this->mSnowballID != 0) {
        target = dActor_c::FindWithID(this->mSnowballID);
        if (target != 0) {
            target->mPos.x = this->mHandPos.x;
            target->mPos.y = this->mHandPos.y;
            target->mPos.z = this->mHandPos.z;
            if (this->mAnmModel.WillHitFrame(0xa) != 0) {
                in.x = 0; in.y = 0; in.z = 0x1e000;
                out.x = 0; out.y = 0; out.z = 0;
                v[0].x = 0; v[0].y = 0; v[0].z = 0;
                player = this->ClosestPlayer();
                if (player != 0) {
                    int angle;
                    int *q = &player->mPos.x; /* one base pointer; see 0212423c */
                    v[1].x = q[0];
                    v[1].y = q[1];
                    v[1].z = q[2];
                    v[0].x = v[1].x - this->mHandPos.x;
                    v[0].y = v[1].y - this->mHandPos.y;
                    v[0].z = v[1].z - this->mHandPos.z;
                    angle = cstd::atan2(v[0].x, v[0].z);
                    Matrix4x3_FromRotationY(&gScratchMtx, angle);
                    Matrix4x3_ApplyInPlaceToRotationX(&gScratchMtx,
                                                      (-cstd::atan2(v[0].y, Vec3_HorzLen(&v[0]))));
                    MTX_MultVec43(&in, &gScratchMtx, &out);
                }
                out.y += 0x14000;
                target->mSpeed.x = out.x;
                target->mSpeed.y = out.y;
                target->mSpeed.z = out.z;
                target->mVertAccel = -0x2000;
                this->mSnowballID = 0;
            }
        }
    }
    if (this->mAnmModel.Finished() != 0) {
        SetState(&data_ov081_02128e14);
    }
    return 1;
}

/* State "throw", enter. */
int daSnowman_c::StateThrowSnowballEnter() {
    this->mAnmModel.SetAnim((BCA_File *)data_ov081_02128db8.filePtr, 0x40000000, 0x1000, 0);
    return 1;
}

/* State "make a snowball", execute. */
int daSnowman_c::StateSpawnSnowballExecute() {
    dActor_c *ball = dActor_c::FindWithID(this->mSnowballID);
    if (ball != 0 && ball->mVertAccel == 0) {
        RotateTowardPlayer(1);
        ball->mPos.x = this->mHandPos.x;
        ball->mPos.y = this->mHandPos.y;
        ball->mPos.z = this->mHandPos.z;
    }

    if (this->mStateTimer == 0) {
        SetState(&data_ov081_02128e44);
        return 1;
    }

    if (FindClosestPlayerDist() > 0x320000) {
        SetState(&data_ov081_02128e64);
    }
    return 1;
}

/* State "make a snowball", enter: spawns it in the hand. */
int daSnowman_c::StateSpawnSnowballEnter() {
    dActor_c *a = dActor_c::Spawn(kSnowballId, 0, this->mHandPos, 0, this->mAreaId, -1);
    if (a)
        this->mSnowballID = a->uniqueID;
    this->mStateTimer = 100;
    this->mStep = 0;
    return 1;
}

/* State "face the player", execute. */
int daSnowman_c::StateRotateExecute() {
    RotateTowardPlayer(0);
    if (this->mStateTimer == 0)
        SetState(&data_ov081_02128e34);
    if (FindClosestPlayerDist() > 0x320000)
        SetState(&data_ov081_02128e64);
    return 1;
}

/* State "face the player", enter. */
int daSnowman_c::StateRotateEnter() {
    this->mAnmModel.SetAnim((BCA_File *)gSnowmanWaitAnimationFile.filePtr, 0, 0x1000, 0);
    if (this->mType == 2)
        this->mFlags = 0x18000002;
    else
        this->mFlags = 0x10000002;
    this->mStateTimer = 20;
    return 1;
}

/* State "rise", execute. */
int daSnowman_c::StateRiseFromGroundExecute() {
    if (this->mSpeed.y < 0) {
        if (this->mHomePos.y > this->mPos.y) {
            this->mSpeed.y = 0;
            this->mVertAccel = 0;
            this->mPos.y = this->mHomePos.y;
            SetState(&data_ov081_02128e14);
        }
    }
    RotateTowardPlayer(0);
    return 1;
}

/* State "rise", enter. */
int daSnowman_c::StateRiseFromGroundEnter() {
    struct Vector3 pos;

    this->mSpeed.y = 0x3c000;
    this->mVertAccel = -0x4000;
    func_02012694(0xdc, &this->mCamSpacePos);
    pos.x = this->mHomePos.x;
    pos.y = this->mHomePos.y;
    pos.z = this->mHomePos.z;
    this->mParticleID1 =
        Particle::System::New(this->mParticleID1, kPtclSnowmanRiseA, pos.x, pos.y, pos.z, 0, 0);
    pos.y = pos.y + 0x1e000;
    this->mParticleID2 =
        Particle::System::New(this->mParticleID2, kPtclSnowmanRiseB, pos.x, pos.y, pos.z, 0, 0);
    return 1;
}

/* State "wait buried", execute: rise when a player comes near. */
int daSnowman_c::StateHideUnhideExecute() {
    if (FindClosestPlayerDist() < 0x258000)
        SetState(&data_ov081_02128ea4);
    return 1;
}

/* State "wait buried", enter. */
int daSnowman_c::StateHideUnhideEnter() {
    int v = this->mType;
    if (v != 2 && v != 3) {
        this->mFlags = 3;
    } else if (v == 2) {
        this->mFlags = 0x8000002;
    }
    this->mAnmModel.SetAnim((BCA_File *)gSnowmanWaitAnimationFile.filePtr, 0, 0x1000, 0);
    return 1;
}

/* State "hop to a node", execute. */
int daSnowman_c::StateHopExecute() {
    struct Vector3 node;
    struct Vector3 diff;
    struct Vector3 scaled;
    int len;

    if (this->mWithMeshClsn.IsOnGround()) {
        func_02012694(0xe5, &this->mCamSpacePos);
        SetState(&data_ov081_02128e54);
        this->mAnmModel.SetAnim((BCA_File *)gSnowmanJumpEdAnimationFile.filePtr, 0x40000000, 0x1000,
                                0);
        return 1;
    }

    PathPtr p;
    p.FromID(this->mPathId);
    p.GetNode(node, this->mPathNodeIndex);
    node.y = this->mPos.y;
    Vec3_Sub(&diff, &this->mPos, &node);
    len = VEC_Mag(&diff);
    ApproachAngle(&this->mPrevAngle.y, Vec3_HorzAngle(&this->mPos, &node), 1, 0x1000, 0x500);
    if (len == 0 || len <= this->mHopStep)
        return 1;
    {
        int q = FX_Div(this->mHopStep, len);
        Vec3_MulScalar(&scaled, &diff, q);
        VEC_Subtract(&this->mPos, &scaled, &this->mPos);
    }
    return 1;
}

/* State "hop to a node", enter: nothing to do. */
int daSnowman_c::StateHopEnter() {
    return 1;
}

/* State "walk the path", execute. */
int daSnowman_c::StateWalkExecute() {
    struct Vector3 node;

    if (this->mStateTimer == 0) {
        if (this->mStep == 1) {
            this->mSpeed.y = 0x14000;
            this->mVertAccel = -0x4000;
            this->mStep = 2;
            func_02012694(0xdd, &this->mCamSpacePos);
        }

        PathPtr p;
        p.FromID(this->mPathId);
        p.GetNode(node, this->mPathNodeIndex);
        ApproachAngle(&this->mPrevAngle.y, Vec3_HorzAngle(&this->mPos, &node), 1, 0x1000, 0x500);

        if (AngleDiff(this->mPrevAngle.y, Vec3_HorzAngle(&this->mPos, &node)) < 0x100 &&
            this->mWithMeshClsn.IsOnGround()) {
            int st = this->mStep;
            if (st != 0) {
                if (st == 1)
                    func_02012694(0xe5, &this->mCamSpacePos);
                this->mStep++;
                if (this->mStep < 0xb)
                    return 1;
            }
            this->mStep = 0;
            this->mSpeed.y = 0x3a000;
            this->mVertAccel = -0x4000;
            this->mAnmModel.SetAnim((BCA_File *)gSnowmanJumpStAnimationFile.filePtr, 0x40000000,
                                    0x1000, 0);
            func_02012694(0xdd, &this->mCamSpacePos);
            SetState(&data_ov081_02128e74);
        }
    }
    return 1;
}

/* State "walk the path", enter: step onto the next node when close enough. */
int daSnowman_c::StateWalkEnter() {
    struct Vector3 node;
    struct Vector3 diff;
    int len;

    PathPtr pp;
    pp.FromID(this->mPathId);
    pp.GetNode(node, this->mPathNodeIndex);
    node.y = this->mPos.y;
    Vec3_Sub(&diff, &this->mPos, &node);
    len = VEC_Mag(&diff);
    this->mHopStep = 0xa000;
    this->mStep = 0;
    if (len == 0 || len <= this->mHopStep) {
        this->mPos.x = node.x;
        this->mPos.y = node.y;
        this->mPos.z = node.z;
        this->mPathNodeIndex++;
        if (this->mPathNodeIndex >= this->mPathNodeCount)
            this->mPathNodeIndex = 0;
        this->mStep = 1;
    }
    this->mStateTimer = 10;
    this->mAnmModel.SetAnim((BCA_File *)gSnowmanWaitAnimationFile.filePtr, 0, 0x1000, 0);
    return 1;
}

/* Enters a state: records it, then runs its enter handler if it has one. */
int daSnowman_c::SetState(State *state) {
    this->mState = state;
    if (this->mState->enter == 0)
        return 1;
    return (this->*this->mState->enter)();
}

/* Poses the model, then carries the cap on bone 5 and the snowball on
   bone 3, and drops the shadow while standing on the path. */
void daSnowman_c::UpdateModelMatrices() {
    Vector3 src, dst;
    int t;
    dActor_c *actor;

    this->mAnmModel.UpdateVerts();

    src.x = this->mPos.x;
    t = this->mPos.y;
    src.y = t;
    src.z = this->mPos.z;
    src.y = t + this->mSinkOffsetY;
    Vec3_Asr(&dst, &src, 3);
    Matrix4x3_FromTranslation(&gScratchMtx, dst.x, dst.y, dst.z);

    Matrix4x3_ApplyInPlaceToRotationXYZExt(&gScratchMtx, this->mAngle.x, this->mAngle.y,
                                           this->mAngle.z);
    this->mAnmModel.mat4x3 = gScratchMtx;

    if (this->mCapUniqueID != 0) {
        this->mCapPos.x = 0;
        this->mCapPos.y = 0;
        this->mCapPos.z = 0;
        actor = dActor_c::FindWithID(this->mCapUniqueID);
        if (actor != 0) {
            MTX_Concat43(&this->mAnmModel.data.transforms[5], &this->mAnmModel.mat4x3,
                         &this->mCapMatrix);

            this->mCapPos.x = gScratchMtx.t.x;
            this->mCapPos.y = gScratchMtx.t.y;
            this->mCapPos.z = gScratchMtx.t.z;
            this->mCapPos.x <<= 3;
            this->mCapPos.y <<= 3;
            this->mCapPos.z <<= 3;
            actor->mPos.x = this->mCapPos.x;
            actor->mPos.y = this->mCapPos.y;
            actor->mPos.z = this->mCapPos.z;

            Matrix4x3_FromTranslation(&gScratchMtx, 0, 0x4000, 0);
            Matrix4x3_ApplyInPlaceToRotationXYZExt(&gScratchMtx, 0, 0x2000, 0);
            MTX_Concat43(&gScratchMtx, &this->mCapMatrix, &this->mCapMatrix);
            actor->mCarryMatrix = &this->mCapMatrix;
        }
    }

    this->mHandPos.x = 0;
    this->mHandPos.y = 0;
    this->mHandPos.z = 0;
    gScratchMtx = this->mAnmModel.mat4x3;
    MTX_Concat43(&this->mAnmModel.data.transforms[3], &gScratchMtx, &gScratchMtx);
    Matrix4x3_ApplyInPlaceToTranslation(&gScratchMtx, 0xc000, -0x1000, 0);

    this->mHandPos.x = gScratchMtx.t.x;
    this->mHandPos.y = gScratchMtx.t.y;
    this->mHandPos.z = gScratchMtx.t.z;
    this->mHandPos.x <<= 3;
    this->mHandPos.y <<= 3;
    this->mHandPos.z <<= 3;

    if (this->mType != 0)
        return;

    Matrix4x3_FromTranslation(&gScratchMtx, this->mPos.x >> 3, (this->mPos.y - 0xe000) >> 3,
                              this->mPos.z >> 3);
    this->mShadowMatrix = gScratchMtx;
    DropShadowRadHeight(this->mShadowModel, this->mShadowMatrix, 0x78000, 0xc8000, 0xf);
}

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

void daSnowman_c::OnPendingDestroy()
{
}

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

int daSnowman_c::Behavior()
{
    daPly_c *p;
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
            func_02012694(0x166, &mCamSpacePos);
            mDeathSound = 0;
        }
        UpdateModelMatrices();
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
    mAngle.x = mPrevAngle.x;
    mAngle.y = mPrevAngle.y;
    mAngle.z = mPrevAngle.z;
    DecIfAbove0_Short(&mStateTimer);
    if (mState->execute != 0)
        (this->*mState->execute)();
    UpdatePos(&mdCcAcPos_c);
    if (mType == 0)
        UpdateWMClsn(mWithMeshClsn, 0);
    UpdateModelMatrices();
    if (mState != &data_ov081_02128e84
        && mState != &data_ov081_02128e64
        && mState != &data_ov081_02128e94
        && mState != &data_ov081_02128e24)
        CheckAttacks();
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

int daSnowman_c::InitResources()
{
    Vector3 v;

    mAnmModel.SetFile(dExtSimpleModel_c::LoadFile(data_ov081_02128db0), 1, -1);
    dExtSimpleModel_c::LoadFile(data_ov081_02128d90);
    mShadowModel.InitCylinder();
    dExtFrameCtrl_c::LoadFile(gSnowmanWaitAnimationFile);
    dExtFrameCtrl_c::LoadFile(data_ov081_02128db8);
    dExtFrameCtrl_c::LoadFile(data_ov081_02128da8);
    dExtFrameCtrl_c::LoadFile(gSnowmanJumpStAnimationFile);
    dExtFrameCtrl_c::LoadFile(gSnowmanJumpEdAnimationFile);

    mPathId = param1 & 0xff;
    mType = ((s32)param1 & 0xff00) >> 8;
    if (mPathId == 0xff)
        mPathId = 0;
    if (mType == 0xff)
        mType = 0;

    mHomePos.x = mPos.x;
    mHomePos.y = mPos.y;
    mHomePos.z = mPos.z;

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

    mScale.x = 0x1000;
    mScale.y = 0x1000;
    mScale.z = 0x1000;
    mTerminalVelocity = -0xc8000;

    v.x = data_ov081_02128998.x;
    v.y = data_ov081_02128998.y;
    v.z = data_ov081_02128998.z;
    mdCcAcPos_c.Init(this, v, 0x3c000, 0x96000, 0x200004, 0x42050);

    mAngle.y = mPrevAngle.y;
    mInitAngleY = mAngle.y;
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
        SetState(&data_ov081_02128e54);
    } else {
        mPos.y -= 0x118000;
        SetState(&data_ov081_02128e84);
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

/* Slot 29. Callers add this to the actor's Y before the egg-aim particles. */
s32 daSnowman_c::OnAimedAtWithEgg() {
    return 0x98000;
}

daSnowman_c *daSnowman_c_classInit() {
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

daSnowman_c::State data_ov081_02128e54(&daSnowman_c::StateWalkEnter,
                                       &daSnowman_c::StateWalkExecute);
daSnowman_c::State data_ov081_02128e74(&daSnowman_c::StateHopEnter, &daSnowman_c::StateHopExecute);
daSnowman_c::State data_ov081_02128e84(&daSnowman_c::StateHideUnhideEnter,
                                       &daSnowman_c::StateHideUnhideExecute);
daSnowman_c::State data_ov081_02128ea4(&daSnowman_c::StateRiseFromGroundEnter,
                                       &daSnowman_c::StateRiseFromGroundExecute);
daSnowman_c::State data_ov081_02128e14(&daSnowman_c::StateRotateEnter,
                                       &daSnowman_c::StateRotateExecute);
daSnowman_c::State data_ov081_02128e34(&daSnowman_c::StateSpawnSnowballEnter,
                                       &daSnowman_c::StateSpawnSnowballExecute);
daSnowman_c::State data_ov081_02128e44(&daSnowman_c::StateThrowSnowballEnter,
                                       &daSnowman_c::StateThrowSnowballExecute);
daSnowman_c::State data_ov081_02128e64(&daSnowman_c::StateBurrowEnter,
                                       &daSnowman_c::StateBurrowExecute);
daSnowman_c::State data_ov081_02128e94(&daSnowman_c::StateDeathEnter,
                                       &daSnowman_c::StateDeathExecute);
daSnowman_c::State data_ov081_02128e24(&daSnowman_c::StateMeltEnter,
                                       &daSnowman_c::StateMeltExecute);

SnowmanCylinderOffset data_ov081_02128998 = {0, 0x1e000, 0};
daSnowmanTurnRate data_ov081_021289a4[2] = {{1, 0x1000, 0x1000}, {1, 0x1000, 0x1000}};
