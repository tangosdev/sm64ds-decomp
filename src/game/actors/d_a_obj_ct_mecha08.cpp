//cpp
/* Tick Tock Clock's up-and-down lift block (CT_MECHA08A/CT_MECHA08B): one
 * class behind two profiles. InitResources loads model and collision,
 * remembers home, seeds speed/timer from per-clock-setting tables. Behavior
 * runs the lift between mHomePosY and mHomePosY + 0x14a000; setting 3 parks
 * it at the top, setting 2 re-rolls the timer.
 *
 * 8 functions, ov065 .text 0x0211b7f0..0x0211bbdc, closing with the two
 * classInit factories. The destructor pair comes off the in-class
 * `~daObjCtMecha08_c() {}` and lands D1 (0x0211b7f0) then D0 (0x0211b834),
 * the cartridge's order. Source order is reverse ROM order (2004/b56 emits
 * .text back to front); do not reorder. The four file-scope handles fold
 * the __sinit_ov065_0211c9b8 shard (retail construction order); _ZTV/_ZTI/
 * _ZTS stay compiler-only (cartridge copies at ov065 0x0211d494/0x0211d434/
 * 0x0211d440).
 */

// Notes: dBgW_KcMbg::SetFile and dBgActor_c::IsClsnInRange are header member
// calls whose headers spell Fix12 as Fix12i, a plain s32 (Fix12<int> by value
// homes the argument and size-DIFFs, 6az). data_ov065_0211c0d0/0211c0d4 speed
// and timer tables, data_0209f2c0 clock byte, data_0209e650 RNG, and the
// ov035 CLPS block stay linker-named. g_profile_CT_MECHA08A/B live outside
// this TU.

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daObjCtMecha08_c_AssetId {
    ASSET_CT_MECHA_OBJ08A_BMD = 0x05c8,
    ASSET_CT_MECHA_OBJ08A_KCL = 0x05c9,
    ASSET_CT_MECHA_OBJ08B_BMD = 0x05ca,
    ASSET_CT_MECHA_OBJ08B_KCL = 0x05cb
};

#include "daObjCtMecha08_c.h"
#include "SharedFilePtr.h"
#include "dBgW.h"
#include "math/CalcLib.h"

struct CLPS_Block;

/* File-scope objects at the end of this file construct the four resource
 * handles (model files 0x5c8/0x5ca, collision files 0x5c9/0x5cb). mwcc
 * emits __sinit_d_a_obj_ct_mecha08.cpp from those definitions. The wrapper
 * names are local; the handle constructors and destructors are the ROM
 * resource-family functions, aliased in the manifest. The CT_MECHA08A
 * handles are named like the 08B pair: this class is a lift block, not a
 * gear. */
struct Mecha08ModelFile : SharedFilePtr {
    Mecha08ModelFile(u32 fileID);
    ~Mecha08ModelFile();
};

struct Mecha08CollisionFilePtr : SharedFilePtr {
    Mecha08CollisionFilePtr(u32 fileID);
    ~Mecha08CollisionFilePtr();
};

extern Mecha08ModelFile gCtMechaObj08aModelFile;
extern Mecha08CollisionFilePtr gCtMechaObj08aCollisionFile;

enum {
    CLOCK_SETTING_RANDOM = 2,  /* Behavior re-rolls the timer at random */
    CLOCK_SETTING_STOPPED = 3, /* Behavior parks the lift at the top */
    LIFT_TRAVEL = 0x14a000     /* height above mHomePosY */
};

extern CLPS_Block data_ov035_021121b8;
extern s32 data_ov065_0211c0d0[][2][2]; /* vertical speed, by setting, by direction */
extern u16 data_ov065_0211c0d4[][2][4]; /* travel timer, by setting, by direction */
extern u8 data_0209f2c0;                /* arm9 clock setting */
extern int data_0209e650;               /* arm9 RNG state */

/* int-returning, so the ROM materializes the 0/1 before testing it. */
static inline int IsInRange(s32 v, s32 lo, s32 hi) {
    return v >= lo && v <= hi;
}

static inline s32 Clamp(s32 v, s32 lo, s32 hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}

// @symbol daObjCtMecha08_c_classInit_CT_MECHA08A
extern "C" daObjCtMecha08_c *daObjCtMecha08_c_classInit_CT_MECHA08A() {
    return new daObjCtMecha08_c();
}

// @symbol daObjCtMecha08_c_classInit_CT_MECHA08B
extern "C" daObjCtMecha08_c *daObjCtMecha08_c_classInit_CT_MECHA08B() {
    return new daObjCtMecha08_c();
}

// @symbol _ZN16daObjCtMecha08_c13InitResourcesEv
/* Load both files, remember the placed position as home, and start moving in
   direction 0 with that direction's speed and timer for the current clock
   setting. */
int daObjCtMecha08_c::InitResources() {
    mModel.SetFile((BMD_File *)dExtSimpleModel_c::LoadFile(gCtMechaObj08aModelFile), 1, -1);
    UpdateModelPosAndRotY();
    UpdateClsnPosAndRot();
    mMeshCollider.SetFile((KCL_File *)dBgW_Kc::LoadFile(gCtMechaObj08aCollisionFile), mClsnMat,
                          0x199, mAngleY, data_ov035_021121b8);

    mHomePosX = mPosX;
    mHomePosY = mPosY;
    mHomePosZ = mPosZ;
    mMoveDir = 0;

    u8 setting = data_0209f2c0;
    mMoveTimer = data_ov065_0211c0d4[setting][mMoveDir][0];
    mVertSpeed = data_ov065_0211c0d0[setting][mMoveDir][0];
    mTerminalVelocity = -0x3c000;
    return 1;
}

// @symbol _ZN16daObjCtMecha08_c8BehaviorEv
/* Setting 3 parks the lift at the top of its travel. Otherwise it moves under
   its own speed, and each time the timer runs out it clamps back inside
   [mHomePosY, mHomePosY + 0x14a000], turns round, and reloads speed and timer.
   Setting 2 overrides the timer with 0xa plus a random multiple of 0x14, at most 0x6e.
   The in-range test is an int-returning inline, which is what makes the ROM
   materialize its 0/1 before branching on it. */
int daObjCtMecha08_c::Behavior() {
    if (data_0209f2c0 == CLOCK_SETTING_STOPPED) {
        mPosY = mHomePosY + LIFT_TRAVEL;
        UpdateModelPosAndRotY();
        if (IsClsnInRange(0, 0) != 0)
            UpdateClsnPosAndRot();
    } else {
        if (DecIfAbove0_Short(&mMoveTimer) == 0) {
            s32 lower, upper;

            UpdatePos(0);
            lower = mHomePosY;
            upper = lower + LIFT_TRAVEL;
            if (!IsInRange(mPosY, lower, upper)) {
                mPosY = Clamp(mPosY, lower, upper);

                mMoveDir ^= 1;

                int setting = data_0209f2c0;
                u8 d = mMoveDir;
                mMoveTimer = data_ov065_0211c0d4[setting][d][0];
                d = mMoveDir;
                mVertSpeed = data_ov065_0211c0d0[setting][d][0];
                if (data_0209f2c0 == CLOCK_SETTING_RANDOM) {
                    u32 rnd = (u32)RandomIntInternal(&data_0209e650);
                    mMoveTimer = (u16)((rnd % 6) * 0x14 + 0xa);
                }
            }
        }

        UpdateModelPosAndRotY();
        if (IsClsnInRange(0, 0) != 0)
            UpdateClsnPosAndRot();
    }
    return 1;
}

// @symbol _ZN16daObjCtMecha08_c6RenderEv
int daObjCtMecha08_c::Render() {
    mModel.Render(0);
    return 1;
}

// @symbol _ZN16daObjCtMecha08_c16CleanupResourcesEv
/* Give back exactly what InitResources took: the collider, then both files. */
int daObjCtMecha08_c::CleanupResources() {
    if (mMeshCollider.IsEnabled())
        mMeshCollider.Disable();
    gCtMechaObj08aModelFile.Release();
    gCtMechaObj08aCollisionFile.Release();
    return 1;
}

/* Static-init globals (was the handwritten __sinit_ov065_0211c9b8 shard).
 * Definition order is the retail initializer's construction order. */
Mecha08ModelFile gCtMechaObj08aModelFile(ASSET_CT_MECHA_OBJ08A_BMD);
Mecha08ModelFile gCtMechaObj08bModelFile(ASSET_CT_MECHA_OBJ08B_BMD);
Mecha08CollisionFilePtr gCtMechaObj08aCollisionFile(ASSET_CT_MECHA_OBJ08A_KCL);
Mecha08CollisionFilePtr gCtMechaObj08bCollisionFile(ASSET_CT_MECHA_OBJ08B_KCL);
