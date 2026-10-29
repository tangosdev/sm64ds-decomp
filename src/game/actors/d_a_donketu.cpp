//cpp
/* Small Bully (DONKETU), ov064 0x02117070..0x021174a0. Derives from daOts_c.
 * The Big Bully is a sibling: InitResources clears mBigBullyID, the spawner
 * writes that actor's id, and both death paths bump its kill count.
 *
 * _ZTI 0x0211b7fc, _ZTS 0x0211b808, _ZTV 0x0211b870. The destructor is inline
 * in daDonketu_c.h: D1 0x02117070, D0 0x021170c4, no D2. g_profile_DONKETU
 * and the file table sit between the typeinfo and the vtable, so this TU
 * stays text-only. Highest address first.
 */
// Notes (measured spellings that do not match):
// - Behavior and UpdateDeathState each build the coin position in separate
//   locals. `pos.y = mPosY + 0x136000` costs 9 words of Behavior.
// - mStateTimer is read and written through a u16 pointer. A plain member
//   increment does not match UpdateRunState.
// - The four Play*Sound methods call Sound.h's func_0201267c, the
//   position-relative sound veneer, with mCamSpacePos.
// - func_ov064_02116110 and func_ov064_0211616c are daOts_c's shared steppers
//   and keep their labels with that class. data_ov064_0211b834 is the file
//   table decl_common.h declares. g_profile_DONKETU is not in this TU.

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daDonketu_c_AssetId {
    ASSET_DONKETU_BMD = 0x02df,
    ASSET_DONKETU_DEAD_BCA = 0x02e0,
    ASSET_DONKETU_HIT_BCA = 0x02e1,
    ASSET_DONKETU_TURN_BCA = 0x02e2,
    ASSET_DONKETU_WALK_BCA = 0x02e3
};

#include "daDonketu_c.h"
#include "common.h"
#include "daBDonketu_c.h"
#include "decl_common.h"
#include "SharedFilePtr.h"
#include "Sound.h"

/* The arm9 RNG step has no shared header; data_0209e650 is its state. */
extern "C" int RandomIntInternal(int *seed);
extern int data_0209e650;

/* File-scope resource handles: the ctor/dtor are the ROM's SharedFilePtr
 * veneer pairs (func_02017acc / func_02017ab4 for the model,
 * SharedFilePtr::Construct / SharedFilePtr_Destruct_Anim for the four
 * animation handles), spelled through declared-only subclasses so the
 * static initializer names the real entry points. */
struct DonketuModelFilePtr : SharedFilePtr {
    DonketuModelFilePtr(u32 fileID);
    ~DonketuModelFilePtr();
};
struct DonketuAnimationFileHandle : SharedFilePtr {
    DonketuAnimationFileHandle(u32 fileID);
    ~DonketuAnimationFileHandle();
};

/* DONKETU factory. Written first so it emits last. */
// @symbol daDonketu_c_classInit
extern "C" daDonketu_c *daDonketu_c_classInit() {
    return new daDonketu_c();
}

// @symbol _ZN11daDonketu_c13InitResourcesEv
int daDonketu_c::InitResources() {
    mBigBullyID = 0;
    mFileTable = (int)&data_ov064_0211b834;
    InitResourcesCommon();
}

// @symbol _ZN11daDonketu_c8BehaviorEv
/* 0 runs the shared worker, 1 is already handled, 2 drops a coin (actor
 * 0x120, subtype 2) and bumps the Big Bully's kill count. The same drop is
 * written again in UpdateDeathState. The coin faces mPrevAngleY + 0x8000
 * plus a 10-bit roll, and launches upward.
 */
int daDonketu_c::Behavior() {
    int outcome = UpdateKillByInvincibleChar(mWithMeshClsn, mAnmModel, 3);
    if (outcome != 0) {
        if (outcome == 2) {
            int y = mPosY;
            int yoff = 0x136000;
            int z = mPosZ;
            int x = mPosX;
            int sum = y + yoff;
            Vector3 pos;
            pos.x = x;
            pos.y = sum;
            pos.z = z;
            int roll = RandomIntInternal(&data_0209e650);
            int yrot = mPrevAngleY + 0x8000 + (int)(((u32)roll >> 8) & 0x3ff);
            Vector3_16 rot;
            rot.x = 0;
            rot.z = 0;
            rot.y = (s16)yrot;

            dActor_c *coin = dActor_c::Spawn(0x120, 2, pos, &rot, mAreaId, -1);
            if (coin) {
                coin->mHorzSpeed = 0xa000;
                coin->mSpeedX = 0;
                coin->mVertSpeed = 0x50000;
                coin->mSpeedZ = 0;
            }
            dActor_c *spawner = dActor_c::FindWithID(mBigBullyID);
            if (spawner) {
                daBDonketu_c *bigBully = (daBDonketu_c *)spawner;
                bigBully->mNumBulliesKilled++;
            }
        }
        return 1;
    }
    return BehaviorCommon();
}

// @symbol _ZN11daDonketu_c16UpdateDeathStateEv
/* The state-4 half of the same drop Behavior does for the invincible-char kill.
 * func_ov064_0211616c is the shared daOts_c death-animation step: it returns 0
 * while the bully is still falling apart, and the coin goes out on the frame it
 * finally returns non-zero. */
void daDonketu_c::UpdateDeathState() {
    if (func_ov064_0211616c() == 0)
        return;
    int pz = mPosZ;
    int py = mPosY + 0x136000;
    int px = mPosX;
    Vector3 pos;
    pos.x = px;
    pos.y = py;
    pos.z = pz;
    int roll = RandomIntInternal(&data_0209e650);
    Vector3_16 rot;
    s16 ang = (s16)(mPrevAngleY + 0x8000 + (((u32)roll >> 8) & 0x3ff));
    rot.x = 0;
    rot.z = 0;
    rot.y = ang;
    dActor_c *coin = dActor_c::Spawn(0x120, 2, pos, &rot, mAreaId, -1);
    if (coin) {
        coin->mHorzSpeed = 0xa000;
        coin->mSpeedX = 0;
        coin->mVertSpeed = 0x50000;
        coin->mSpeedZ = 0;
    }
    dActor_c *spawner = dActor_c::FindWithID(mBigBullyID);
    if (spawner) {
        daBDonketu_c *bigBully = (daBDonketu_c *)spawner;
        bigBully->mNumBulliesKilled++;
    }
}

// @symbol _ZN11daDonketu_c14UpdateRunStateEv
/* Below ten ticks the bully stands still and plays the wind-up through the
 * shared daOts_c stepper; from ten on it charges at 0xf000 and the cycle timer
 * wraps at 0x2c. The Big Bully's override is the same shape with a faster charge
 * and a shorter cycle. */
int daDonketu_c::UpdateRunState() {
    if (*(u16 *)&mStateTimer < 0xa) {
        mHorzSpeed = 0;
        int result = func_ov064_02116110(0x700);
        if (result != 0)
            return result;
        u16 *timer = (u16 *)&mStateTimer;
        if (*timer == 9)
            *timer = 0;
        return (int)timer;
    }
    mHorzSpeed = 0xf000;
    u16 *timer = (u16 *)&mStateTimer;
    u32 value = *timer;
    if (value > 0x2c) {
        value = 0;
        *timer = 0;
    }
    return value;
}

// @symbol _ZN11daDonketu_c13PlayStepSoundEv
/* Frames 4 and 7 of the walk cycle are the two footfalls. */
void daDonketu_c::PlayStepSound() {
    if (mAnmModel.WillHitFrame(4) == 0) {
        if (mAnmModel.WillHitFrame(7) == 0)
            return;
    }
    func_0201267c(0xca, &mCamSpacePos);
}

// @symbol _ZN11daDonketu_c12PlayHitSoundEv
void daDonketu_c::PlayHitSound() {
    func_0201267c(0xcb, &mCamSpacePos);
}

// @symbol _ZN11daDonketu_c17PlayShellHitSoundEv
void daDonketu_c::PlayShellHitSound() {
    func_0201267c(0xc9, &mCamSpacePos);
}

// @symbol _ZN11daDonketu_c14PlayDeathSoundEv
void daDonketu_c::PlayDeathSound() {
    func_0201267c(0xc8, &mCamSpacePos);
}

// @symbol __sinit_d_a_donketu.cpp
/* The small Bully's model and animation handles, in the sinit call order the
 * data_ov064_0211b834 file table repeats. */
DonketuModelFilePtr gDonketuModelFile(ASSET_DONKETU_BMD);
DonketuAnimationFileHandle gDonketuDeadAnimationFile(ASSET_DONKETU_DEAD_BCA);
DonketuAnimationFileHandle gDonketuHitAnimationFile(ASSET_DONKETU_HIT_BCA);
DonketuAnimationFileHandle gDonketuTurnAnimationFile(ASSET_DONKETU_TURN_BCA);
DonketuAnimationFileHandle gDonketuWalkAnimationFile(ASSET_DONKETU_WALK_BCA);
