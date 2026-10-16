//cpp
/* daBDonketu_c -- Big Bully (BOSS_DONKETU), ov064 0x021174a0..0x02117978.
 * Sibling of daDonketu_c; both derive from daOts_c. The destructor is inline
 * in include/daBDonketu_c.h, so UpdateRunState is the key function
 * (_ZTV12daBDonketu_c at 0x0211b978). Source order is the reverse of the ROM.
 * Do not reorder. The factory is `new daBDonketu_c()` (was BigBully_Spawn).
 */
/* leftovers: UpdateRunState returns a pointer cast to int on the timer path;
 * that shape is what matches. func_ov064_02116110, func_ov064_0211616c and
 * func_ov064_02116bac are daOts_c workers this file calls, still named by
 * address. InitResources leaves spawnPos.y unset when DetectClsn misses.
 */

/* Runtime asset handles resolved through the overlay 0 file table. */
enum daBDonketu_c_AssetId {
    ASSET_BOSS_DONKETU_BMD = 0x02de,
    ASSET_DONKETU_DEAD_BCA = 0x02e0,
    ASSET_DONKETU_HIT_BCA = 0x02e1,
    ASSET_DONKETU_TURN_BCA = 0x02e2,
    ASSET_DONKETU_WALK_BCA = 0x02e3
};

#include "daBDonketu_c.h"
#include "daDonketu_c.h"
#include "types.h"
#include "decl_common.h"
#include "dBgCh_Gnd.h"
#include "Sound.h"
#include "SharedFilePtr.h"

/* One model handle constructs through func_02017acc and destroys through
 * func_02017ab4. Four animation handles construct through
 * SharedFilePtr::Construct and destroy through SharedFilePtr_Destruct_Anim.
 * The file table stays ROM data. */
struct DonketuModelFilePtr : SharedFilePtr {
    DonketuModelFilePtr(unsigned int fileId);
    ~DonketuModelFilePtr();
};
struct DonketuAnimFileHandle : SharedFilePtr {
    DonketuAnimFileHandle(unsigned int fileId);
    ~DonketuAnimFileHandle();
};
typedef char DonketuModelFilePtr_size_must_be_8[
    sizeof(DonketuModelFilePtr) == 8 ? 1 : -1];
typedef char DonketuAnimFileHandle_size_must_be_8[
    sizeof(DonketuAnimFileHandle) == 8 ? 1 : -1];

extern "C" {
extern s16 data_02082214[];
}

/* Factory for the BOSS_DONKETU profile. Written first so it emits last. */
// @symbol daBDonketu_c_classInit
extern "C" daBDonketu_c *daBDonketu_c_classInit()
{
    return new daBDonketu_c();
}

/* param1 bits 8..15 equal to 1 make this the boss with its three small
 * Bullies: they are spawned here, each told this actor's uniqueID, and
 * mNumBulliesKilled starts at 0. Any other setting starts it at 0xff, which
 * skips the small-Bully phase in Behavior. */
// @symbol _ZN12daBDonketu_c13InitResourcesEv
int daBDonketu_c::InitResources()
{
    int result;

    mFileTable = (s32)&data_ov064_0211b93c;
    result = InitResourcesCommon();
    mStarID = param1 & 0xf;
    mTrackStarID = (u8)TrackStar(mStarID, 2);
    mSecretSoundCounter = 0;

    if ((param1 & 0xff00) == 0x100) {
        mNumBulliesKilled = 0;

        dBgCh_Gnd ground;
        Vector3 spawnPos;
        Vector3 probe;
        int i;
        int ang;

        {
            int tz = mPosZ;
            int ty = mPosY + 0x32000;
            int tx = mPosX;
            probe.x = tx;
            probe.y = ty;
            probe.z = tz;
        }
        ground.SetObjAndPos(probe, this);

        if (ground.DetectClsn() != 0) {
            spawnPos.y = ground.clsnY;
        }

        /* Three DONKETU (actor 215), 0x5555 apart, placed by the sine table
           data_02082214 scaled by 500. */
        i = 0;
        ang = 0;
        do {
            int idx = (u16)(s16)ang >> 4;
            dActor_c *spawned;

            spawnPos.x = mPosX + data_02082214[idx * 2] * 500 - 0x64000;
            spawnPos.z = mPosZ - data_02082214[idx * 2 + 1] * 500;

            spawned = dActor_c::Spawn(0xd7, -1, spawnPos, (Vector3_16 *)0, mAreaId, -1);
            if (spawned != 0) {
                ((daDonketu_c *)spawned)->mBigBullyID = uniqueID;
            } else {
                return 0;
            }

            i++;
            ang += 0x5555;
        } while (i < 3);
    } else {
        mNumBulliesKilled = 0xff;
    }
    return result;
}

/* mNumBulliesKilled below 3: nothing to do. At 3 (all three small Bullies
 * dead) the boss drops in under gravity, and increments the count when it
 * lands. From 4 on, the shared daOts_c behavior runs. */
// @symbol _ZN12daBDonketu_c8BehaviorEv
int daBDonketu_c::Behavior()
{
    u8 killed = mNumBulliesKilled;
    if (killed >= 4) {
        if (mSecretSoundCounter != 0) {
            if (Sound::PlaySecretSound(this, &mSecretSoundCounter) != 0)
                mSecretSoundCounter = 0;
        }
        return BehaviorCommon();
    }
    if (killed == 3) {
        int speed;
        int limit;
        if (Sound::PlaySecretSound(this, &mSecretSoundCounter) != 0)
            mSecretSoundCounter = 0;
        speed = mVertSpeed + mVertAccel;
        limit = mTerminalVelocity;
        if (speed >= limit) limit = speed;
        mVertSpeed = limit;
        mPosY = mPosY + mVertSpeed;
        UpdateWMClsn(mWithMeshClsn, 0);
        if (mWithMeshClsn.IsOnGround() != 0) {
            func_0200fa8c(this, 0);
            /* Kept as the ROM-matching spelling: a pointer increment. */
            u8 *count = &mNumBulliesKilled;
            *count = *count + 1;
        }
        func_ov064_02116bac();
    }
    return 1;
}

/* Drawn once the small Bullies are dead. */
// @symbol _ZN12daBDonketu_c6RenderEv
int daBDonketu_c::Render()
{
    if (mNumBulliesKilled >= 3)
        mModelAnim.Render(0);
    return 1;
}

/* Once func_ov064_0211616c reports nonzero: poof dust, and spawn the star
 * (0x40 | mStarID) 100 units above the actor. */
// @symbol _ZN12daBDonketu_c16UpdateDeathStateEv
void daBDonketu_c::UpdateDeathState()
{
    int result = func_ov064_0211616c();
    if (result == 0)
        return;
    TriplePoofDust();
    Vector3 pos;
    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    pos.y += 0x64000;
    UntrackAndSpawnStar(*(s8 *)&mTrackStarID, (mStarID | 0x40) & 0xff, pos, 4);
}

/* Charge timer, then run. Key function: the inline destructor emits no vtable. */
// @symbol _ZN12daBDonketu_c14UpdateRunStateEv
int daBDonketu_c::UpdateRunState()
{
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
    mHorzSpeed = 0x14000;
    u16 *timer = (u16 *)&mStateTimer;
    u32 value = *timer;
    if (value > 0x23) {
        value = 0;
        *timer = 0;
    }
    return value;
}

/* Retail construction order. mwcc emits __sinit_d_a_b_donketu.cpp. */
DonketuModelFilePtr gBossDonketuModelFile(ASSET_BOSS_DONKETU_BMD);
DonketuAnimFileHandle gdaBDonketuDonketuDeadAnimationFile(ASSET_DONKETU_DEAD_BCA);
DonketuAnimFileHandle gdaBDonketuDonketuHitAnimationFile(ASSET_DONKETU_HIT_BCA);
DonketuAnimFileHandle gdaBDonketuDonketuTurnAnimationFile(ASSET_DONKETU_TURN_BCA);
DonketuAnimFileHandle gdaBDonketuDonketuWalkAnimationFile(ASSET_DONKETU_WALK_BCA);
