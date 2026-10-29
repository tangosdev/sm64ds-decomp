//cpp
/* daBrq_c -- the Amp (BIRIKYU 266), ov070.
 *
 * It circles its spawn point -- radius 180.0 when param1 bit 1 is set,
 * otherwise 360.0, its direction set by param1 bit 0 -- bobbing and pulsing
 * in size, with its loop sound playing. Once its 15-frame start timer runs
 * out, a Mario who touches it is shocked (Player::Shock) and it rests for 60
 * frames (state 0) before circling again. A hit with collider flag 0x10
 * defeats it instead: it counts a mega kill, flies off tumbling and poofs on
 * landing or after 45 frames (state 2).
 *
 * daBrq_c_classInit and g_profile_BIRIKYU are reconstructed source-style
 * names (historical aliases Amp_Spawn and Amp_SpawnInfo); the private
 * state-machine spellings are inferred.
 *
 * Source is reverse ROM order: the factory first, the destructor group
 * last. InitResources is the key function; the inline destructor in the
 * header emits retail D1 then D0 plus the complete class data group.
 * common.h comes first so Matrix4x3 is the flat 12-word spelling.
 *
 * Notes:
 * - BrqMatrixWords block-copies IDENTITY_MATRIX4X3 through int[12]: a
 *   Matrix4x3 assignment scalarizes.
 * - UpdateDefeatedState calls the interworking veneer
 *   dBgCh_Actr_UpdateDiscreteNoLava_veneer (0x02038420), not the method
 *   body at 0x02037024.
 * - The data_ov070_* resource handles and the PMF state table keep linker
 *   names; their native initialization belongs to this TU.
 * - The factory's `new` odr-uses inline ~Vector3; its vague D1 is a
 *   deadstrip duplicate of arm9:0x020072c0.
 */

#include "common.h"
#include "daBrq_c.h"
#include "nitro/fx/fx.h"
#include "SpawnInfo.h"
#include "SharedFilePtr.h"
#include "math/Approach.h"
#include "Sound.h"
#include "Particle__System.h"
#include "dBgCh_Gnd.h"
#include "Player.h"

/* arm9 C helpers with no shared header. */
extern "C" {
unsigned char DecIfAbove0_Byte(unsigned char *p);
void AddVec3(Vector3 *a, Vector3 *b, Vector3 *c);
void Matrix4x3_FromRotationXYZExt(Matrix4x3 *matrix, s16 x, s16 y, s16 z);
short Vec3_HorzAngle(const Vector3 *from, const Vector3 *to);
}
extern short data_02082214[];
extern signed char data_0209f2f8[];

/* Inferred eight-byte resource-family ABI views, not recovered original
 * type names. Exact manifest aliases bind each C1/D1 to its retail helper. */
struct BrqModelResource : SharedFilePtr {
    BrqModelResource(u32 fileID);
    ~BrqModelResource();
};

struct BrqAnimationFileResource : SharedFilePtr {
    BrqAnimationFileResource(u32 fileID);
    ~BrqAnimationFileResource();
};

struct BrqTextureSequenceResource : SharedFilePtr {
    BrqTextureSequenceResource(u32 fileID);
    ~BrqTextureSequenceResource();
};

BrqModelResource data_ov070_021235fc(0x02b1);           // BIRIKYU_BMD
BrqModelResource data_ov070_02123604(0x02b3);           // BIRIKYU_ELEC_BMD
BrqAnimationFileResource data_ov070_021235f4(0x02b2);   // BIRIKYU_ELEC_BCA
BrqAnimationFileResource data_ov070_0212360c(0x02b5);   // BIRIKYU_ELEC_STOP_BCA
BrqTextureSequenceResource data_ov070_021235ec(0x02b4); // BIRIKYU_ELEC_BTP

/* The six ROM PMF descriptors stay ROM-owned; their native copies initialize
 * the cooldown, active and defeated rows in BSS. */
extern daBrq_c::StateFn data_ov070_0212320c;
extern daBrq_c::StateFn data_ov070_02123224;
extern daBrq_c::StateFn data_ov070_02123214;
extern daBrq_c::StateFn data_ov070_02123234;
extern daBrq_c::StateFn data_ov070_0212321c;
extern daBrq_c::StateFn data_ov070_0212322c;

daBrq_c::State data_ov070_02123668[3] = {
    {data_ov070_0212320c, data_ov070_02123224},
    {data_ov070_02123214, data_ov070_02123234},
    {data_ov070_0212321c, data_ov070_0212322c},
};

Vector3 data_ov070_0212365c(0, -0x28000, 0);

/* Actor ID 0xbf is PLAYER. This int-returning predicate is what makes the
 * ROM materialize 0/1 before testing it. */
static inline int IsPlayerActor(dActor_c *actor) {
    return actor->actorID == 0xbf;
}

// @symbol daBrq_c_classInit
extern "C" daBrq_c *daBrq_c_classInit() {
    return new daBrq_c();
}

extern "C" SpawnInfo<daBrq_c> g_profile_BIRIKYU = {
    daBrq_c_classInit, 0x010a, 0x007d, 0x00000003, 0, 0x00028000, 0x01000000, 0x00a28000};

extern SharedFilePtr *data_ov070_021222e0[2];
extern BTA_File data_ov070_021231f4;
extern char IDENTITY_MATRIX4X3;

struct BrqMatrixWords {
    int words[12];
};

// @symbol _ZN7daBrq_c13InitResourcesEv
int daBrq_c::InitResources() {
    BMD_File *bmd;
    bmd = (BMD_File *)dExtSimpleModel_c::LoadFile(data_ov070_021235fc);
    mAnmModel.SetFile(bmd, 1, 1);
    bmd = (BMD_File *)dExtSimpleModel_c::LoadFile(data_ov070_02123604);
    mModel.SetFile(bmd, 1, 1);

    int i;
    for (i = 0; i < 2; i++) {
        dExtFrameCtrl_c::LoadFile(*data_ov070_021222e0[i]);
    }

    BMD_File *bmd2 = (BMD_File *)data_ov070_02123604.filePtr;
    BTP_File *btp = (BTP_File *)dExtAnmTexPat_c::LoadFile(data_ov070_021235ec);
    dExtAnmTexPat_c::Prepare(*bmd2, *btp);

    bmd2 = (BMD_File *)data_ov070_02123604.filePtr;
    dExtAnmTexSRT_c::Prepare(*bmd2, data_ov070_021231f4);

    if (!mShadowModel.InitCylinder())
        return 0;

    if ((unsigned char)((param1 >> 1) & 1)) {
        SetRanges(0, 0x20d000, 0x1000000, 0xa28000);
    } else {
        SetRanges(0, 0x2c1000, 0x1000000, 0xa28000);
    }

    mdCcAcPos_c.Init(this, data_ov070_0212365c, 0x2d000, 0x50000, 0x200002, 0x8000);
    mWithMeshClsn.Init(this, 0x2d000, 0x2d000, 0, 0);

    mVertAccel = 0;
    mTerminalVelocity = 0;
    SetState(1);

    *(BrqMatrixWords *)&mMat4x3 = *(BrqMatrixWords *)&IDENTITY_MATRIX4X3;

    UpdateModelTransform();
    return 1;
}

// @symbol _ZN7daBrq_c8BehaviorEv
int daBrq_c::Behavior() {
    UpdateState();
    mCylinderOffset.y += data_ov070_0212365c.y;
    mdCcAcPos_c.SetPosRelativeToActor(mCylinderOffset);
    mdCcAcPos_c.Clear();
    mdCcAcPos_c.Update();
    UpdateModelTransform();
    return 1;
}

// @symbol _ZN7daBrq_c6RenderEv
int daBrq_c::Render() {
    mAnmModel.Render(0);

    if (mState != 0 && mState != 2) {
        mTextureSequence.Update(mModel.data);
        mTextureTransformer.Update(mModel.data);
        mModel.Render(&mScale);
    }

    return 1;
}

// @symbol _ZN7daBrq_c16OnPendingDestroyEv
void daBrq_c::OnPendingDestroy() {}

// @symbol _ZN7daBrq_c16CleanupResourcesEv
int daBrq_c::CleanupResources() {
    data_ov070_021235fc.Release();
    data_ov070_02123604.Release();

    int i = 0;
    do {
        data_ov070_021222e0[i]->Release();
        i++;
    } while (i < 2);

    data_ov070_021235ec.Release();
    return 1;
}

// @symbol _ZN7daBrq_c8SetStateEi
void daBrq_c::SetState(s32 state) {
    mStateHandlers = &data_ov070_02123668[state];
    EnterState();
}

// @symbol _ZN7daBrq_c10EnterStateEv
void daBrq_c::EnterState() {
    StateFn *handler = &mStateHandlers->enter;
    (this->**handler)();
}

// @symbol _ZN7daBrq_c11UpdateStateEv
void daBrq_c::UpdateState() {
    StateFn *handler = &mStateHandlers->update;
    (this->**handler)();
}

// @symbol _ZN7daBrq_c18EnterCooldownStateEv
s32 daBrq_c::EnterCooldownState() {
    mAnmModel.SetAnim((BCA_File *)data_ov070_0212360c.filePtr, 0x40000000, 0x1000, 0);
    mStateTimer = 0x3c;
    mState = 0;
    return 1;
}

// @symbol _ZN7daBrq_c19UpdateCooldownStateEv
s32 daBrq_c::UpdateCooldownState() {
    mAnmModel.Advance();
    unsigned char timeLeft = DecIfAbove0_Byte(&mStateTimer);
    if (timeLeft == 0) {
        SetState(1);
    }
    return 1;
}

// @symbol _ZN7daBrq_c16EnterActiveStateEv
s32 daBrq_c::EnterActiveState() {
    mAnmModel.SetAnim((BCA_File *)data_ov070_021235f4.filePtr, 0, 0x1000, 0);
    mAnmModel.speed = 0x1000;
    mTextureSequence.SetFile(*(BTP_File *)data_ov070_021235ec.filePtr, 0, 0x1000, 0);
    mTextureSequence.speed = 0x1000;
    mTextureTransformer.SetFile(data_ov070_021231f4, 0, 0x1000, 0);
    mTextureTransformer.speed = 0x2000;
    mStateTimer = 0xf;
    mTurnSpeed = 0;
    mSpinAngle = 0;
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    mState = 1;
    return 1;
}

// @symbol _ZN7daBrq_c17UpdateActiveStateEv
s32 daBrq_c::UpdateActiveState() {
    ApproachLinear(mTurnSpeed, 1000, 20);

    unsigned char bit0 = (unsigned char)(param1 & 1);
    if (bit0) {
        mPrevAngleY = (short)(mPrevAngleY + mTurnSpeed);
    } else {
        mPrevAngleY = (short)(mPrevAngleY - mTurnSpeed);
    }

    mOrbitAngle = (short)(mOrbitAngle + 0xa00);

    unsigned char bit1 = (unsigned char)((param1 >> 1) & 1);
    int orbitRadius = bit1 ? 0xb4000 : 0x168000;

    mOrbitCenter.x = mPosX;
    mOrbitCenter.y = mPosY;
    mOrbitCenter.z = mPosZ;

    {
        int idx = ((unsigned short)mPrevAngleY) >> 4;
        int sinv = data_02082214[idx * 2];
        mCylinderOffset.x = FX_MUL(orbitRadius, sinv);
    }
    {
        int idx = ((unsigned short)mOrbitAngle) >> 4;
        int sinv = data_02082214[idx * 2];
        mCylinderOffset.y = FX_MUL(0x14000, sinv);
    }
    {
        int idx = ((unsigned short)mPrevAngleY) >> 4;
        int cosv = data_02082214[idx * 2 + 1];
        mCylinderOffset.z = FX_MUL(orbitRadius, cosv);
    }

    AddVec3(&mOrbitCenter, &mCylinderOffset, &mOrbitCenter);

    mSpinAngle = (short)(mSpinAngle + 0x4000);

    {
        int idx = ((unsigned short)mSpinAngle) >> 4;
        int sinv = data_02082214[idx * 2];
        int scale = FX_MUL(0x555, sinv) + 0x800;
        mScaleX = scale;
        mScaleY = scale;
        mScaleZ = scale;
    }

    mAnmModel.Advance();
    mTextureSequence.Advance();
    mTextureTransformer.Advance();

    mSoundHandle = Sound::PlayLong(mSoundHandle, 3, 0x183, mCamSpacePos, 0);

    if (DecIfAbove0_Byte(&mStateTimer) == 0) {
        HandlePlayerCollision();
    }
    return 1;
}

// @symbol _ZN7daBrq_c18EnterDefeatedStateEv
s32 daBrq_c::EnterDefeatedState() {
    Sound::PlayBank0(9, mCamSpacePos);
    mFlags &= ~1;
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    mPosX = mOrbitCenter.x;
    mPosY = mOrbitCenter.y;
    mPosZ = mOrbitCenter.z;
    mHorzSpeed = 0xa000;
    mVertSpeed = 0x28000;
    mStateTimer = 0x2d;
    mAnmModel.SetAnim((BCA_File *)data_ov070_0212360c.filePtr, 0x40000000, 0x1000, 0);
    Particle::System::NewSimple(0x43, mPosX, mPosY, mPosZ);
    Particle::System::NewSimple(0x44, mPosX, mPosY, mPosZ);
    mState = 2;
    return 1;
}

/* The ROM calls the interworking veneer at 0x02038420, not the direct
 * dBgCh_Actr::UpdateDiscreteNoLava body at 0x02037024. */

// @symbol _ZN7daBrq_c19UpdateDefeatedStateEv
s32 daBrq_c::UpdateDefeatedState() {
    mAngleX = (s16)(mAngleX - 0x1000);
    mAnmModel.Advance();
    UpdatePos(&mdCcAcPos_c);
    dBgCh_Actr_UpdateDiscreteNoLava_veneer(&mWithMeshClsn);

    if (mWithMeshClsn.JustHitGround() != 0 || DecIfAbove0_Byte(&mStateTimer) == 0) {
        PoofDust();
        MarkForDestruction();
    }
    return 1;
}

// @symbol _ZN7daBrq_c20UpdateModelTransformEv
void daBrq_c::UpdateModelTransform() {
    int shadowDepth;

    if (mState == 2) {
        Matrix4x3_FromRotationXYZExt(&mAnmModel.mat4x3, mAngleX, mAngleY, mAngleZ);
        mAnmModel.mat4x3.m[9] = mPosX >> 3;
        mAnmModel.mat4x3.m[10] = mPosY >> 3;
        mAnmModel.mat4x3.m[11] = mPosZ >> 3;
        mMat4x3.m[9] = mPosX >> 3;
        mMat4x3.m[10] = mPosY >> 3;
        mMat4x3.m[11] = mPosZ >> 3;
    } else {
        mAnmModel.mat4x3.m[9] = mOrbitCenter.x >> 3;
        mAnmModel.mat4x3.m[10] = mOrbitCenter.y >> 3;
        mAnmModel.mat4x3.m[11] = mOrbitCenter.z >> 3;
        mModel.mat4x3 = mAnmModel.mat4x3;
        mMat4x3.m[9] = mOrbitCenter.x >> 3;
        mMat4x3.m[10] = mOrbitCenter.y >> 3;
        mMat4x3.m[11] = mOrbitCenter.z >> 3;
    }

    shadowDepth = 0x1f4000;
    if (data_0209f2f8[0] == 0x11) {
        Vector3 pos;
        int y;
        pos.x = mPosX;
        y = mPosY;
        pos.y = y;
        pos.z = mPosZ;
        pos.y = y - 0xa000;
        dBgCh_Gnd ground;
        ground.SetObjAndPos(pos, 0);
        if (ground.DetectClsn() != 0) {
            shadowDepth = (mPosY - ground.clsnY) + 0x28000;
        }
    }

    DropShadowRadHeight(mShadowModel, mMat4x3, 0x5a000, shadowDepth, 0xf);
}

// @symbol _ZN7daBrq_c21HandlePlayerCollisionEv
s32 daBrq_c::HandlePlayerCollision() {
    u32 id = mdCcAcPos_c.otherOwner;
    if (id == 0)
        return 0;

    dActor_c *other = dActor_c::FindWithID(id);
    if (other == 0 || !IsPlayerActor(other))
        return 0;

    Player *player = (Player *)other;
    if (player->mIsVanish != 0)
        return 0;

    if (mdCcAcPos_c.hitFlags & 0x10) {
        mPrevAngleY = Vec3_HorzAngle(&player->mPos, &mPos);
        mAngleY = (short)(mPrevAngleY + 0x8000);
        player->IncMegaKillCount();
        SetState(2);
    } else {
        if (player->Shock(1) != 0)
            SetState(0);
    }
    return 1;
}
// @symbol _ZN7daBrq_cD1Ev
// @symbol _ZN7daBrq_cD0Ev
/* The inline destructor and InitResources key function emit the retail D1/D0
 * group without a retained D2 or forcing helper. */
