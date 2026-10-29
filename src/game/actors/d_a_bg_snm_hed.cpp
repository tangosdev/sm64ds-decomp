//cpp
/* daBgSnmHed_c -- the snowman's head (BIG_SNOWMAN_HEAD), ov072
 * 0x0211ff34..0x02120824. _ZTS12daBgSnmHed_c is at 0x02122888; _ZTI at
 * 0x0212287c names dActor_c as the base. The destructor is the key function.
 * defer_codegen off emits D1 at 0x0211ff34, D0 at 0x0211ff7c, then a D2
 * with no ROM home. Source order is ROM order. The factory was
 * SnowmanHead_Spawn; `new daBgSnmHed_c()` is the same sequence.
 */

#pragma defer_codegen off

#include "decl_common.h"
#include "daBgSnmHed_c.h"
#include "SpawnInfo.h"
#include "Player.h"
#include "SharedFilePtr.h"
#include "dExtAnmTexPat_c.h"
#include "dBgCh_Gnd.h"
#include "Sound.h"
#include "math/Approach.h"
#include "math/CalcLib.h"
#include "math/MtxStack.h"
#include "Particle__System.h"

struct BMD_File;
struct BTP_File;

/* The model and texture-sequence handles reach their files through these
 * wrappers' constructor and destructor veneers; the manifest maps each to
 * the retail destination. Descriptive local types, not recovered spellings. */
struct BgSnmHedModelFile : SharedFilePtr {
    BgSnmHedModelFile(u32);
    ~BgSnmHedModelFile();
};

struct BgSnmHedTexSeqFile : SharedFilePtr {
    BgSnmHedTexSeqFile(u32);
    ~BgSnmHedTexSeqFile();
};

/* Runtime asset handles, distinct from the NitroFS file IDs. */
enum BgSnmHedAssetId {
    ASSET_BIG_SNOWMAN_CRY_BTP = 0x02ae,
    ASSET_BIG_SNOWMAN_HEAD_BMD = 0x02af,
    ASSET_BIG_SNOWMAN_SMILE_BTP = 0x02b0
};

/* This TU's statics, in the order the retail initializer (0x021221f8)
 * builds them. */
BgSnmHedModelFile gBgSnmHedModelFile(ASSET_BIG_SNOWMAN_HEAD_BMD);
BgSnmHedTexSeqFile gBigSnowmanCryTextureSequenceFile(ASSET_BIG_SNOWMAN_CRY_BTP);
BgSnmHedTexSeqFile gBigSnowmanSmileTextureSequenceFile(ASSET_BIG_SNOWMAN_SMILE_BTP);

daBgSnmHed_c::StateEntry gBgSnmHedStates[4] = {
    {&daBgSnmHed_c::InitState0, &daBgSnmHed_c::State0},
    {&daBgSnmHed_c::InitState1, &daBgSnmHed_c::State1},
    {&daBgSnmHed_c::InitState2, &daBgSnmHed_c::State2},
    {&daBgSnmHed_c::InitState3, &daBgSnmHed_c::State3},
};

// @symbol _ZN12daBgSnmHed_cD1Ev
// @symbol _ZN12daBgSnmHed_cD0Ev
daBgSnmHed_c::~daBgSnmHed_c() {}

// @symbol _ZN12daBgSnmHed_c11UpdateModelEv
void daBgSnmHed_c::UpdateModel() {
    Matrix4x3_FromRotationY(&mModel.mat4x3, mAngleY);
    mModel.mat4x3.m[9] = mPosX >> 3;
    mModel.mat4x3.m[10] = (mPosY + 0x96000) >> 3;
    mModel.mat4x3.m[11] = mPosZ >> 3;
}

// @symbol _ZN12daBgSnmHed_c6State3Ev
int daBgSnmHed_c::State3() {
    /* A plain-int triple: a Vector3 here schedules the four stores
       differently. */
    struct Position {
        s32 x;
        s32 y;
        s32 z;
    } pos;
    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    pos.y += 0x1c2000;

    switch (mSubstate) {
    case 0:
        mTalkPlayer = ClosestPlayer();
        if (Vec3_HorzDist(&mPos, &mTalkPlayer->mPos) < 0x118000) {
            if (mTalkPlayer->StartTalk(*this, true))
                mSubstate++;
        }
        break;
    case 1:
        if (mTalkPlayer->ShowMessage(*this, 0xb1, (Vector3 *)&pos, 0, 2))
            mSubstate++;
        break;
    case 2:
        if (mTalkPlayer->GetTalkState() == -1) {
            unsigned int param = (*(int *)&param1) & 0xf;
            param = param & 0xff;
            param |= 0x40;
            Spawn(0xb2, param, mPos, 0, mAreaId, -1);
            mSubstate++;
        }
        break;
    }
    mTextureSequence.Advance();
    ApproachLinear(mAngleY, -0x4000, 0x514);
    mPosY = (int)0xffc427c0;
    return 1;
}

// @symbol _ZN12daBgSnmHed_c10InitState3Ev
int daBgSnmHed_c::InitState3() {
    mTextureSequence.SetFile(*(BTP_File *)gBigSnowmanSmileTextureSequenceFile.filePtr, 0, 0x1000,
                             0);
    mVertAccel = 0;
    mTerminalVelocity = 0;
    mSubstate = 0;
    mStateValue = 3;
    return 1;
}

// @symbol _ZN12daBgSnmHed_c6State2Ev
int daBgSnmHed_c::State2() {
    Vector3 pos;
    switch (mSubstate) {
    case 0:
        UpdatePos(&mCylinder);
        if (mVertSpeed < 0 && mPosY < (int)0xffc427c0) {
            pos.x = mPosX;
            pos.y = mPosY;
            pos.z = mPosZ;
            Earthquake(pos, 0x5dc000);
            func_0201267c(0x115, &mCamSpacePos);
            Particle::System::NewSimple(0x10f, mPosX, mPosY, mPosZ);
            Particle::System::NewSimple(0x110, mPosX, mPosY, mPosZ);
            Sound::PlaySub(0x20, 0x14, 0x7f, 0x15666, 0);
            mSubstate++;
        }
        break;
    case 1:
        if (DecIfAbove0_Byte(&mStateTimer) == 0) {
            Sound::PlaySub(0x20, 0x7f, 0, 0x15666, 0);
            SetState(3);
        }
        break;
    }
    return 1;
}

// @symbol _ZN12daBgSnmHed_c10InitState2Ev
int daBgSnmHed_c::InitState2() {
    mFlags &= ~1;
    mVertAccel = -0x2000;
    mTerminalVelocity = -0x3c000;
    mVertSpeed = 0x2d000;
    mStateTimer = 0x78;
    mSubstate = 0;
    mStateValue = 2;
    return 1;
}

// @symbol _ZN12daBgSnmHed_c6State1Ev
int daBgSnmHed_c::State1() {
    switch (mSubstate) {
    case 0: {
        short angle = Vec3_HorzAngle(&mPos, &mTalkPlayer->mPos);
        if (ApproachLinear(mAngleY, angle, 0x514)) {
            Vector3 pos;
            pos.x = mPosX;
            pos.y = mPosY;
            pos.z = mPosZ;
            pos.y += 0xfa000;
            if (mTalkPlayer->ShowMessage(*this, 0xaf, &pos, 0, 0))
                mSubstate++;
        }
        break;
    }
    case 1:
        if (mTalkPlayer->GetTalkState() == -1)
            SetState(0);
        break;
    }
    return 1;
}

// @symbol _ZN12daBgSnmHed_c10InitState1Ev
int daBgSnmHed_c::InitState1() {
    mSubstate = 0;
    mStateTimer = 0x96;
    mStateValue = 1;
    return 1;
}

// @symbol _ZN12daBgSnmHed_c6State0Ev
int daBgSnmHed_c::State0() {
    ApproachLinear(mAngleY, -0x4000, 0x514);
    mTextureSequence.Advance();
    if (mBodyArriving) {
        SetState(2);
        return 1;
    }
    if (DecIfAbove0_Byte(&mStateTimer) == 0) {
        Player *player = ClosestPlayer();
        if (Vec3_HorzDist(&mPos, &player->mPos) < 0x118000 && player->StartTalk(*this, true) != 0) {
            mTalkPlayer = player;
            SetState(1);
        }
    }
    return 1;
}

// @symbol _ZN12daBgSnmHed_c10InitState0Ev
int daBgSnmHed_c::InitState0() {
    mTextureSequence.SetFile(*(BTP_File *)gBigSnowmanCryTextureSequenceFile.filePtr, 0, 0x1000, 0);
    mVertAccel = 0;
    mTerminalVelocity = 0;
    mStateValue = 0;
    return 1;
}

// @symbol _ZN12daBgSnmHed_c17CallStateBehaviorEv
void daBgSnmHed_c::CallStateBehavior() {
    (this->*mState->behavior)();
}

// @symbol _ZN12daBgSnmHed_c13CallStateInitEv
void daBgSnmHed_c::CallStateInit() {
    (this->*mState->init)();
}

// @symbol _ZN12daBgSnmHed_c8SetStateEi
void daBgSnmHed_c::SetState(int state) {
    mState = &gBgSnmHedStates[state];
    CallStateInit();
}

// @symbol _ZN12daBgSnmHed_c16CleanupResourcesEv
int daBgSnmHed_c::CleanupResources() {
    gBgSnmHedModelFile.Release();
    int i = 0;
    do {
        ((SharedFilePtr **)&data_ov072_02121ffc)[i]->Release();
        i++;
    } while (i < 2);
    return 1;
}

// @symbol _ZN12daBgSnmHed_c16OnPendingDestroyEv
void daBgSnmHed_c::OnPendingDestroy() {}

// @symbol _ZN12daBgSnmHed_c6RenderEv
int daBgSnmHed_c::Render() {
    mTextureSequence.Update(mModel.data);
    mModel.Render(&mScale);
    return 1;
}

// @symbol _ZN12daBgSnmHed_c8BehaviorEv
int daBgSnmHed_c::Behavior() {
    CallStateBehavior();
    mCylinder.Clear();
    mCylinder.Update();
    UpdateModel();
    return 1;
}

// @symbol _ZN12daBgSnmHed_c13InitResourcesEv
int daBgSnmHed_c::InitResources() {
    struct Vector3 pos;
    int i;

    mModel.SetFile((BMD_File *)dExtSimpleModel_c::LoadFile(gBgSnmHedModelFile), 1, -1);

    for (i = 0; i < 2; i++) {
        void *tex = ((void **)&data_ov072_02121ffc)[i];
        dExtAnmTexPat_c::LoadFile(*(SharedFilePtr *)tex);
        dExtAnmTexPat_c::Prepare(*(BMD_File *)gBgSnmHedModelFile.filePtr,
                                 *(BTP_File *)((void **)tex)[1]);
    }

    mCylinder.Init(this, 0x96000, 0x12c000, 0x800004, 0);
    mScaleX = 0x1800;
    mScaleY = 0x1800;
    mScaleZ = 0x1800;
    mWithMeshClsn.Init(this, 0x96000, 0x96000, 0, 0);

    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    pos.y += 0x14000;
    dBgCh_Gnd ground;
    ground.SetObjAndPos(pos, 0);
    if (ground.DetectClsn() != 0)
        mPosY = ground.clsnY;
    else
        mPosY = pos.y;

    SetState(0);
    UpdateModel();
    return 1;
}

// @symbol daBgSnmHed_c_classInit
extern "C" daBgSnmHed_c *daBgSnmHed_c_classInit() {
    return new daBgSnmHed_c();
}

/* The actor profile, in SpawnInfo's layout (include/SpawnInfo.h): the
 * factory, behavior and render priorities (the behavior priority equals the
 * BIG_SNOWMAN_HEAD registry id), actor flags, then the clip offset Y, clip
 * radius, clip distance and far distance passed to SetRanges. The retail
 * values are unchanged. */
SpawnInfo<daBgSnmHed_c> g_profile_BIG_SNOWMAN_HEAD = {
    daBgSnmHed_c_classInit, 0x0111, 0x0084, 3, 0x96000, 0x96000, 0x01194000, 0x00bb8000};
