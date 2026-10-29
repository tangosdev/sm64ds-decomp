//cpp
/* Production translation unit for ov064/daWater_Ring_c.
 * 15 function(s), .text 0x02119a58..0x0211a200. The last is the registry
 * factory daWater_Ring_c_classInit (0x0211a1b0), `new daWater_Ring_c()`.
 * One out-of-line destructor emits D1 then D0;
 * `#pragma defer_codegen off` lays .text down in source order.
 *
 * Two states, each a daWater_Ring_c::State of member-function pointers:
 * active (InitStateActive / StateActive, data_ov064_0211c954), installed by
 * InitResources, and passed (InitStatePassed / StatePassed,
 * data_ov064_0211c944), installed by CheckPlayerPass once the player swims
 * through. SetState installs a state; UpdateModel poses the model each frame.
 *
 * What stays:
 * - The data_ov064_* tables and constants keep address names; the profile
 *   rows and .data PMF constants are not owned by this TU.
 */

#pragma defer_codegen off

#include "common.h"
#include "daWater_Ring_c.h"
#include "SharedFilePtr.h"
#include "dExtAnmTexSRT_c.h"
#include "Player.h"
#include "math/Approach.h"
#include "math/CalcLib.h"
#include "math/MtxStack.h"
#include "daManta_c.h"
#include "daWater_Hakidasi_c.h"

/* The two state tables, ov064 .bss; the definitions at the end of this
   file fill them from .data rows holding these methods' addresses. Each
   table is one State (init + execute); the descriptors below are the
   single-handler constants it copies. */
extern daWater_Ring_c::State data_ov064_0211c944; /* passed */
extern daWater_Ring_c::State data_ov064_0211c954; /* active */
extern int (daWater_Ring_c::*data_ov064_0211c3b0)();
extern int (daWater_Ring_c::*data_ov064_0211c3c8)();
extern int (daWater_Ring_c::*data_ov064_0211c3c0)();
extern int (daWater_Ring_c::*data_ov064_0211c3b8)();
extern Vector3 data_ov064_0211c3d0; /* the collider offset */
extern s16 data_02082214[];         /* sine/cosine table */
extern char data_ov002_0210d6dc[];  /* the ring's BTA */

/* local extern: no shared header declares these. */
extern "C" {
s16 Vec3_VertAngle(const Vector3 *v1, const Vector3 *v0);
int AngleDiff(int a, int b);
void Vec3_Asr(struct Vector3 *d, struct Vector3 *s, int sh);
}

extern SharedFilePtr data_ov002_0210da10; /* the ring's BMD */

// @symbol _ZN14daWater_Ring_cD1Ev
// @symbol _ZN14daWater_Ring_cD0Ev
daWater_Ring_c::~daWater_Ring_c() {}

/* Checks whether the player swam through the ring this frame. */
// @symbol _ZN14daWater_Ring_c15CheckPlayerPassEv
void daWater_Ring_c::CheckPlayerPass() {
    Vector3 otherPos;
    Vector3 offset;
    dActor_c *other;
    int isPlayer;
    u32 id;

    mTextureTransformer.speed = 0x1000;
    offset = data_ov064_0211c3d0;
    mdCcAcPos_c.SetPosRelativeToActor(offset);
    id = mdCcAcPos_c.otherOwner;
    if (id == 0)
        return;
    other = dActor_c::FindWithID(id);
    if (other == 0)
        return;
    isPlayer = (other->actorID == 0xbf);
    if (isPlayer == 0)
        return;
    {
        const Vector3 &pos = other->mPos;
        otherPos.x = pos.x;
        otherPos.y = pos.y;
        otherPos.z = pos.z;
    }
    if (AngleDiff(mAngleX, Vec3_VertAngle(&mPos, &otherPos)) >= 0x3000)
        return;
    if ((mRingType == 1 && ((mPrevPlayerAngle >> 16) & 1) != ((HorzAngleToCPlayer() >> 16) & 1)) ||
        mRingType != 1) {
        if (mRingType == 1)
            static_cast<Player *>(other)->Heal(0x100);
        mTextureTransformer.speed = 0x4000;
        SetState(&data_ov064_0211c944);
    } else {
        mPrevPlayerAngle = HorzAngleToCPlayer();
    }
}

/* Fading out: grows and fades until it is gone. */
// @symbol _ZN14daWater_Ring_c11StatePassedEv
int daWater_Ring_c::StatePassed() {
    mOpacity -= 2;
    if (mOpacity < 2)
        mOpacity = 2;
    ApproachLinear(mScaleX, 0x3000, 0x199);
    mScaleZ = mScaleX;
    mScaleY = mScaleZ;
    if ((u16)mStateTimer == 0 || mScaleX >= 0x2ffd)
        MarkForDestruction();
    return 1;
}

/* Init of the passed state: tells the spawner which ring was hit. */
// @symbol _ZN14daWater_Ring_c15InitStatePassedEv
int daWater_Ring_c::InitStatePassed() {
    mOpacity = 0x1f;
    mStateTimer = 0x64;
    int type = mRingType;
    if (type == 1 || type == 2) {
        dActor_c *spawner = mSpawner;
        if (spawner != 0) {
            /* The spawner's passed-ring slot: type 1 rings come from
               daManta_c, type 2 from daWater_Hakidasi_c. */
            if (type == 1) {
                static_cast<daManta_c *>(spawner)->mHitRing = this;
            } else {
                static_cast<daWater_Hakidasi_c *>(spawner)->mPassedRing = this;
            }
        }
    }
    return 1;
}

/* Main of the active state: wobbles, spins and waits for the player. */
// @symbol _ZN14daWater_Ring_c11StateActiveEv
int daWater_Ring_c::StateActive() {
    int spd;
    CheckPlayerPass();
    if ((u16)mStateTimer < 0x1e) {
        if (mOpacity >= 2)
            mOpacity--;
    }
    spd = (mRingType == 1) ? 0x333 : 0x199;
    mWobblePhase += 0x1000;
    {
        s16 phase = mWobblePhase;
        int idx = ((u16)phase >> 4) * 2;
        int wobble = (int)((((s64)data_02082214[idx] << 10) + 0x800) >> 12);
        int target = wobble + mScaleBase;
        ApproachLinear(mScaleBase, 0x2000, spd);
        ApproachLinear(mScaleX, target, spd);
    }
    mSpinPhase += 0x800;
    {
        s16 phase = mSpinPhase;
        int idx = ((u16)phase >> 4) * 2;
        mPrevAngleX += (int)((((s64)data_02082214[idx] << 8) + 0x800) >> 12);
    }
    mScaleZ = mScaleX;
    mScaleY = mScaleZ;
    if ((u16)mStateTimer == 0 || mOpacity <= 1)
        MarkForDestruction();
    return 1;
}

/* Init of the active state. */
// @symbol _ZN14daWater_Ring_c15InitStateActiveEv
s32 daWater_Ring_c::InitStateActive() {
    mStateTimer = 0xc8;
    s32 angle = HorzAngleToCPlayer();
    mPrevPlayerAngle = (s16)angle;
    return 1;
}

/* Installs a state and runs its init. */
// @symbol _ZN14daWater_Ring_c8SetStateEPKNS_5StateE
int daWater_Ring_c::SetState(const State *state) {
    mState = state;
    const State *installed = mState;
    if (installed->init == 0)
        return 1;
    return (this->*installed->init)();
}

/* Poses the model: position, rotation and opacity. */
// @symbol _ZN14daWater_Ring_c11UpdateModelEv
void daWater_Ring_c::UpdateModel() {
    Vector3 v;
    Vec3_Asr(&v, &mPos, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, v.x, v.y, v.z);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68, mAngleX, mAngleY, mAngleZ);
    mModel.ApplyOpacity(mOpacity, 1);
    mModel.mat4x3 = data_020a0e68;
}

// @symbol _ZN14daWater_Ring_c16CleanupResourcesEv
int daWater_Ring_c::CleanupResources() {
    data_ov002_0210da10.Release();
    return 1;
}

// @symbol _ZN14daWater_Ring_c16OnPendingDestroyEv
void daWater_Ring_c::OnPendingDestroy() {}

// @symbol _ZN14daWater_Ring_c6RenderEv
int daWater_Ring_c::Render() {
    mTextureTransformer.Update(mModel.data);
    mModel.Render(&mScale);
    return 1;
}

// @symbol _ZN14daWater_Ring_c8BehaviorEv
int daWater_Ring_c::Behavior() {
    DecIfAbove0_Short((unsigned short *)&mStateTimer);
    if (mState->execute)
        (this->*mState->execute)();
    UpdatePos(&mdCcAcPos_c);
    mAngleX = mPrevAngleX;
    mAngleY = mPrevAngleY;
    mAngleZ = mPrevAngleZ;
    UpdateModel();
    mdCcAcPos_c.Clear();
    mdCcAcPos_c.Update();
    mTextureTransformer.Advance();
    return 1;
}

// @symbol _ZN14daWater_Ring_c13InitResourcesEv
int daWater_Ring_c::InitResources() {
    BMD_File *f = (BMD_File *)dExtSimpleModel_c::LoadFile(data_ov002_0210da10);
    if (mModel.SetFile(f, 1, -1) == 0)
        return 0;

    mRingType = param1 & 0xff;
    if (mRingType > 2 || mRingType == 0xff) {
        mRingType = 0;
    }

    dExtAnmTexSRT_c::Prepare(*(BMD_File *)data_ov002_0210da10.filePtr,
                             *(BTA_File *)data_ov002_0210d6dc);
    mTextureTransformer.SetFile(*(BTA_File *)data_ov002_0210d6dc, 0, 0x1000, 0);

    mTextureTransformer.speed = 0x1000;
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;

    Vector3 v;
    v.x = data_ov064_0211c3d0.x;
    v.y = data_ov064_0211c3d0.y;
    v.z = data_ov064_0211c3d0.z;
    mdCcAcPos_c.Init(this, v, 0x88000, 0xe8000, 0x800006, 0);

    mOpacity = 0x1f;
    SetState(&data_ov064_0211c954);
    return 1;
}

/* Reconstructed source-style name: SM64DS proves daWater_Ring_c through
 * RTTI, allocation size, vtable identity, and the WATER_RING registry
 * profile; later EAD lineage supplies classInit. Exact original spelling is
 * not preserved. Historical alias: WaterRing_Spawn. */
// @symbol daWater_Ring_c_classInit
extern "C" daWater_Ring_c *daWater_Ring_c_classInit() {
    return new daWater_Ring_c();
}

/* Static-init globals (was the handwritten __sinit_ov064_0211b518 shard):
 * the two state tables, plain-copied from the ROM constants in retail
 * order. */
daWater_Ring_c::State data_ov064_0211c954 = {data_ov064_0211c3b0, data_ov064_0211c3c8};
daWater_Ring_c::State data_ov064_0211c944 = {data_ov064_0211c3c0, data_ov064_0211c3b8};
