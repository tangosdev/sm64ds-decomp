//cpp
/* Dire, Dire Docks whirlpool (WATER_TATUMAKI). A player who comes within
 * 300 of the centre is caught and swung down the funnel; once they pass the
 * centre, KillPlayer runs once. Anyone merely inside 1100 is pulled inward,
 * unless they are metal. The model is water_tatumaki.bmd (handle 0x4a9) and
 * the joint animation is water_tatumaki.bca (handle 0x4a8). The texture
 * animation is the overlay BTA at data_ov026_02112f40, not a NitroFS file.
 * Fourteen functions, ov026 .text 0x02111aa0..0x021121fc; the registry
 * factory daWater_Tatumaki_c_classInit is the last. No g_profile in this TU.
 *
 * Distances below are 20.12 (1.0 == 0x1000). Angles are s16, full circle
 * 0x10000.
 *
 * Two states: spin (InitStateSpin / StateSpin, installed by InitResources)
 * and drag (InitStateDrag / StateDrag, installed by StateSpin once the
 * player reaches the centre). SetState installs a state.
 *
 * What stays:
 * - StateDrag: ApproachLinear on mPullPos with a Fix12<int>
 *   temporary is 0x198 against ROM 0x190 and adds a 4-byte .rodata word.
 *   The int step math/Approach.h declares keeps it in a register.
 * - StateDrag: without volatile, the spilled player position is
 *   0x178 against ROM 0x190. The ROM stores the three words and compares
 *   the y still in the register.
 * - StateDrag, InitStateDrag, StateSpin, UpdateModelMatrix: a TU-local reference to mPosX grows those
 *   functions by 4 bytes (0x190, 0x98, 0x18c, 0x74 become 0x194, 0x9c,
 *   0x190, 0x78).
 * - InitStateDrag: deleting the stores of 0 to mPullHeight and
 *   mPullRadius is 0x90 against ROM 0x98. Deleting unk_190 = 0 is 0x94
 *   against ROM 0x98. The ROM zeroes both heights and then overwrites
 *   them; unk_190 is stored 0 and never read in this TU.
 * - UpdateModelMatrix: mAnmModel.mat4x3 = data_020a0e68 is 0x90 against
 *   ROM 0x74. This TU sees Matrix4x3 as {Matrix3x3 r; Vector3 t}. The ROM
 *   copies twelve words with one three-pass ldm/stm.
 * - InitResources: dExtAnmTexSRT_c::SetFile with a Fix12<int> temporary
 *   is 0xd8 against ROM 0xd0 and adds a 4-byte .rodata word.
 *   dExtAnmModel_c::SetAnim the same way is 0xdc against ROM 0xd0, also with
 *   a 4-byte .rodata word. Both are member calls under their scalar
 *   spellings, which match.
 * - Behavior: Particle::System::New(mParticleID, 0x139, mPosX,
 *   mPosY + 0x384000, mPosZ, 0, 0) is 0xc8 against ROM 0xe0. The ROM
 *   spills the position, adds the lift into the spill, and reloads x and
 *   the raised y.
 * - Without #pragma defer_codegen off the sections come out in reverse
 *   source order (InitResources first, D1 last). The pragma is what puts
 *   this TU in ROM order. D2 still has no cartridge home.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daWater_Tatumaki_c_AssetId {
    ASSET_WATER_TATUMAKI_BCA = 0x04a8,
    ASSET_WATER_TATUMAKI_BMD = 0x04a9
};

#pragma defer_codegen off

#include "daWater_Tatumaki_c.h"
#include "dActor_c.h"
#include "common.h"
#include "Player.h"
#include "SharedFilePtr.h"
#include "dExtAnmTexSRT_c.h"
#include "private/mtx43.h"
#include "math/Approach.h"
#include "math/CalcLib.h"
#include "math/MtxStack.h"
#include "Particle__System.h"

/* 20.12 distances and s16 angles used by the funnel. Same numeric value can
   be a fix12 step and an angle; the names stay with the call that uses them. */
enum {
    kAnimSpeed = 0x1000,
    kSpinTarget = 0x1000,
    kSpinStep = 0x200,
    kTightRadius = 0x100,
    kRadiusStep = 0x1000,
    kFunnelDepth = 0x64000, /* 100.0 */
    kSinkStep = 0x4000,     /* 4.0 */
    kChaseStep = 0x1e000,   /* 30.0 */
    kHalfTurn = 0x8000,
    kLookDown = -0x2000,
    kPullTilt = 0x2000,
    kCaptureReach = 0x12c000, /* 300.0 */
    kDrawReach = 0x44c000,    /* 1100.0 */
    kDrawDivisor = 35,
    kSprayLift = 0x384000, /* 900.0 above the actor */
    kSprayEffect = 0x139
};

/* File handles this TU's static initializer constructs, and the two state
   records it copies pointer-to-member descriptors into. Definitions are at
   the end of the file. */

struct TatumakiModelFilePtr : SharedFilePtr {
    TatumakiModelFilePtr(u32 fileID);
    ~TatumakiModelFilePtr();
};

struct TatumakiAnimFileHandle : SharedFilePtr {
    TatumakiAnimFileHandle(u32 fileID);
    ~TatumakiAnimFileHandle();
};

extern "C" TatumakiModelFilePtr gWaterTatumakiModelFile;       /* water_tatumaki.bmd, 0x4a9 */
extern "C" TatumakiAnimFileHandle gWaterTatumakiAnimationFile; /* water_tatumaki.bca, 0x4a8 */
extern daWater_Tatumaki_c::State data_ov026_02113f2c;          /* spin, waiting to catch */
extern daWater_Tatumaki_c::State data_ov026_02113f3c;          /* drag the player down */
extern BTA_File data_ov026_02112f40;

#define WHIRLPOOL_BTA data_ov026_02112f40
#define STATE_SPIN (&data_ov026_02113f2c)
#define STATE_DRAG (&data_ov026_02113f3c)

/* local extern: no shared header declares these. */
extern "C" {
extern void MulVec3Mat4x3(Vector3 *in, Matrix4x3 *m, Vector3 *out);
extern void Vec3_Asr(void *dst, void *src, int n);
extern void KillPlayer(void);
}

// @symbol _ZN18daWater_Tatumaki_cD1Ev
daWater_Tatumaki_c::~daWater_Tatumaki_c() {}

// @symbol _ZN18daWater_Tatumaki_cD0Ev
/* D0 is emitted from the destructor above. #pragma defer_codegen off puts
   D1 then D0, which is the cartridge order. The trailing D2 has no ROM home. */

// @symbol _ZN18daWater_Tatumaki_c9StateDragEv
/* Drag, each frame: orbit the player around the funnel and sink them. Below
   the centre, kill them once. */
int daWater_Tatumaki_c::StateDrag() {
    Vector3 target;
    Vector3 offset;
    volatile s32 spilled[3]; /* stored, never read; the compare uses y in r1 */
    Player *player;

    target.x = 0;
    target.y = 0;
    target.z = 0;
    offset.x = 0;
    offset.y = 0;
    offset.z = 0;
    target.z = this->mPullRadius;
    player = this->ClosestPlayer();
    if (player) {
        Matrix4x3_FromRotationY(&data_020a0e68, this->mPullAngle);
        MulVec3Mat4x3(&target, &data_020a0e68, &offset);
        target.x = this->mCenter.x;
        target.y = this->mCenter.y;
        target.z = this->mCenter.z;
        ApproachLinear2(this->mPullSpin, kSpinTarget, kSpinStep);
        ApproachLinear(this->mPullRadius, kTightRadius, kRadiusStep);
        ApproachLinear(this->mPullHeight, this->mCenter.y - kFunnelDepth, kSinkStep);
        this->mPullAngle += this->mPullSpin;
        target.y = this->mPullHeight;
        target.x += offset.x;
        target.z += offset.z;
        ApproachLinear(this->mPullPos, target, kChaseStep);
        player->mPosX = this->mPullPos.x;
        player->mPosY = this->mPullPos.y;
        player->mPosZ = this->mPullPos.z;
        {
            int ang = this->HorzAngleToCPlayer() + kHalfTurn;
            player->mAngleX = kLookDown;
            player->mAngleY = ang;
            player->mAngleZ = 0;
        }
        {
            const Vector3 *playerPos = &player->mPos;
            spilled[0] = playerPos->x;
            spilled[1] = playerPos->y;
            spilled[2] = playerPos->z;
            if (this->mCenter.y > playerPos->y) {
                if (this->mPlayerKilled == 0) {
                    KillPlayer();
                    this->mPlayerKilled = 1;
                }
            }
        }
    }
    return 1;
}

// @symbol _ZN18daWater_Tatumaki_c13InitStateDragEv
/* Drag, on entry: start the orbit where the player is standing. */
int daWater_Tatumaki_c::InitStateDrag() {
    Player *player = this->ClosestPlayer();
    if (player) {
        const Vector3 *playerPos = &player->mPos;
        Vector3 pos;
        pos.x = playerPos->x;
        pos.y = playerPos->y;
        pos.z = playerPos->z;
        this->mPullAngle = Vec3_HorzAngle(&this->mPos, &pos);
        this->unk_190 = 0;
        this->mPullHeight = 0;
        this->mPullRadius = 0;
        this->mPullPos.x = pos.x;
        this->mPullPos.y = pos.y;
        this->mPullPos.z = pos.z;
        this->mPullHeight = pos.y;
        this->mPullRadius = Vec3_HorzDist(&this->mCenter, &pos);
    }
    return 1;
}

// @symbol _ZN18daWater_Tatumaki_c9StateSpinEv
/* Spin, each frame: catch a player who has reached the centre, otherwise
   draw a non-metal player inward. The pull is stronger when they are closer. */
int daWater_Tatumaki_c::StateSpin() {
    Player *player = this->ClosestPlayer();
    if (player != 0) {
        Vector3 pos;
        {
            const Vector3 *playerPos = &player->mPos;
            pos = *playerPos;
        }

        if (Vec3_HorzDist(&this->mCenter, &pos) <= kCaptureReach) {
            int dy = this->mCenter.y - pos.y;
            if (dy < 0)
                dy = -dy;
            if (dy <= kCaptureReach) {
                player->EnterWhirlpool();
                SetState(STATE_DRAG);
                return 1;
            }
        }

        if (player->mIsMetal == 0) {
            int dist = Vec3_Dist(&this->mCenter, &pos);
            if (dist < kDrawReach) {
                Vector3 pull;
                Vector3 out;
                int speed;
                speed = kDrawReach;
                speed -= dist;
                speed = speed / kDrawDivisor;
                pull.x = 0;
                pull.y = 0;
                pull.z = speed;
                out.x = 0;
                out.y = 0;
                out.z = 0;

                Matrix4x3_FromRotationY(&data_020a0e68,
                                        (short)(this->HorzAngleToCPlayer() + kHalfTurn));
                Matrix4x3_ApplyInPlaceToRotationX(&data_020a0e68, kPullTilt);
                MulVec3Mat4x3(&pull, &data_020a0e68, &out);

                {
                    const Vector3 *playerPos = &player->mPos;
                    pull = *playerPos;
                }
                pull.x += out.x;
                pull.y += out.y;
                pull.z += out.z;
                player->mPosX = pull.x;
                player->mPosY = pull.y;
                player->mPosZ = pull.z;
            }
        }
    }
    return 1;
}

// @symbol _ZN18daWater_Tatumaki_c13InitStateSpinEv
/* Spin, on entry: nothing to set up. */
int daWater_Tatumaki_c::InitStateSpin() {
    return 1;
}

// @symbol _ZN18daWater_Tatumaki_c8SetStateEPKNS_5StateE
/* Install a state and run its enter half, if it has one. */
int daWater_Tatumaki_c::SetState(const State *state) {
    mState = state;
    const State *s = mState;
    if (s->enter == 0)
        return 1;
    return (this->*s->enter)();
}

// @symbol _ZN18daWater_Tatumaki_c17UpdateModelMatrixEv
/* Rebuild the model matrix from the position and angles. */
void daWater_Tatumaki_c::UpdateModelMatrix() {
    Vector3 pos;
    Vec3_Asr(&pos, &mPos, 3);
    Matrix4x3_FromTranslation(&data_020a0e68, pos.x, pos.y, pos.z);
    Matrix4x3_ApplyInPlaceToRotationXYZExt(&data_020a0e68, mAngleX, mAngleY, mAngleZ);
    /* Twelve flat words. A Matrix4x3 assignment copies r and t separately. */
    *(Mtx43 *)&mAnmModel.mat4x3 = *(Mtx43 *)&data_020a0e68;
}

// @symbol _ZN18daWater_Tatumaki_c16CleanupResourcesEv
int daWater_Tatumaki_c::CleanupResources() {
    gWaterTatumakiModelFile.Release();
    gWaterTatumakiAnimationFile.Release();
    return 1;
}

// @symbol _ZN18daWater_Tatumaki_c16OnPendingDestroyEv
void daWater_Tatumaki_c::OnPendingDestroy() {}

// @symbol _ZN18daWater_Tatumaki_c6RenderEv
int daWater_Tatumaki_c::Render() {
    mTextureTransformer.Update(mAnmModel.data);
    mAnmModel.Render(0);
    return 1;
}

// @symbol _ZN18daWater_Tatumaki_c8BehaviorEv
int daWater_Tatumaki_c::Behavior() {
    volatile int spilled[3]; /* x and the raised y are reloaded as arguments */
    int x, y, z;

    DecIfAbove0_Short((u16 *)&mStateTimer);
    if (mState->execute != 0) {
        (this->*mState->execute)();
    }
    UpdatePos(0);

    x = mPosX;
    spilled[0] = x;
    y = mPosY;
    spilled[1] = y;
    z = mPosZ;
    spilled[2] = z;
    y += kSprayLift;
    spilled[1] = y;

    mParticleID = Particle::System::New(*(volatile u32 *)&mParticleID, kSprayEffect, spilled[0],
                                        spilled[1], z, 0, 0);

    mAngleX = mPrevAngleX;
    mAngleY = mPrevAngleY;
    mAngleZ = mPrevAngleZ;

    UpdateModelMatrix();
    mTextureTransformer.Advance();
    mAnmModel.Advance();
    return 1;
}

// @symbol _ZN18daWater_Tatumaki_c13InitResourcesEv
int daWater_Tatumaki_c::InitResources() {
    BMD_File *model = (BMD_File *)dExtSimpleModel_c::LoadFile(gWaterTatumakiModelFile);
    mAnmModel.SetFile(model, 1, -1);

    dExtFrameCtrl_c::LoadFile(gWaterTatumakiAnimationFile);

    dExtAnmTexSRT_c::Prepare(*(BMD_File *)gWaterTatumakiModelFile.filePtr, WHIRLPOOL_BTA);
    mTextureTransformer.SetFile(WHIRLPOOL_BTA, 0, kAnimSpeed, 0);

    mTextureTransformer.speed = kAnimSpeed;
    mAnmModel.speed = kAnimSpeed;

    mCenter.x = mPosX;
    mCenter.y = mPosY;
    mCenter.z = mPosZ;
    mCenter.y -= kFunnelDepth;

    mAnmModel.SetAnim((BCA_File *)gWaterTatumakiAnimationFile.filePtr, 0, kAnimSpeed, 0);

    SetState(STATE_SPIN);
    return 1;
}

/* Reconstructed source-style name: SM64DS proves daWater_Tatumaki_c through
 * RTTI, allocation size, vtable identity, and the WATER_TATUMAKI registry
 * profile; later EAD lineage supplies classInit. Exact original spelling is
 * not preserved. Historical alias: Whirlpool_Spawn. */
// @symbol daWater_Tatumaki_c_classInit
extern "C" daWater_Tatumaki_c *daWater_Tatumaki_c_classInit() {
    return new daWater_Tatumaki_c();
}

/* Static initializer: model handle, anim handle, then the two state records.
   The pointer-to-member descriptors stay anonymous compiler objects. */
TatumakiModelFilePtr gWaterTatumakiModelFile(ASSET_WATER_TATUMAKI_BMD);
TatumakiAnimFileHandle gWaterTatumakiAnimationFile(ASSET_WATER_TATUMAKI_BCA);

daWater_Tatumaki_c::State data_ov026_02113f2c = {
    &daWater_Tatumaki_c::InitStateSpin,
    &daWater_Tatumaki_c::StateSpin,
};
daWater_Tatumaki_c::State data_ov026_02113f3c = {
    &daWater_Tatumaki_c::InitStateDrag,
    &daWater_Tatumaki_c::StateDrag,
};
