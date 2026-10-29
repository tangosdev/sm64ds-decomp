//cpp
/* daPgDfdr_c -- Penguin Defender (PENGUIN_DEFENDER 258), ov027.
 *
 * A solid penguin (it carries a KCL mesh) placed at a fixed spot that ignores
 * its spawn position. InitResources drops it onto the ground at
 * (0x6c4000, 0xcb2000, 0x182bb8) facing 0xdd30. It then patrols nine steps
 * from the stride-0xc table at data_ov027_02113a1c (distance, speed, start
 * heading, end heading). State 1 walks the current step; when the distance
 * is spent it advances the step and enters state 0, which waits 20 frames
 * and turns onto that step's end heading. Equal headings play the straight
 * walk clip (rate follows the step speed); a heading change plays the turn
 * clip. Footsteps are sound 0xf3.
 *
 * ov027 is mixed (sliding ice / chill bully / Bubba / snowman breath).
 * RTTI names this class daPgDfdr_c; the debug table names PENGUIN_DEFENDER.
 *
 * What stays:
 * - Sound::Play(3, 0xf3, pos) is +8 bytes in StateWalk (0x104 -> 0x10c).
 *   The ROM calls func_0201267c.
 * - Assigning mMeshCollider.beforeClsnCallback is -4 bytes in
 *   InitResources (0x1c8 -> 0x1c4). The ROM calls func_020393d4.
 * - Naming mAnmModel.currFrame and .file in StateWalk reorders 6 words (the
 *   straight-walk file loads after the frame). c+0x378 / c+0x380 and the
 *   ternary stay.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daPgDfdr_c_AssetId {
    ASSET_PENGUIN_BMD = 0x03fb,
    ASSET_PENGUIN_KCL = 0x03fc,
    ASSET_PENGUIN_BACK_BCA = 0x03fe,
    ASSET_PENGUIN_WAIT1_BCA = 0x0406,
    ASSET_PENGUIN_WAIT1_BTP = 0x0407,
    ASSET_PENGUIN_WALK2_BCA = 0x0409
};

#include "common.h"
#include "daPgDfdr_c.h"
#include "Player.h"
#include "decl_common.h"
#include "SharedFilePtr.h"
#include "dExtAnmTexPat_c.h"
#include "dBgCh_Gnd.h"
#include "nitro/fx/fx.h"
#include "Sound.h"
#include "math/Approach.h"
#include "math/CalcLib.h"
#include "math/MtxStack.h"

struct BMD_File;
struct BTP_File;

/* Two 8-byte daPgDfdr_c member pointers (enter, then the per-frame tick). */
struct StatePair {
    int w[4];
};

enum {
    kStateTurn = 0,
    kStateWalk = 1,
    kStepCount = 9,
    /* dActor_c::mFlags 0x2: with the clip test, skip Render. */
    kSkipRender = 2
};

/* local extern: stores dBgW::beforeClsnCallback (p[6]); dBgW.h has no
 * setter. */
extern "C" void func_020393d4(dBgW *collider, void *callback);

/* File-scope objects at the end of this file construct the six resource
 * handles and fill the four-slot state table. mwcc emits
 * __sinit_d_a_pg_dfdr.cpp from those definitions. The wrapper names are
 * local; the handle constructors and destructors are the ROM resource-family
 * functions, aliased in the manifest. */
struct PgDfdrModelFile : SharedFilePtr {
    PgDfdrModelFile(u32 fileID);
    ~PgDfdrModelFile();
};

struct PgDfdrAnimationFilePtr : SharedFilePtr {
    PgDfdrAnimationFilePtr(u32 fileID);
    ~PgDfdrAnimationFilePtr();
};

struct PgDfdrTexSequenceFilePtr : SharedFilePtr {
    PgDfdrTexSequenceFilePtr(u32 fileID);
    ~PgDfdrTexSequenceFilePtr();
};

struct PgDfdrCollisionFilePtr : SharedFilePtr {
    PgDfdrCollisionFilePtr(u32 fileID);
    ~PgDfdrCollisionFilePtr();
};

/* One 8-byte state word (a pointer-to-member: function address, this-delta).
 * The four words live in .data; the state table copies each of them. */
struct DfdrDesc {
    int w[2];
};

struct DfdrStateTable {
    DfdrDesc a, b, c, d;
};

extern PgDfdrCollisionFilePtr data_ov027_02113c6c;
extern PgDfdrModelFile data_ov027_02113c7c;
extern PgDfdrTexSequenceFilePtr data_ov027_02113c94;
extern PgDfdrAnimationFilePtr data_ov027_02113c84;
extern PgDfdrAnimationFilePtr data_ov027_02113c74;
extern PgDfdrAnimationFilePtr data_ov027_02113c8c;
extern DfdrDesc data_ov027_021139d4;
extern DfdrDesc data_ov027_021139cc;
extern DfdrDesc data_ov027_021139dc;
extern DfdrDesc data_ov027_021139c4;
extern DfdrStateTable data_ov027_02113ce4;
struct PatrolStep {
    s32 distance;
    s32 speed;
    s16 startHeading;
    s16 endHeading;
};
extern PatrolStep data_ov027_02113a1c[];

// @symbol daPgDfdr_c_classInit
extern "C" daPgDfdr_c *daPgDfdr_c_classInit() {
    return new daPgDfdr_c();
}

// @symbol _ZN10daPgDfdr_c13InitResourcesEv
s32 daPgDfdr_c::InitResources() {
    int i;
    void *file;
    Vector3 pos;

    file = dExtSimpleModel_c::LoadFile(data_ov027_02113c7c);
    mAnmModel.SetFile((BMD_File *)file, 1, -1);

    for (i = 0; i < 3; i++)
        dExtFrameCtrl_c::LoadFile(*(SharedFilePtr *)data_ov027_02112ca4[i]);

    dExtAnmTexPat_c::LoadFile(data_ov027_02113c94);
    dExtAnmTexPat_c::Prepare(*(BMD_File *)data_ov027_02113c7c.filePtr,
                             *(BTP_File *)data_ov027_02113c94.filePtr);
    mTextureSequence.SetFile(*(BTP_File *)data_ov027_02113c94.filePtr, 0, 0x1000, 0);

    mAngleY = (short)0xdd30;
    mPrevAngleY = mAngleY;
    mPosX = 0x6c4000;
    mPosY = 0xcb2000;
    mPosZ = 0x182bb8;
    UpdateModelMatrix();

    file = dBgW_Kc::LoadFile(data_ov027_02113c6c);
    mMeshCollider.SetFile((KCL_File *)file, mClsnMat, 0x199, mAngleY,
                          *(CLPS_Block *)&data_ov027_021130e8);
    func_020393d4(&mMeshCollider, (void *)&dBgW::UpdatePosAndAngs);

    mVertAccel = 0;
    mTerminalVelocity = 0;
    mdCcAc_c.Init(this, 0x82000, 0xc8000, 0x800004, 0);
    SetState(kStateWalk);

    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    pos.y += 0x14000;
    dBgCh_Gnd ground;
    ground.SetObjAndPos(pos, 0);
    if (ground.DetectClsn())
        mPosY = ground.clsnY;
    else
        mPosY = pos.y;
    mScaleX = 0x1000;
    mScaleY = 0x1000;
    mScaleZ = 0x1000;
    return 1;
}

// @symbol _ZN10daPgDfdr_c8BehaviorEv
s32 daPgDfdr_c::Behavior() {
    IsClsnInRangeOnScreen(0, 0);
    CallStateBehavior();
    if (ClosestPlayer()->IsInsideOfCannon()) {
        mFlags &= ~kSkipRender;
    } else {
        mFlags |= kSkipRender;
    }
    mAnmModel.Advance();
    mTextureSequence.Advance();
    mdCcAc_c.Clear();
    mdCcAc_c.Update();
    UpdateModelMatrix();
    return 1;
}

// @symbol _ZN10daPgDfdr_c6RenderEv
s32 daPgDfdr_c::Render() {
    mTextureSequence.Update(mAnmModel.data);
    mAnmModel.Render(0);
    return 1;
}

// @symbol _ZN10daPgDfdr_c16OnPendingDestroyEv
void daPgDfdr_c::OnPendingDestroy() {}

// @symbol _ZN10daPgDfdr_c16CleanupResourcesEv
s32 daPgDfdr_c::CleanupResources() {
    int i;
    data_ov027_02113c7c.Release();
    for (i = 0; i < 3; i++) {
        ((SharedFilePtr *)data_ov027_02112ca4[i])->Release();
    }
    data_ov027_02113c94.Release();
    if (mMeshCollider.IsEnabled()) {
        mMeshCollider.Disable();
    }
    data_ov027_02113c6c.Release();
    return 1;
}

// @symbol _ZN10daPgDfdr_c8SetStateEi
/* Select state idx and run its enter function. */
void daPgDfdr_c::SetState(int idx) {
    mStateTable = &((StatePair *)&data_ov027_02113ce4)[idx];
    CallStateInit();
}

// @symbol _ZN10daPgDfdr_c13CallStateInitEv
void daPgDfdr_c::CallStateInit() {
    typedef void (daPgDfdr_c::*Fn)();
    Fn *fn = (Fn *)mStateTable;
    (this->**fn)();
}

// @symbol _ZN10daPgDfdr_c17CallStateBehaviorEv
void daPgDfdr_c::CallStateBehavior() {
    typedef void (daPgDfdr_c::*Fn)();
    Fn *fn = (Fn *)mStateTable + 1;
    (this->**fn)();
}

// @symbol _ZN10daPgDfdr_c13InitStateTurnEv
/* State 0 enter: pause-and-turn clip, 20 frames, mIsWalking = 0. */
int daPgDfdr_c::InitStateTurn() {
    mAnmModel.SetAnim((BCA_File *)data_ov027_02113c84.filePtr, 0, 0x1000, 0);
    mAnmModel.speed = 0x1000;
    mTimer = 20;
    mIsWalking = 0;
    return 1;
}

// @symbol _ZN10daPgDfdr_c9StateTurnEv
/* State 0 tick: after the pause, turn mAngleY toward this step's end heading. */
int daPgDfdr_c::StateTurn() {
    if (DecIfAbove0_Byte(&mTimer) == 0) {
        unsigned char step = mStepIndex;
        short endHeading = data_ov027_02113a1c[step].endHeading;
        if (ApproachLinear(mAngleY, endHeading, 0x514) != 0) {
            SetState(kStateWalk);
        }
    }
    return 1;
}

// @symbol _ZN10daPgDfdr_c13InitStateWalkEv
/* State 1 enter: distance, speed and start heading of this step, then the
   straight-walk clip when the headings match and the turn clip when they do not. */
void daPgDfdr_c::InitStateWalk() {
    mDistanceLeft = data_ov027_02113a1c[mStepIndex].distance;
    mHorzSpeed = data_ov027_02113a1c[mStepIndex].speed;
    mPrevAngleY = data_ov027_02113a1c[mStepIndex].startHeading;
    if (data_ov027_02113a1c[mStepIndex].startHeading !=
        data_ov027_02113a1c[mStepIndex].endHeading) {
        mAnmModel.SetAnim((BCA_File *)data_ov027_02113c8c.filePtr, 0, 0x1000, 0);
        mAnmModel.speed = 0x1000;
    } else {
        mAnmModel.SetAnim((BCA_File *)data_ov027_02113c74.filePtr, 0, 0x1000, 0);
        mAnmModel.speed = FX_MUL(data_ov027_02113a1c[mStepIndex].speed, 0x5000);
    }
    mIsWalking = 1;
}

// @symbol _ZN10daPgDfdr_c9StateWalkEv
/* State 1 tick: spend mDistanceLeft. At zero, wrap the step and enter the turn.
   Otherwise update position and play 0xf3 on the walk clip's footstep frames. */
int daPgDfdr_c::StateWalk() {
    int distLeft = mDistanceLeft;
    if (distLeft == 0) {
        /* Plain `mStepIndex++` emits the same .text (`mStepIndex =
           mStepIndex + 1` does not: it folds into ldrb [this, #0x3d9] and
           drops the pooled add, 0x104 -> 0xf8). The pointer local feeds
           mwccarm's per-TU @NNN counter, one number per named local: without
           it __sinit's registration nodes come out one number below the
           ones the manifest renames. */
        u8 *step = &mStepIndex;
        (*step)++;
        if (mStepIndex >= kStepCount)
            mStepIndex = 0;
        SetState(kStateTurn);
        return 1;
    }
    {
        int speed = mHorzSpeed;
        if (distLeft < speed) {
            mHorzSpeed = distLeft;
            mDistanceLeft = 0;
        } else {
            mDistanceLeft -= speed;
        }
    }
    UpdatePos(&mdCcAc_c);
    {
        /* currFrame is +0x378 and the BCA is +0x380. Naming them reorders
           6 words; the ternary keeps the straight-walk file in the compare. */
        char *c = (char *)this;
        int frameRaw = *(int *)(c + 0x378);
        int walkAnim = (int)data_ov027_02113c74.filePtr;
        int curAnim = walkAnim ? *(int *)(c + 0x380) : *(int *)(c + 0x380);
        unsigned int frame = (unsigned int)(frameRaw << 4) >> 0x10;
        if (curAnim == walkAnim) {
            if (frame == 0xa || frame == 0x16) {
                func_0201267c(0xf3, &mCamSpacePos);
            }
        } else if (curAnim == (int)data_ov027_02113c8c.filePtr) {
            if (frame == 9 || frame == 0x16) {
                func_0201267c(0xf3, &mCamSpacePos);
            }
        }
    }
    return 1;
}

// @symbol _ZN10daPgDfdr_c17UpdateModelMatrixEv
/* dExtSimpleModel_c matrix from mAngleY plus position >> 3; collider copy keeps the
   full position. Flat Matrix4x3 (common.h before dExtSimpleModel_c.h) is the 12-word copy. */
void daPgDfdr_c::UpdateModelMatrix() {
    Matrix4x3_FromRotationY(&mAnmModel.mat4x3, mAngleY);
    mAnmModel.mat4x3.m[9] = mPosX >> 3;
    mAnmModel.mat4x3.m[10] = mPosY >> 3;
    mAnmModel.mat4x3.m[11] = mPosZ >> 3;
    mClsnMat = mAnmModel.mat4x3;
    mClsnMat.m[9] = mPosX;
    mClsnMat.m[10] = mPosY;
    mClsnMat.m[11] = mPosZ;
    mMeshCollider.Transform(mClsnMat, mAngleY);
}
// @symbol _ZN10daPgDfdr_cD1Ev
// @symbol _ZN10daPgDfdr_cD0Ev
/* -------------------------------------------------------------------------- */
/* ROM ordinals 0 and 1 -- _ZN10daPgDfdr_cD1Ev 0x021118c8 size 0x5c and        */
/* _ZN10daPgDfdr_cD0Ev 0x02111924 size 0x70 -- are NOT written here.           */
/*                                                                            */
/* The destructor is defined INLINE in include/daPgDfdr_c.h; see change (1) in */
/* this file's header for why. One vtable store and three member teardowns     */
/* come out of that one empty body: its own vptr, then -- in reverse           */
/* declaration order -- dCcAc_c at +0x398, dExtAnmTexPat_c at +0x384 and       */
/* dExtAnmModel_c at +0x320, then dBgActor_c's own teardown inlined (its destructor */
/* is defined in its class body too), which is dBgW_KcMbg at +0x124 and dExtSimpleModel_c  */
/* at +0x0d4, then dActor_c. D0 is that plus the inherited inline `operator    */
/* delete` found by ordinary lookup on dActor_c two levels up; slot 17 is the  */
/* deleting variant.                                                          */
/* -------------------------------------------------------------------------- */

/* Static-init globals (was the handwritten __sinit_ov027_02112df8 shard).
 * Definition order is the retail initializer's construction order: the model
 * handle (file 0x3fb), the three animation handles (files 0x406, 0x409,
 * 0x3fe), the texture-sequence handle (file 0x407), the collision handle
 * (file 0x3fc), then the four state words copied into the state table. */
PgDfdrModelFile data_ov027_02113c7c(ASSET_PENGUIN_BMD);
PgDfdrAnimationFilePtr data_ov027_02113c84(ASSET_PENGUIN_WAIT1_BCA);
PgDfdrAnimationFilePtr data_ov027_02113c74(ASSET_PENGUIN_WALK2_BCA);
PgDfdrAnimationFilePtr data_ov027_02113c8c(ASSET_PENGUIN_BACK_BCA);
PgDfdrTexSequenceFilePtr data_ov027_02113c94(ASSET_PENGUIN_WAIT1_BTP);
PgDfdrCollisionFilePtr data_ov027_02113c6c(ASSET_PENGUIN_KCL);
DfdrStateTable data_ov027_02113ce4 = {data_ov027_021139d4, data_ov027_021139cc, data_ov027_021139dc,
                                      data_ov027_021139c4};
