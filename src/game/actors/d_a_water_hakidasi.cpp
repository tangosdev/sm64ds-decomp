//cpp
/* daWater_Hakidasi_c: a water spout that spawns WATER_RING actors (registry profile
 * WATER_HAKIDASI, ov064).
 * 12 functions, .text 0x02119330..0x02119a58. The last is the registry
 * factory daWater_Hakidasi_c_classInit (0x02119a18), `new daWater_Hakidasi_c()`.
 *
 * ROM evidence: _ZTS18daWater_Hakidasi_c is "18daWater_Hakidasi_c" at ov064
 * 0x0211c2f8; _ZTI at 0x0211c2ec reads [__si_class_type_info, that string,
 * _ZTI12dEnemyBase_c]. The vtable's address point is 0x0211c334; the word
 * before it is that _ZTI. The tree previously called the class JetStream
 * (coined; the actors it spawns are WATER_RINGs, not jets).
 *
 * The out-of-line destructor is the key function, so this TU emits _ZTV/_ZTI/
 * _ZTS. Under `#pragma defer_codegen off` it comes out D1 (0x02119330), D0
 * (0x02119368), then a D2 the cartridge has no home for (manifest: deadstrip);
 * the same pragma lays .text down in source order, so this file is
 * ROM-ascending.
 *
 * The class has one state (data_ov064_0211c934): InitState0 (0x021197fc)
 * enters it, State0 (0x021193b4) runs it every frame. SetState (0x0211982c)
 * installs it from InitResources.
 *
 * What stays:
 * - `#pragma opt_strength_reduction off` is required inside State0 and is
 *   turned back on right after it: the pragma is file-global and the later
 *   members matched without it.
 * - State0 keeps its M() launders, the volatile rot block and `m1 = -1` as
 *   found; they were needed to match.
 * - The state table is two plain copies of the ROM's pointer-to-member
 *   constants (HakPmfSlot), so InitResources casts it to State.
 * - The empty member at 0x0211987c keeps its address name: Behavior calls it
 *   where sibling actors rebuild their model matrix, but its body is empty and
 *   says nothing about its role.
 */

#include "daWater_Hakidasi_c.h"
#include "SharedFilePtr.h"
#include "Player.h"
#include "daWater_Ring_c.h"
#include "dExtSimpleModel_c.h"
#include "Sound.h"
#include "Particle__System.h"
#include "math/CalcLib.h"
#include "math/MtxStack.h"

#pragma defer_codegen off

struct Vec3 {
    int x, y, z;
};
struct Vec3_16 {
    s16 x, y, z;
};

/* The ROM member-function-pointer constants the state table copies from
 * (unlicensed .data), and the table itself (defined at the end of this
 * file). Each slot is one 8-byte handler; the table is the two slots side
 * by side. The names are TU-unique on purpose: PMF as a type name is
 * redefined differently all over the tree, and naming it in new
 * declarations would drag every one of those spellings into the decl
 * gate's scope. InitResources hands the table to SetState as a State. */
struct HakPmfSlot {
    int w[2];
};
struct HakPmfTable {
    HakPmfSlot a, b;
};
extern HakPmfSlot data_ov064_0211c2e4;
extern HakPmfSlot data_ov064_0211c2dc;
extern HakPmfTable data_ov064_0211c934;

/* local extern: MulVec3Mat4x3 takes this TU's Vec3; no shared header
 * declares SublevelToLevel or IsStarCollected. */
extern "C" {
extern void MulVec3Mat4x3(struct Vec3 *in, void *m, struct Vec3 *out);
extern int SublevelToLevel(int);
extern int IsStarCollected(int r0, int r1);
}

extern SharedFilePtr data_ov002_0210da10;
extern SharedFilePtr data_ov002_0210d9a8;
extern signed char data_0209f2f8;
extern unsigned char data_0209f220;

/* Identity, but it keeps the address expression from being folded into the access. */
#define M(p) (p)

/* D0 is the deleting destructor: it destroys through this class and its
 * bases, which is why more than one vptr store appears, then frees through an
 * inline operator delete, which is why nothing here mentions a heap. */
// @symbol _ZN18daWater_Hakidasi_cD1Ev
// @symbol _ZN18daWater_Hakidasi_cD0Ev
daWater_Hakidasi_c::~daWater_Hakidasi_c() {}

/* Per-frame update. Moves a non-metal player's position while within
 * 0x3e8000 of the spout, spawns a WATER_RING (actor 0xf4) every 0x50 frames
 * (recording its uniqueID in the mSpawnedIDs buffer), and counts the rings the
 * player passes, as reported in mPassedRing, in spawn order. At a count of 5 it
 * waits 0x1e frames, spawns a STAR (actor 0xb2) and sets mRingsPassed to 0xa. */
// @symbol _ZN18daWater_Hakidasi_c6State0Ev
int daWater_Hakidasi_c::State0() {
    struct Vec3_16 rot;
    struct Vec3 vec;
    struct Vec3 delta;
    struct Vec3 rewardPos;
    struct Vec3 hitPos1;
    struct Vec3 hitPos2;
    Player *player;
    int dist;
    daWater_Ring_c *ring;
    int i;
    dActor_c *hit;
    int uid;
    int *playerPos;
    int px, py, pz;
    int *headp;
    int *hitsp;
    int *slotp;
    int *timerp;
    int *posp;
    u16 ax, ay, az;
    s16 tx;
    int m1;

    player = ClosestPlayer();
    if (player != 0 && player->mIsMetal == 0) { /* Player::mIsMetal */
        dist = DistToCPlayer();
        if (dist < 0x3e8000) {
            dist = (0x3e8000 - dist) / 30;
            vec.x = 0;
            vec.y = 0;
            vec.z = dist;
            delta.x = 0;
            delta.y = 0;
            delta.z = 0;
            Matrix4x3_FromRotationY(&data_020a0e68, (s16)(HorzAngleToCPlayer() + 0x8000));
            Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, -0x4000);
            MulVec3Mat4x3(&vec, &data_020a0e68, &delta);
            playerPos = (int *)(int)M(&player->mPosX);
            px = playerPos[0];
            vec.x = px;
            py = playerPos[1];
            vec.y = py;
            pz = playerPos[2];
            vec.z = pz;
            {
                int newX = px + delta.x;
                vec.y = py + delta.y;
                vec.z = pz + delta.z;
                vec.x = newX;
                player->mPosX = newX;
                player->mPosY = vec.y;
                player->mPosZ = vec.z;
            }
        }
        mdCcAc_c.Clear();
        mdCcAc_c.dCc_c::Update();
        if (dist > 0x7d0000 && mRingsPassed < 5) {
            mRingsPassed = 0;
        }
    }

    if (mNoRings == 0) {
        mParticle = Particle::System::New(mParticle, 0x138, mPosX, mPosY, mPosZ, 0, 0);
        if (mRingsPassed < 5) {
            if (*(u16 *)&mStateTimer == 0) {
                ax = *(volatile u16 *)&mAngleX;
                ay = *(volatile u16 *)&mAngleY;
                {
                    volatile struct Vec3_16 *vr = &rot;
                    vr->x = ax;
                    vr->y = ay;
                    az = *(u16 *)&mAngleZ;
                    tx = (s16)vr->x;
                    m1 = -1;
                    vr->z = az;
                    tx = (s16)(tx + 0x4000);
                    vr->x = (u16)tx;
                }
                /* Registry slot 0xf4 constructs daWater_Ring_c. */
                ring = static_cast<daWater_Ring_c *>(
                    dActor_c::Spawn(0xf4, 2, mPos, (Vector3_16 *)&rot, mAreaId, -1));
                if (ring != 0) {
                    mSpawnedIDs[mSpawnedHead] = ring->uniqueID;
                    headp = (int *)(int)M(&mSpawnedHead);
                    *headp = *headp + 1;
                    if (mSpawnedHead >= 0x14) {
                        mSpawnedHead = 0;
                    }
                    ring->mSpawner = this;
                    ring->mSpeedX = 0;
                    ring->mVertSpeed = 0x5000;
                    ring->mSpeedZ = 0;
                }
                mStateTimer = 0x50;
            }
            hit = mPassedRing;
            if (hit != 0) {
                if (mRingsPassed == 0) {
                    uid = hit->uniqueID;
#pragma opt_strength_reduction off
                    for (i = 0; i < 0x14; i++) {
                        u32 slot = mSpawnedIDs[i];
                        if (slot == uid) {
                            mMatchedSlot = i;
                            hitsp = (int *)(int)M(&mRingsPassed);
                            *hitsp = *hitsp + 1;
                            func_02012790(0x25);
                            hit = mPassedRing;
                            posp = (int *)(int)M(&hit->mPosX);
                            hitPos1.x = posp[0];
                            hitPos1.y = posp[1];
                            hitPos1.z = posp[2];
                            SpawnNumber(*(Vector3 *)&hitPos1, mRingsPassed, 0, 0, 0);
                            mPassedRing = 0;
                            return 1;
                        }
                    }
                } else {
                    slotp = (int *)(int)M(&mMatchedSlot);
                    *slotp = *slotp + 1;
                    if (mMatchedSlot >= 0x14) {
                        mMatchedSlot = 0;
                    }
                    {
                        uid = mPassedRing->uniqueID;
                        i = mMatchedSlot;
                        while (1) {
                            u32 slot = mSpawnedIDs[i];
                            if (slot == uid) {
                                hitsp = (int *)(int)M(&mRingsPassed);
                                *hitsp = *hitsp + 1;
                                func_02012790(0x25);
                                hit = mPassedRing;
                                posp = (int *)(int)M(&hit->mPosX);
                                hitPos2.x = posp[0];
                                hitPos2.y = posp[1];
                                hitPos2.z = posp[2];
                                SpawnNumber(*(Vector3 *)&hitPos2, mRingsPassed, 0, 0, 0);
                                mPassedRing = 0;
                                return 1;
                            }
                            break;
                        }
                    }
                }
                mRingsPassed = 0;
                mMatchedSlot = 0;
                mPassedRing = 0;
            }
        }
    } else {
        mParticle = Particle::System::New(mParticle, 0x7b, mPosX, mPosY, mPosZ, 0, 0);
    }

    if (mRingsPassed == 5) {
        timerp = (int *)(int)M(&mRewardTimer);
        rewardPos.x = mPosX;
        rewardPos.y = mPosY;
        rewardPos.z = mPosZ;
        rewardPos.y = rewardPos.y - 0x64000;
        *timerp = *timerp + 1;
        if (mRewardTimer > 0x1e) {
            dActor_c::Spawn(0xb2, mStarID | 0x40, *(Vector3 *)&rewardPos, &mAngle, mAreaId, -1);
            mRingsPassed = 0xa;
        }
    }
    return 1;
}

#pragma opt_strength_reduction on

/* Enters the state: clears the passed ring, the ring head, the matched slot
 * and the spawned-ring buffer. */
// @symbol _ZN18daWater_Hakidasi_c10InitState0Ev
int daWater_Hakidasi_c::InitState0() {
    int i = 0;
    mPassedRing = 0;
    mSpawnedHead = i;
    mMatchedSlot = i;
    for (int v = i; i < 20; i++) {
        mSpawnedIDs[i] = v;
    }
    return 1;
}

// @symbol _ZN18daWater_Hakidasi_c8SetStateEPNS_5StateE
/* Installs a state and runs its enter handler, if it has one. */
int daWater_Hakidasi_c::SetState(State *state) {
    mState = state;
    State *s = mState;
    if (s->mInit == 0)
        return 1;
    return (this->*(s->mInit))();
}

// @symbol _ZN18daWater_Hakidasi_c19func_ov064_0211987cEv
void daWater_Hakidasi_c::func_ov064_0211987c() {}

/* Gives back the two shared files the spout renders from. Both live in ov002,
 * not in this overlay: the class borrows models the always-resident module
 * owns, so the handles are released rather than freed. */
// @symbol _ZN18daWater_Hakidasi_c16CleanupResourcesEv
s32 daWater_Hakidasi_c::CleanupResources() {
    data_ov002_0210da10.Release();
    data_ov002_0210d9a8.Release();
    return 1;
}

/* Empty: the ROM body is a single `bx lr`. The override exists to suppress
 * whatever the base does on pending destroy. */
// @symbol _ZN18daWater_Hakidasi_c16OnPendingDestroyEv
void daWater_Hakidasi_c::OnPendingDestroy() {}

/* `mov r0,#1; bx lr` and nothing else: the class draws nothing of its own
 * from the render slot. */
// @symbol _ZN18daWater_Hakidasi_c6RenderEv
s32 daWater_Hakidasi_c::Render() {
    return 1;
}

/* The state pointer at 0x300 is daWater_Hakidasi_c::State (see the header),
 * the same shape and treatment as daPropeller_Heyho_Fire_c::Behavior. */
// @symbol _ZN18daWater_Hakidasi_c8BehaviorEv
s32 daWater_Hakidasi_c::Behavior() {
    DecIfAbove0_Short((u16 *)&mStateTimer);
    State *h = mState;
    if (h->mMain != 0) {
        (this->*(h->mMain))();
    }
    mAngleX = mPrevAngleX;
    mAngleY = mPrevAngleY;
    mAngleZ = mPrevAngleZ;
    func_ov064_0211987c();
    return 1;
}

// @symbol _ZN18daWater_Hakidasi_c13InitResourcesEv
s32 daWater_Hakidasi_c::InitResources() {
    dExtSimpleModel_c::LoadFile(data_ov002_0210da10);
    dExtSimpleModel_c::LoadFile(data_ov002_0210d9a8);
    mStarID = (param1 >> 0xc) & 0xf;
    mNoRings = param1 & 1;
    if ((param1 & 0xf) > 1)
        mNoRings = 0;
    if (data_0209f2f8 == 8 && (data_0209f220 == 1 || IsStarCollected(SublevelToLevel(8), 1) == 0)) {
        return 0;
    }
    mdCcAc_c.Init(this, 0xc8000, 0x190000, 0x800004, 0);
    SetState((State *)&data_ov064_0211c934);
    return 1;
}

/* Reconstructed source-style name: SM64DS proves daWater_Hakidasi_c through
 * RTTI, allocation size, vtable identity, and the WATER_HAKIDASI registry
 * profile; later EAD lineage supplies classInit. Exact original spelling is
 * not preserved. Historical alias: JetStream_Spawn. */
// @symbol daWater_Hakidasi_c_classInit
extern "C" daWater_Hakidasi_c *daWater_Hakidasi_c_classInit() {
    return new daWater_Hakidasi_c();
}

/* Static-init global (was the handwritten __sinit_ov064_0211b4dc shard):
 * the two state handlers, plain-copied from the ROM constants in retail
 * order. */
HakPmfTable data_ov064_0211c934 = {data_ov064_0211c2e4, data_ov064_0211c2dc};
