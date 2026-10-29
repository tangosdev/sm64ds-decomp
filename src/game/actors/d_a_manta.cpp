//cpp
/* daManta_c -- Jolly Roger Bay manta (MANTA 226).
 *
 * _ZTS at ov090 0x021341f4 names daManta_c. Base is dEnemyBase_c.
 * Swims the path in param1's low byte. Every 39 frames it drops a
 * WATER_RING (244) on bone 3 and remembers that ring's uniqueID.
 * Rings taken in that order play the collection jingle. Five of them,
 * then 30 frames, spawn STAR (178) with the star index from param1
 * bits 12..15. The cylinder on data_ov090_02134200 hurts PLAYER (191).
 *
 * daManta_c_classInit (historical alias MantaRay_Spawn) is
 * `new daManta_c()`. defer_codegen off, above the includes, lays
 * .text down in ROM order, so the factory follows InitResources.
 * g_profile_MANTA stays in its own file.
 *
 * What stays, and why:
 * - ExecuteSwim compares mRingIDs[mRingRead] with the hit
 *   ring's uniqueID in cartridge order (cmp r0, r1). Swapping the
 *   operands DIFFs. The 0x3ac/+4 dance stays; uniqueID is the word
 *   at +4 of that ring.
 * - Behavior keeps the mSpeedZ copy after the terminal clamp.
 *   Dropping it loses the reload of dActor_c's z speed.
 * - Matrix4x3 stays decl_common.h's flat s32[12].
 *   UpdateModelAndRingPos indexes .m. common.h must not replace it.
 * - Player::Hurt stays mangled (Fix12 by value).
 */

#pragma defer_codegen off

/* decl_common.h first: Matrix4x3 must stay the flat s32[12] spelling.
 * UpdateModelAndRingPos indexes .m. */
#include "decl_common.h"
#include "daManta_c.h"
#include "daWater_Ring_c.h"
#include "PathPtr.h"
#include "Player.h"
#include "SharedFilePtr.h"
#include "Sound.h"
#include "math/Approach.h"
#include "math/CalcLib.h"
#include "math/MtxStack.h"

/* init at +0, execute at +8. Same shape as daWater_Ring_c::State.
 * The overlay image stores each as (function, this-delta) with delta 0. */
struct MantaState {
    int (daManta_c::*init)();
    int (daManta_c::*execute)();
};

/* Local 8-byte resource handles: Construct stores the file ID and clears
 * numRefs/filePtr; the loaders populate the BMD/BCA payload at +4.
 * The registered destructors record file IDs; actor cleanup releases files. */
struct MantaModelFilePtr : SharedFilePtr {
    MantaModelFilePtr(u32 fileID);
    ~MantaModelFilePtr();
};

struct MantaAnimationFilePtr : SharedFilePtr {
    MantaAnimationFilePtr(u32 fileID);
    ~MantaAnimationFilePtr();
};

enum {
    /* Runtime asset handles: data/enemy/manta/manta.bmd and manta_swim.bca. */
    MANTA_MODEL_FILE = 0x39d,
    MANTA_SWIM_ANIMATION_FILE = 0x39e,
    MANTA_STAR = 0xb2,
    MANTA_PLAYER = 0xbf,
    MANTA_WATER_RING = 0xf4,
    MANTA_RING_GOAL = 5,
    MANTA_RING_SLOTS = 0x14,
    MANTA_RING_INTERVAL = 0x27,
    MANTA_STAR_WAIT = 0x1e,
    MANTA_STAR_SPAWNED = 0xa,
    MANTA_COLLECT_JINGLE = 0x25,
    MANTA_NODE_REACH = 0x258000,
    MANTA_FORWARD = 0xa000,
    MANTA_STAR_FLAG = 0x40
};

extern MantaModelFilePtr data_ov090_02134524;
extern SharedFilePtr data_ov002_0210da10;
extern SharedFilePtr data_ov002_0210d9a8;
extern MantaAnimationFilePtr data_ov090_0213452c;
extern unsigned char data_0209f2d8;

/* local extern: arm9 vector helpers with no shared header yet. */
extern "C" {
void Vec3_Asr(Vector3 *d, Vector3 *s, int sh);
void Vec3_Sub(Vector3 *out, Vector3 *a, Vector3 *b);
int LenVec3(Vector3 *v);
short Vec3_VertAngle(const Vector3 *v0, const Vector3 *v1);
void MulVec3Mat4x3(Vector3 *v, void *m, Vector3 *out);
}

extern MantaState data_ov090_0213454c;

/* One written destructor. The compiler emits D1 then D0 from the
 * members above; D0 adds dEnemyBase_c's inline operator delete. */
// @symbol _ZN9daManta_cD1Ev
// @symbol _ZN9daManta_cD0Ev
daManta_c::~daManta_c() {}

/* Cylinder follows data_ov090_02134200. A PLAYER inside it is hurt
 * from this actor's position. */
// @symbol _ZN9daManta_c18HurtTouchingPlayerEv
void daManta_c::HurtTouchingPlayer() {
    Vector3 offset;
    offset.x = data_ov090_02134200.x;
    offset.y = data_ov090_02134200.y;
    offset.z = data_ov090_02134200.z;
    mdCcAcPos_c.SetPosRelativeToActor(offset);
    unsigned int id = mdCcAcPos_c.otherOwner;
    if (id == 0)
        return;
    dActor_c *actor = dActor_c::FindWithID(id);
    int isPlayer = (int)(actor->actorID == MANTA_PLAYER);
    if (isPlayer == 0)
        return;
    {
        Vector3 hit;
        hit.x = mPosX;
        hit.y = mPosY;
        hit.z = mPosZ;
        ((Player *)actor)->Hurt(hit, 1, 0xc000, 1, 0, 1);
    }
}

/* State execute. Advance the swim anim, hurt, drop the next ring,
 * score an in-order pass, and spawn the star once five have landed. */
// @symbol _ZN9daManta_c11ExecuteSwimEv
int daManta_c::ExecuteSwim() {
    dActor_c *spawned;
    Vector3 num1;
    Vector3 num2;

    mAnmModel.Advance();
    HurtTouchingPlayer();

    if (mRingCount < MANTA_RING_GOAL) {
        if (*(unsigned short *)&mStateTimer == 0) {
            spawned = dActor_c::Spawn(MANTA_WATER_RING, 1, mRingPos, &mAngle, mAreaId, -1);
            if (spawned != 0) {
                mRingIDs[mRingWrite] = spawned->uniqueID;
                mRingWrite += 1;
                if (mRingWrite >= MANTA_RING_SLOTS)
                    mRingWrite = 0;
                ((daWater_Ring_c *)spawned)->mSpawner = this;
            }
            mStateTimer = MANTA_RING_INTERVAL;
        }

        if (mHitRing != 0) {
            if (mRingCount == 0) {
                int i;
                int key = mHitRing->uniqueID;
                for (i = 0; i < MANTA_RING_SLOTS; i++) {
                    int slot = mRingIDs[i];
                    if (slot == key) {
                        mRingRead = i;
                        mRingCount += 1;
                        func_02012790(MANTA_COLLECT_JINGLE);
                        num1 = mHitRing->mPos;
                        SpawnNumber(num1, mRingCount, 0, 0, 0);
                        mHitRing = 0;
                        return 1;
                    }
                }
            } else {
                mRingRead += 1;
                if (mRingRead >= MANTA_RING_SLOTS)
                    mRingRead = 0;
                /* Named mRingIDs[mRingRead] == uniqueID is cmp r1, r0 either
                 * operand order. The cartridge is cmp r0, r1. 0x3ac is mRingIDs;
                 * spelling the slot as &mRingIDs[i] differs by four words. */
                int slotAddr = mRingRead;
                int ringId = (int)mHitRing;
                slotAddr = (int)((char *)this + (slotAddr << 2));
                ringId = ((dActor_c *)ringId)->uniqueID;
                slotAddr = *(int *)(slotAddr + 0x3ac);
                if (slotAddr == ringId) {
                    mRingCount += 1;
                    func_02012790(MANTA_COLLECT_JINGLE);
                    num2 = mHitRing->mPos;
                    SpawnNumber(num2, mRingCount, 0, 0, 0);
                    mHitRing = 0;
                    return 1;
                }
            }
            mRingCount = 0;
            mRingRead = 0;
            mHitRing = 0;
        }
    }

    if (mRingCount == MANTA_RING_GOAL) {
        mStarDelay += 1;
        if (mStarDelay > MANTA_STAR_WAIT) {
            dActor_c::Spawn(MANTA_STAR, mStarID | MANTA_STAR_FLAG, mPos, &mAngle, mAreaId, -1);
            mRingCount = MANTA_STAR_SPAWNED;
        }
    }
    return 1;
}

/* State init. Swim anim at 1.0, empty ring list. */
// @symbol _ZN9daManta_c9EnterSwimEv
int daManta_c::EnterSwim() {
    mAnmModel.SetAnim((BCA_File *)data_ov090_0213452c.filePtr, 0, 0x1000, 0);
    mHitRing = 0;
    mRingWrite = 0;
    mRingRead = 0;
    mAnmModel.speed = 0x1000;
    int i;
    for (i = 0; i < MANTA_RING_SLOTS; i++)
        mRingIDs[i] = 0;
    return 1;
}

/* Install state and run its init PMF. A null init is "do nothing". */
// @symbol _ZN9daManta_c11ChangeStateEP10MantaState
int daManta_c::ChangeState(MantaState *state) {
    mState = state;
    MantaState *current = mState;
    if (current->init == 0)
        return 1;
    return (this->*(current->init))();
}

/* Position >> 3 and the angle triple become the model matrix. Bone 3's
 * transform is then folded in, and its translation << 3 is where the
 * next ring spawns. */
// @symbol _ZN9daManta_c21UpdateModelAndRingPosEv
void daManta_c::UpdateModelAndRingPos() {
    Vector3 scaled;
    Vec3_Asr(&scaled, &mPos, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, scaled.x, scaled.y, scaled.z);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68, mAngleX, mAngleY, mAngleZ);
    mAnmModel.mat4x3 = data_020a0e68;
    mRingPos.x = 0;
    mRingPos.y = 0;
    mRingPos.z = 0;
    data_020a0e68 = mAnmModel.mat4x3;
    MulMat4x3Mat4x3((const int *)(mAnmModel.data.transforms + 3), data_020a0e68.m, data_020a0e68.m);
    mRingPos.x = data_020a0e68.m[9];
    mRingPos.y = data_020a0e68.m[10];
    mRingPos.z = data_020a0e68.m[11];
    mRingPos.x <<= 3;
    mRingPos.y <<= 3;
    mRingPos.z <<= 3;
}

// @symbol _ZN9daManta_c16CleanupResourcesEv
int daManta_c::CleanupResources() {
    data_ov090_02134524.Release();
    data_ov002_0210da10.Release();
    data_ov002_0210d9a8.Release();
    data_ov090_0213452c.Release();
    return 1;
}

// @symbol _ZN9daManta_c16OnPendingDestroyEv
void daManta_c::OnPendingDestroy() {}

// @symbol _ZN9daManta_c6RenderEv
int daManta_c::Render() {
    mAnmModel.Render(0);
    return 1;
}

// @symbol _ZN9daManta_c8BehaviorEv
int daManta_c::Behavior() {
    DecIfAbove0_Short((unsigned short *)&mStateTimer);
    {
        MantaState *state = mState;
        if (state->execute != 0)
            (this->*(state->execute))();
    }
    {
        PathPtr path;
        Vector3 node;
        Vector3 diff;
        Vector3 forward;
        int len;

        path.FromID(*(unsigned int *)&mPathID);
        path.GetNode(node, *(unsigned int *)&mPathNode);
        Vec3_Sub(&diff, &mPos, &node);
        len = LenVec3(&diff);
        if (len == 0 || len <= MANTA_NODE_REACH) {
            mPathNode++;
            if (mPathNode >= mNumNodes)
                mPathNode = 0;
        }
        ApproachLinear(mPrevAngleY, Vec3_HorzAngle(&mPos, &node), 0x60);
        ApproachLinear(mPrevAngleX, Vec3_VertAngle(&mPos, &node), 0x40);
        mAngleX = mPrevAngleX;
        mAngleY = mPrevAngleY;
        mAngleZ = mPrevAngleZ;
        forward.y = forward.x = forward.z = 0;
        forward.z = MANTA_FORWARD;
        Matrix4x3_FromRotationY(&data_020a0e68, mPrevAngleY);
        Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, mPrevAngleX);
        MulVec3Mat4x3(&forward, &data_020a0e68, (Vector3 *)&mSpeedX);
    }
    {
        int speed = mVertSpeed + mVertAccel;
        int clamped = mTerminalVelocity;
        int keep0ac = mSpeedZ;
        if (speed >= clamped)
            clamped = speed;
        mVertSpeed = clamped;
        mSpeedZ = keep0ac;
    }
    UpdatePosWithOnlySpeed(&mdCcAcPos_c);
    UpdateModelAndRingPos();
    mdCcAcPos_c.Clear();
    mdCcAcPos_c.Update();
    return 1;
}

// @symbol _ZN9daManta_c13InitResourcesEv
int daManta_c::InitResources() {
    mAnmModel.SetFile((BMD_File *)dExtSimpleModel_c::LoadFile(data_ov090_02134524), 1, -1);
    dExtSimpleModel_c::LoadFile(data_ov002_0210da10);
    dExtSimpleModel_c::LoadFile(data_ov002_0210d9a8);
    dExtFrameCtrl_c::LoadFile(data_ov090_0213452c);

    mPathID = param1 & 0xff;
    mStarID = (param1 >> 12) & 0xf;
    if (mPathID < 0)
        mPathID = 0;

    {
        PathPtr path;
        path.FromID(*(unsigned int *)&mPathID);
        mNumNodes = path.NumNodes();
    }

    mTerminalVelocity = -0x3c000;

    {
        Vector3 offset;
        offset.x = data_ov090_02134200.x;
        offset.y = data_ov090_02134200.y;
        offset.z = data_ov090_02134200.z;
        mdCcAcPos_c.Init(this, offset, 0x150000, 0xc8000, 0x200004, 0);
    }

    {
        PathPtr path;
        path.FromID(*(unsigned int *)&mPathID);
        mPathNode = 1;
        path.GetNode(mPos, *(unsigned int *)&mPathNode);
    }

    {
        /* ContinueKuppaScriptIfNecessary stores 2 here. That visit snaps
         * the manta onto node 3 at a fixed pose and clears mFlags. */
        int kuppa = (int)(data_0209f2d8 == 2);
        if (kuppa != 0) {
            mPathNode = 3;
            mPrevAngleX = (short)0xf303;
            mPrevAngleY = 0xb50;
            mPrevAngleZ = 0;
            mPosX = (int)0xfdfb8000;
            mPosY = (int)0xff8f8000;
            mPosZ = 0x29a000;
            mFlags = 0;
        }
    }

    ChangeState((MantaState *)&data_ov090_0213454c);
    return 1;
}

// @symbol daManta_c_classInit
extern "C" daManta_c *daManta_c_classInit() {
    return new daManta_c();
}

/* dExtSimpleModel_c, anim, then the state record. The record's pointer-to-member
 * descriptors stay anonymous. This is what emits __sinit_d_a_manta.cpp. */
MantaModelFilePtr data_ov090_02134524(MANTA_MODEL_FILE);
MantaAnimationFilePtr data_ov090_0213452c(MANTA_SWIM_ANIMATION_FILE);
MantaState data_ov090_0213454c = {
    &daManta_c::EnterSwim,
    &daManta_c::ExecuteSwim,
};
