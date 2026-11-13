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
 * - Vec3_Sub has no shared header and keeps a local extern. Random() wraps math/CalcLib.h's RandomIntInternal as
 *   unsigned: a signed % 400 / % 4000 or >> 16 changes codegen.
 * - Behavior builds the model matrix through MtxStack (math/MtxStack.h) but
 *   copies it out as twelve plain words (Mtx43).
 * - data_ov009_02113c20 / 02113c28 SharedFilePtr handles: the initializer
 *   constructs them as file IDs 1080 / 1081. symbols.txt has no recovered
 *   names, so they are not coined.
 * - data_0209e650 RNG seed; func_0201267c plays
 *   sound 0x6a at mCamSpacePos.
 * - The flock measures from the ClosestPlayer result's mPos.
 * - g_profile_SBIRD stays outside the licensed sections.
 * - common.h is included after the class header so dExtAnmModel_c's
 *   Matrix4x3 spelling wins.
 */

#pragma defer_codegen off
#include "daSBird_c.h"
#include "common.h"
#include "daPly_c.h"
#include "SharedFilePtr.h"
#include "Sound.h"
#include "nitro/fx/fx.h"
#include "nitro/fx/fx_trig.h"
#include "math/Approach.h"
#include "math/MtxStack.h"
#include "math/CalcLib.h"
#include "cstd.h"
#include "private/mtx43.h"
#include "LevelRng.h"

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

/* Behavior copies the scratch matrix through the flat Mtx43; a Matrix4x3
   assignment compiles to a different copy (.text grows by 0x1c bytes). */
extern SBirdModelFilePtr data_ov009_02113c20;
extern SBirdAnimationFilePtr data_ov009_02113c28;
extern SBirdStateTable data_ov009_02113c48;

/* local extern: no shared header declares Vec3_Sub. */

extern "C" void Vec3_Sub(Vector3 *out, const Vector3 *a, const Vector3 *b);

/* The flock's random draws are unsigned: a signed % or >> changes codegen. */
static inline u32 Random() {
    return RandomIntInternal(&data_0209e650.value);
}

/* Empty body: mShadowModel then mAnmModel teardown, the vptr store and
   dActor_c's teardown are synthesised. */
daSBird_c::~daSBird_c() {}

/* Follower attach -- spawn loops store the leader's uniqueID. */
void daSBird_c::AttachToLeader(int ownerID) {
    mIsLeader = 0;
    mOwnerID = ownerID;
}

/* Fly state -- data_ov009_02113c48[3]. Steers toward the leader's position
   (or the target point when leading) and derives forward/vertical speed. */
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
        mFlySpeed = VEC_Mag(&diff) / 25 + 0x14000;
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

    mHorzSpeed = FX_MUL(mFlySpeed, FX_CosIdx((u16)mPrevAngleX >> 4));
    mVertSpeed = FX_MUL(mFlySpeed, FX_SinIdx((u16)mPrevAngleX >> 4));
}

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
        daPly_c *player = ClosestPlayer();
        if (player != 0) {
            Vector3 copy;
            Vector3 diff;
            Vec3_Sub(&diff, &mPos, &player->mPos);
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

/* State 1. The leader waits for the closest player to come within 0x7d0 and
   chirps; a follower waits for its leader to take off. Then the bird picks a
   random pitch and enters state 3. */
void daSBird_c::StateWaitForPlayer() {
    if (mIsLeader) {
        daPly_c *player = ClosestPlayer();
        if (player != 0) {
            Vector3 copy;
            Vector3 diff;
            int len;
            Vec3_Sub(&diff, &mPos, &player->mPos);
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

s32 daSBird_c::CleanupResources() {
    data_ov009_02113c28.Release();
    data_ov009_02113c20.Release();
    return 1;
}

void daSBird_c::OnPendingDestroy() {}

s32 daSBird_c::Render() {
    mAnmModel.Render(0);
    return 1;
}

s32 daSBird_c::Behavior() {
    (this->*data_ov009_02113c48.slots[mState])();
    Vector3 tmp;
    Vec3_Asr(&tmp, &mPos, 3);
    MtxStack::transS(tmp.x, tmp.y, tmp.z);
    mAngleY = mPrevAngleY;
    Matrix4x3_ApplyInPlaceToRotationZ(&MtxStack::get(), mAngleZ);
    MtxStack::YrotM(mAngleY);
    *(Mtx43 *)&mAnmModel.mat4x3 = *(Mtx43 *)&MtxStack::get();
    DropShadowRadHeight(mShadowModel, mAnmModel.mat4x3, 0x1e000, 0x7d0000, 0xf);
    mAnmModel.Advance();
    return 1;
}

s32 daSBird_c::InitResources() {
    BMD_File *modelFile = dExtSimpleModel_c::LoadFile(data_ov009_02113c20);
    mAnmModel.SetFile(modelFile, 1, 1);
    BCA_File *animFile = dExtFrameCtrl_c::LoadFile(data_ov009_02113c28);
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

 daSBird_c *daSBird_c_classInit() {
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
