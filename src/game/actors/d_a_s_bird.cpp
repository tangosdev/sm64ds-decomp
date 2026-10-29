//cpp
/* Castle grounds birds -- ov009/daSBird_c, profile SBIRD (BIRD 343).
 *
 * ov009 is mixed (DOCK_POLE / CASTLE_WATER / METAL_NET / FLAG / BIRD).
 * RTTI ov009:0x02113928 names daSBird_c; the debug table names SBIRD.
 * This is the flock, not the flag and not castle water.
 *
 * One TU for the class's whole linker run, 0x021111a0..0x02111a70. Under
 * `#pragma defer_codegen off` .text is laid down in source order, so the file
 * is written ROM-ascending. The out-of-line destructor is the key function:
 * it comes out D1 (0x021111a0), D0 (0x021111d8), then a D2 the cartridge has
 * no home for (manifest: deadstrip), and this TU emits the class vtable and
 * RTTI.
 *
 * Behavior runs one of four states through data_ov009_02113c48, a member
 * pointer table the static initializer fills: StateSpawnFlock,
 * StateWaitForPlayer, StateLaunchFlock, StateFly.
 *
 * daSBird_c_classInit is reconstructed (RTTI daSBird_c, SBIRD registry);
 * retail does not store that spelling. Historical alias: Bird_Spawn. It is
 * `return new daSBird_c()` through the inherited fBase_c::operator new.
 *
 * NOTES:
 * - Vec3_Asr / Vec3_Sub / LenVec3 have no shared header and keep local
 *   void * externs. Random() wraps math/CalcLib.h's RandomIntInternal as
 *   unsigned: a signed % 400 / % 4000 or >> 16 changes codegen.
 * - Behavior builds the model matrix through MtxStack (math/MtxStack.h) but
 *   copies it out as twelve plain words (BirdMtx).
 * - data_ov009_02113c20 / 02113c28 SharedFilePtr handles: the initializer
 *   constructs them as file IDs 1080 / 1081. symbols.txt has no recovered
 *   names, so they are not coined.
 * - data_02082214 sine table; data_0209e650 RNG seed; func_0201267c plays
 *   sound 0x6a at mCamSpacePos.
 * - The player position is read at +0x5c of the ClosestPlayer result.
 * - g_profile_SBIRD stays outside the licensed sections.
 * - common.h is included after the class header so dExtAnmModel_c's
 *   Matrix4x3 spelling wins.
 */

#pragma defer_codegen off
#include "daSBird_c.h"
#include "common.h"
#include "SharedFilePtr.h"
#include "Sound.h"
#include "nitro/fx/fx.h"
#include "math/Approach.h"
#include "math/MtxStack.h"
#include "math/CalcLib.h"
#include "cstd.h"

/* Resource handles this TU's static initializer constructs. The wrapper
 * spellings are reconstructed; constructor and destructor addresses, file
 * IDs, widths, and BSS order are the ROM's. */
struct SBirdModelFilePtr : SharedFilePtr {
    SBirdModelFilePtr(u32 fileID);
    ~SBirdModelFilePtr();
};

struct SBirdAnimationFilePtr : SharedFilePtr {
    SBirdAnimationFilePtr(u32 fileID);
    ~SBirdAnimationFilePtr();
};

typedef void (daSBird_c::*BirdState)();

struct SBirdStateTable {
    BirdState slots[4];

    SBirdStateTable() {
        slots[0] = &daSBird_c::StateSpawnFlock;
        slots[1] = &daSBird_c::StateWaitForPlayer;
        slots[2] = &daSBird_c::StateLaunchFlock;
        slots[3] = &daSBird_c::StateFly;
    }
};

/* Behavior copies the scratch matrix as twelve plain words; a Matrix4x3
   assignment compiles to a different copy. */
struct BirdMtx {
    int w[12];
};

extern SBirdModelFilePtr data_ov009_02113c20;
extern SBirdAnimationFilePtr data_ov009_02113c28;
extern SBirdStateTable data_ov009_02113c48;
extern s16 data_02082214[];
extern int data_0209e650;

/* local extern: no shared header declares these; their definitions take a
   file-local Vec3, and this TU passes raw field addresses. */
extern "C" {
extern void Vec3_Asr(void *dst, void *src, int n);
extern void Vec3_Sub(void *out, void *a, void *b);
extern s32 LenVec3(void *v);
}

/* The flock's random draws are unsigned: a signed % or >> changes codegen. */
static inline u32 Random() {
    return RandomIntInternal(&data_0209e650);
}

/* Empty body: mShadowModel then mAnmModel teardown, the vptr store and
   dActor_c's teardown are synthesised. */
// @symbol _ZN9daSBird_cD1Ev
// @symbol _ZN9daSBird_cD0Ev
daSBird_c::~daSBird_c() {}

/* Follower attach -- spawn loops store the leader's uniqueID. */
// @symbol _ZN9daSBird_c14AttachToLeaderEi
void daSBird_c::AttachToLeader(int ownerID) {
    mIsLeader = 0;
    mOwnerID = ownerID;
}

/* Fly state -- data_ov009_02113c48[3]. Steers toward the leader's position
   (or the target point when leading) and derives forward/vertical speed. */
// @symbol _ZN9daSBird_c8StateFlyEv
void daSBird_c::StateFly() {
    UpdatePos(0);
    dActor_c *owner = dActor_c::FindWithID(mOwnerID);
    if (owner == 0 || owner->mPosY > 0xbb8000) {
        MarkForDestruction();
        return;
    }

    Vector3 diff;
    if (mIsLeader) {
        Vector3 d1;
        Vec3_Sub(&d1, &mTargetPos, &mPos);
        diff.x = d1.x;
        diff.y = d1.y;
        diff.z = d1.z;
        s32 hl = Vec3_HorzLen(&diff);
        mTargetPitch = cstd::atan2(hl, mPosY + 0xfd8f0000);
        mTargetYaw = cstd::atan2(diff.z, diff.x);
    } else {
        Vector3 d2;
        Vec3_Sub(&d2, &owner->mPos, &mPos);
        diff.x = d2.x;
        diff.y = d2.y;
        diff.z = d2.z;
        s32 hl = Vec3_HorzLen(&diff);
        mTargetPitch = cstd::atan2(hl, -diff.y);
        mTargetYaw = cstd::atan2(diff.z, diff.x);
        mFlySpeed = LenVec3(&diff) / 25 + 0x14000;
    }

    ApproachLinear(mPrevAngleX, mTargetPitch, 0x8c);
    ApproachLinear(mPrevAngleY, mTargetYaw, 0x320);

    s32 hi = 0x3000;
    s32 dd = (s16)(mPrevAngleY - mTargetYaw);
    if (dd < -hi)
        dd = -hi;
    else if (dd > hi)
        dd = hi;
    mBank = (s16)dd;
    ApproachLinear(mAngleZ, mBank, 0x258);

    {
        u16 a = *(u16 *)&mPrevAngleX;
        s32 t = mFlySpeed;
        s16 s = data_02082214[((a >> 4) << 1) + 1];
        mHorzSpeed = FX_MUL(t, s);
    }
    {
        u16 a = *(u16 *)&mPrevAngleX;
        s32 t = mFlySpeed;
        s16 s = data_02082214[(a >> 4) << 1];
        mVertSpeed = FX_MUL(t, s);
    }
}

// @symbol _ZN9daSBird_c16StateLaunchFlockEv
/* State 2. Spawn the flock and take off in one step: when the closest player is
   within 0x7d0, a leader spawns (param1 & 0xf) - 1 followers on its own
   position, aims the flock at the fixed target and chirps; then every bird
   picks a random pitch and heading and enters state 3. Nothing in this TU
   stores mState = 2, so this entry is reached only by a spawner that sets it.

   The stores sit in the taken arm of `if (player) { ... } else { return; }`
   with the sound call after the join. Written as `if (!player) return;` the
   call's argument setup is hoisted above the last store and the function
   comes out five words short. */
void daSBird_c::StateLaunchFlock() {
    if (mIsLeader) {
        char *player = (char *)ClosestPlayer();
        if (player != 0) {
            Vector3 copy;
            Vector3 diff;
            Vec3_Sub(&diff, &mPosX, player + 0x5c);
            copy.x = diff.x;
            copy.y = diff.y;
            copy.z = diff.z;
            if (Vec3_HorzLen(&copy) > 0x7d0000)
                return;
            int n = (int)param1 & 0xf;
            if (n > 1) {
                for (int i = 0; i < n - 1; i++) {
                    daSBird_c *follower =
                        (daSBird_c *)dActor_c::Spawn(0x157, 0x10, mPos, 0, mAreaId, -1);
                    if (follower)
                        follower->AttachToLeader(uniqueID);
                }
            }
            mTargetPos.x = -0x14000;
            mTargetPos.z = 0xff06a000;
        } else {
            return;
        }
        func_0201267c(0x6a, &mCamSpacePos);
    }
    mPrevAngleX = 5000 - Random() % 4000;
    mPrevAngleY = Random() >> 16;
    mFlySpeed = 0x28000;
    mState = 3;
    mFlags &= ~0x10000;
}

// @symbol _ZN9daSBird_c18StateWaitForPlayerEv
/* State 1. The leader waits for the closest player to come within 0x7d0 and
   chirps; a follower waits for its leader to take off. Then the bird picks a
   random pitch and enters state 3. */
void daSBird_c::StateWaitForPlayer() {
    if (mIsLeader) {
        char *player = (char *)ClosestPlayer();
        if (player != 0) {
            Vector3 copy;
            Vector3 diff;
            int len;
            Vec3_Sub(&diff, &mPosX, player + 0x5c);
            copy.x = diff.x;
            copy.y = diff.y;
            copy.z = diff.z;
            len = Vec3_HorzLen(&copy);
            if (len > 0x7d0000)
                return;
        }
        func_0201267c(0x6a, &mCamSpacePos);
    } else {
        daSBird_c *leader = (daSBird_c *)dActor_c::FindWithID(mOwnerID);
        if (leader == 0) {
            MarkForDestruction();
            return;
        }
        if (leader->mState != 3)
            return;
    }
    mPrevAngleX = 5000 - Random() % 4000;
    mFlySpeed = 0x28000;
    mState = 3;
    mFlags &= ~0x10000;
}

// @symbol _ZN9daSBird_c15StateSpawnFlockEv
/* State 0. A leader spawns (param1 & 0xf) - 1 followers scattered around it
   and aims the flock at the fixed target; every bird then waits in state 1. */
void daSBird_c::StateSpawnFlock() {
    if (mIsLeader) {
        int n = (int)param1 & 0xf;
        if (n > 1) {
            Vector3_16 rot = mPrevAngle;
            for (int i = 0; i < n - 1; i++) {
                Vector3 pos;
                pos.x = mPosX + (int)((Random() % 400) - 0xa0) * 4096;
                pos.y = mPosY;
                pos.z = mPosZ + (int)((Random() % 400) - 0xa0) * 4096;
                rot.y = (short)(rot.y + (short)(Random() >> 16));
                daSBird_c *follower =
                    (daSBird_c *)dActor_c::Spawn(0x157, 0, pos, &rot, mAreaId, -1);
                if (follower)
                    follower->AttachToLeader(uniqueID);
            }
        }
        mTargetPos.x = -0x14000;
        mTargetPos.z = 0xff06a000;
    }
    mState = 1;
}

// @symbol _ZN9daSBird_c16CleanupResourcesEv
s32 daSBird_c::CleanupResources() {
    data_ov009_02113c28.Release();
    data_ov009_02113c20.Release();
    return 1;
}

// @symbol _ZN9daSBird_c16OnPendingDestroyEv
void daSBird_c::OnPendingDestroy() {}

// @symbol _ZN9daSBird_c6RenderEv
s32 daSBird_c::Render() {
    mAnmModel.Render(0);
    return 1;
}

// @symbol _ZN9daSBird_c8BehaviorEv
s32 daSBird_c::Behavior() {
    (this->*data_ov009_02113c48.slots[mState])();
    int tmp[3];
    Vec3_Asr(tmp, &mPosX, 3);
    MtxStack::transS(tmp[0], tmp[1], tmp[2]);
    mAngleY = mPrevAngleY;
    Matrix4x3_ApplyInPlaceToRotationZ(&MtxStack::get(), mAngleZ);
    MtxStack::YrotM(mAngleY);
    *(BirdMtx *)&mAnmModel.mat4x3 = *(BirdMtx *)&MtxStack::get();
    DropShadowRadHeight(mShadowModel, mAnmModel.mat4x3, 0x1e000, 0x7d0000, 0xf);
    mAnmModel.Advance();
    return 1;
}

// @symbol _ZN9daSBird_c13InitResourcesEv
s32 daSBird_c::InitResources() {
    BMD_File *modelFile = (BMD_File *)dExtSimpleModel_c::LoadFile(data_ov009_02113c20);
    mAnmModel.SetFile(modelFile, 1, 1);
    BCA_File *animFile = (BCA_File *)dExtFrameCtrl_c::LoadFile(data_ov009_02113c28);
    mAnmModel.SetAnim((BCA_File *)animFile, 0, 0x1000, 0);
    mShadowModel.InitCylinder();
    {
        int *p60 = &mPosY;
        int y = *p60;
        int zero = 0;
        *p60 = y + 0xa000;
        mVertAccel = zero;
        mTerminalVelocity = -0x32000;
        mIsLeader = 1;
        mOwnerID = uniqueID;
        mTargetPos.x = mPosX;
        mTargetPos.y = mPosY;
        mTargetPos.z = mPosZ;
        mState = zero;
    }
    return 1;
}

// @symbol daSBird_c_classInit
extern "C" daSBird_c *daSBird_c_classInit() {
    return new daSBird_c();
}

/* Static-resource ownership (was the retired 0xcc handwritten initializer).
 * Definition order is the retail initializer's construction order: the
 * model handle (file 1080), the animation handle (file 1081). Their
 * registration nodes are compiler temporaries, like the PMF descriptors
 * the table initializer below materializes. */
SBirdModelFilePtr data_ov009_02113c20(1080);
SBirdAnimationFilePtr data_ov009_02113c28(1081);

SBirdStateTable data_ov009_02113c48;
