//cpp
/**
 * daShark_c -- the lake shark (SHARK), ov090.
 *
 * Swims a closed path. Behavior advances mPathNodeIdx once the actor is
 * within 0x258000 of the current node, turns toward that node, and moves
 * forward at 0x14000. The hurt cylinder cycles three lateral offsets.
 *
 * common.h FIRST. UpdateModelMatrix copies data_020a0e68 into
 * mAnmModel.mat4x3 as twelve words. common.h's flat s32 m[12] is that
 * copy; math/Matrix.h's nested {Matrix3x3 r; Vector3 t;} scalarizes it.
 *
 * #pragma defer_codegen off emits .text in source order. The out-of-line
 * destructor is the key function, so it emits D1 then D0. The factory
 * (daShark_c_classInit, 0x02133ca0..0x02133ce8, historical alias
 * Shark_Spawn) hand-called fBase_c::operator new(0x3a0) + the inherited
 * dEnemyBase_c ctor + this class's vtable store + three member subobjects
 * in field order (dCcAcPos_c, dBgCh_Actr, dExtAnmModel_c). daShark_c has no
 * user-declared constructor, so `new daShark_c()` reproduces the identical
 * sequence; the factory now appends after InitResources, at the end of
 * source order, matching its ROM placement. g_profile_SHARK is not this TU.
 *
 * The two state slots live in data_ov090_021345cc as one SharkStateTable:
 * enter is InitStateSwim and the per-frame body is StateSwim. Both are
 * static members taking the shark, so each .data descriptor is a
 * SharkFreeState function-pointer/PMF pair, and the table constructor
 * copies them into the BSS slots at startup. A PMF constant initializer
 * instead emits two extra @NNN objects and reorders .data.
 *
 * What stays:
 * - CheckPlayerHit: `if (hit->actorID != 0xbf)` is size 0x120 to 0x114.
 *   The int temporary stays.
 * - Behavior: clamping with `speed < mTerminalVelocity` and dropping the
 *   mSpeedZ copy is size 0x1c0 to 0x1b8 and shifts the calls
 *   (WRONG-DEST, UpdateModelMatrix against 0x02010d40). The
 *   compare-into-terminal and the copy stay.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daShark_c_AssetId { ASSET_HOJIRO_BMD = 0x0325, ASSET_HOJIRO_SWIM_BCA = 0x0326 };

#pragma defer_codegen off

#include "common.h"
#include "daShark_c.h"
#include "PathPtr.h"
#include "SharedFilePtr.h"
#include "Player.h"
#include "math/Approach.h"
#include "math/CalcLib.h"
#include "math/MtxStack.h"

/* local extern: no shared header declares these. */
extern "C" {
extern int func_02012694(int, void *);
extern void Vec3_Asr(void *dst, void *src, int n);
extern void Vec3_Sub(Vector3 *out, Vector3 *a, Vector3 *b);
extern s32 LenVec3(Vector3 *value);
extern s16 Vec3_VertAngle(Vector3 *a, Vector3 *b);
extern void MulVec3Mat4x3(void *a, void *matrix, void *b);
}

typedef daShark_c::StateFn SharkFn;

struct SharkState {
    SharkFn enter;
    SharkFn update;
};

/* File-scope objects at the end of this file construct the two resource
 * handles (model file 0x325, animation file 0x326) and fill the two-entry
 * state table. mwcc emits __sinit_d_a_shark.cpp from those definitions. */
struct SharkModelFilePtr : SharedFilePtr {
    SharkModelFilePtr(u32 fileID);
    ~SharkModelFilePtr();
};

struct SharkAnimationFilePtr : SharedFilePtr {
    SharkAnimationFilePtr(u32 fileID);
    ~SharkAnimationFilePtr();
};

/* Both state slots target static members, so each descriptor is a
 * function-pointer/PMF pair. The first word carries the code relocation;
 * the second is zero, like every other descriptor. */
union SharkFreeState {
    int (*fn)(daShark_c *);
    SharkFn pmf;
};

extern SharkFreeState data_ov090_021343a0;
extern SharkFreeState data_ov090_021343a8;

struct SharkStateTable {
    SharkFn slots[2];
    /* Section-alignment fill: retail BSS runs 4 bytes past the second slot,
     * to the section end at 0x021345e0. The constructor never stores here. */
    u8 tailFill[4];

    SharkStateTable() {
        slots[0] = data_ov090_021343a0.pmf;
        slots[1] = data_ov090_021343a8.pmf;
    }
};

extern SharkModelFilePtr data_ov090_021345a4;
extern SharkAnimationFilePtr data_ov090_021345ac;
extern SharkStateTable data_ov090_021345cc;

// @symbol _ZN9daShark_cD1Ev
// @symbol _ZN9daShark_cD0Ev
daShark_c::~daShark_c() {}

// @symbol _ZN9daShark_c14CheckPlayerHitEv
/* Step the hurt cylinder through its three offsets, then hurt the player
 * touching it unless the player is vanished. */
void daShark_c::CheckPlayerHit() {
    Vector3 offset;
    Vector3 pos;
    mSwingPhase += 1;
    if (mSwingPhase > 2)
        mSwingPhase = 0;
    offset.x = 0;
    offset.y = 0;
    offset.z = 0;
    if (mSwingPhase == 1)
        offset.z = 0x8c000;
    else if (mSwingPhase == 2)
        offset.z = -0x8c000;
    Matrix4x3_FromRotationY(&data_020a0e68, 0);
    MulVec3Mat4x3(&offset, &data_020a0e68, &mClsnOffset);
    mdCcAcPos_c.SetPosRelativeToActor(mClsnOffset);
    if (mdCcAcPos_c.otherOwner == 0)
        return;
    dActor_c *hit = dActor_c::FindWithID(mdCcAcPos_c.otherOwner);
    {
        int isPlayer = (hit->actorID == 0xbf); /* PLAYER */
        if (isPlayer == 0)
            return;
    }
    if (((Player *)hit)->mIsVanish)
        return;
    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    ((Player *)hit)->Hurt(pos, 3, 0xc000, 1, 0, 1);
}

// @symbol _ZN9daShark_c9StateSwimEPS_
/* Swim per-frame: the swim sound at the clip's first frame, the clip, then
 * the hit check. */
int daShark_c::StateSwim(daShark_c *self) {
    int frame = self->mAnmModel.currFrame;

    frame = (frame >> 12) << 16;
    frame = (unsigned int)frame >> 16;
    if (frame == 0)
        func_02012694(9, &self->mCamSpacePosX);
    self->mAnmModel.speed = 0x1000;
    static_cast<dExtFrameCtrl_c &>(self->mAnmModel).Advance();
    self->CheckPlayerHit();
    return 1;
}

// @symbol _ZN9daShark_c13InitStateSwimEPS_
/* Swim enter: start the swim clip. */
int daShark_c::InitStateSwim(daShark_c *self) {
    self->mAnmModel.SetAnim((BCA_File *)data_ov090_021345ac.filePtr, 0, 0x1000, 0);
    return 1;
}

// @symbol _ZN9daShark_c8SetStateEPM9daShark_cFivE
/* Install a state record (enter PMF, then per-frame PMF) and run its enter
 * routine, if it has one. */
int daShark_c::SetState(StateFn *state) {
    StateFn *enter;

    mState = state;
    enter = mState;
    if (*enter == 0)
        return 1;
    return (this->*(*enter))();
}

// @symbol _ZN9daShark_c17UpdateModelMatrixEv
/* Model matrix: position (>> 3, Vec3_Asr) and the X/Y/Z angles, built in
 * the scratch matrix and copied out. */
void daShark_c::UpdateModelMatrix() {
    int pos[3];

    Vec3_Asr(pos, &mPosX, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, pos[0], pos[1], pos[2]);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68, mAngleX, mAngleY, mAngleZ);
    mAnmModel.mat4x3 = data_020a0e68;
}

// @symbol _ZN9daShark_c16CleanupResourcesEv
int daShark_c::CleanupResources() {
    data_ov090_021345a4.Release();
    data_ov090_021345ac.Release();
    return 1;
}

// @symbol _ZN9daShark_c16OnPendingDestroyEv
void daShark_c::OnPendingDestroy() {}

// @symbol _ZN9daShark_c6RenderEv
int daShark_c::Render() {
    mAnmModel.Render((const Vector3 *)0);
    return 1;
}

// @symbol _ZN9daShark_c8BehaviorEv
int daShark_c::Behavior() {
    DecIfAbove0_Short((u16 *)&mStateTimer);
    {
        SharkState *state = (SharkState *)mState;
        if (state->update != 0)
            (this->*(state->update))();
    }
    {
        PathPtr path;
        Vector3 node;
        Vector3 difference;
        Vector3 velocity;
        int distance;

        path.FromID(mPathID);
        path.GetNode(node, mPathNodeIdx);
        Vec3_Sub(&difference, &mPos, &node);
        distance = LenVec3(&difference);
        if (distance == 0 || distance <= 0x258000) {
            mPathNodeIdx++;
            if (mPathNodeIdx >= mPathNodeCount)
                mPathNodeIdx = 0;
        }
        ApproachLinear(mPrevAngleY, Vec3_HorzAngle(&mPos, &node), 0x180);
        ApproachLinear(mPrevAngleX, Vec3_VertAngle(&mPos, &node), 0x40);
        mAngleX = mPrevAngleX;
        mAngleY = mPrevAngleY;
        mAngleZ = mPrevAngleZ;
        velocity.y = velocity.x = velocity.z = 0;
        velocity.z = 0x14000;
        Matrix4x3_FromRotationY(&data_020a0e68, mPrevAngleY);
        Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, mPrevAngleX);
        MulVec3Mat4x3(&velocity, &data_020a0e68, (Vector3 *)&mSpeedX);
    }
    {
        /* mSpeedZ is the z word of the vector just stored at mSpeedX.
         * The copy is load-bearing. */
        int speed = mVertSpeed + mVertAccel;
        int terminalVelocity = mTerminalVelocity;
        int kept = mSpeedZ;
        if (speed >= terminalVelocity)
            terminalVelocity = speed;
        mVertSpeed = terminalVelocity;
        mSpeedZ = kept;
    }
    UpdatePosWithOnlySpeed(&mdCcAcPos_c);
    UpdateModelMatrix();
    mdCcAcPos_c.Clear();
    {
        Player *player = ClosestPlayer();
        if (player != 0 && player->mIsVanish == 0)
            mdCcAcPos_c.Update();
    }
    return 1;
}

// @symbol _ZN9daShark_c13InitResourcesEv
int daShark_c::InitResources() {
    /* Each PathPtr constructs at its declaration. The first call is
     * before the cylinder init and the second is after it. */
    mAnmModel.SetFile((BMD_File *)dExtSimpleModel_c::LoadFile(data_ov090_021345a4), 1, -1);
    dExtFrameCtrl_c::LoadFile(data_ov090_021345ac);
    mPathID = (*(s32 *)&param1) & 0xff;
    if (mPathID < 0)
        mPathID = 0;
    {
        PathPtr path;
        path.FromID(mPathID);
        mPathNodeCount = path.NumNodes();
    }
    mTerminalVelocity = -0x3c000;
    mClsnOffset.x = 0;
    mClsnOffset.y = 0;
    mClsnOffset.z = 0;
    mdCcAcPos_c.Init(this, mClsnOffset, 0x42000, 0x6e000, 0x200004, 0);
    {
        PathPtr path;
        path.FromID(mPathID);
        mPathNodeIdx = 1;
        path.GetNode(mPos, mPathNodeIdx);
    }
    SetState(data_ov090_021345cc.slots);
    return 1;
}

// @symbol daShark_c_classInit
extern "C" daShark_c *daShark_c_classInit() {
    return new daShark_c();
}

/* Static-init globals (was the handwritten __sinit_ov090_02134020 shard).
 * Definition order is the retail initializer's construction order: the model
 * handle, then the animation handle, then the two state descriptors and
 * the state table they are copied into. */
SharkModelFilePtr data_ov090_021345a4(ASSET_HOJIRO_BMD);
SharkAnimationFilePtr data_ov090_021345ac(ASSET_HOJIRO_SWIM_BCA);

SharkFreeState data_ov090_021343a0 = {&daShark_c::InitStateSwim};
SharkFreeState data_ov090_021343a8 = {&daShark_c::StateSwim};

SharkStateTable data_ov090_021345cc;
