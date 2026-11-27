//cpp
/* daBDonketu_c -- Big Bully (BOSS_DONKETU), ov064 0x021174a0..0x02117978.
 * Sibling of daDonketu_c; both derive from daOts_c. The destructor is inline
 * in include/daBDonketu_c.h, so UpdateRunState is the key function
 * (_ZTV12daBDonketu_c at 0x0211b978). Source order is the reverse of the ROM.
 * Do not reorder. The factory is `new daBDonketu_c()` (was BigBully_Spawn).
 */
/* Notes: TurnTowardPlayer, func_ov064_0211616c and
 * UpdateModelAndShadow are daOts_c members (d_a_ots.cpp); 0211616c is
 * still named by address. InitResources leaves spawnPos.y unset when
 * DetectClsn misses, as the ROM does.
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
#include "nitro/fx/fx_trig.h"

/* One model handle constructs through ModelFile::ModelFile and destroys through
 * ModelFile::~ModelFile. Four animation handles construct through
 * AnimFile::AnimFile and destroy through AnimFile::~AnimFile.
 * The file table stays ROM data. */
struct DonketuModelFilePtr : SharedFilePtr {
    DonketuModelFilePtr(unsigned int fileId);
    ~DonketuModelFilePtr();
};
struct DonketuAnimFileHandle : SharedFilePtr {
    DonketuAnimFileHandle(unsigned int fileId);
    ~DonketuAnimFileHandle();
};
typedef char DonketuModelFilePtr_size_must_be_8[sizeof(DonketuModelFilePtr) == 8 ? 1 : -1];
typedef char DonketuAnimFileHandle_size_must_be_8[sizeof(DonketuAnimFileHandle) == 8 ? 1 : -1];

/* The Big Bully's resource block (daOts_c.h); overlay .data owns the bytes. */
extern BullyResourceConfig data_ov064_0211b93c;

/* Factory for the BOSS_DONKETU profile. Written first so it emits last. */
daBDonketu_c *daBDonketu_c_classInit() {
    return new daBDonketu_c();
}

/* param1 bits 8..15 equal to 1 make this the boss with its three small
 * Bullies: they are spawned here, each told this actor's uniqueID, and
 * mNumBulliesKilled starts at 0. Any other setting starts it at 0xff, which
 * skips the small-Bully phase in Behavior. */
int daBDonketu_c::InitResources() {
    int result;

    mFileTable = &data_ov064_0211b93c;
    result = InitResourcesCommon();
    mStarID = param1 & 0xf;
    mTrackStarID = (s8)TrackStar(mStarID, 2);
    mSecretSoundCounter = 0;

    if ((param1 & 0xff00) == 0x100) {
        mNumBulliesKilled = 0;

        dBgCh_Gnd ground;
        Vector3 spawnPos;
        Vector3 probe;
        int i;
        int ang;

        {
            int tz = mPos.z;
            int ty = mPos.y + 0x32000;
            int tx = mPos.x;
            probe.x = tx;
            probe.y = ty;
            probe.z = tz;
        }
        ground.SetObjAndPos(probe, this);

        if (ground.DetectClsn() != 0) {
            spawnPos.y = ground.clsnY;
        }

        /* Three DONKETU (actor 215), 0x5555 apart, at 500 times the sine
           and cosine of each angle. */
        i = 0;
        ang = 0;
        do {
            int idx = (u16)(s16)ang >> 4;
            dActor_c *spawned;

            spawnPos.x = mPos.x + FX_SinIdx(idx) * 500 - 0x64000;
            spawnPos.z = mPos.z - FX_CosIdx(idx) * 500;

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
int daBDonketu_c::Behavior() {
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
        speed = mSpeed.y + mVertAccel;
        limit = mTerminalVelocity;
        if (speed >= limit)
            limit = speed;
        mSpeed.y = limit;
        mPos.y = mPos.y + mSpeed.y;
        UpdateWMClsn(mWithMeshClsn, 0);
        if (mWithMeshClsn.IsOnGround() != 0) {
            func_0200fa8c(0);
            /* Kept as the ROM-matching spelling: a pointer increment. */
            mNumBulliesKilled++;
        }
        UpdateModelAndShadow();
    }
    return 1;
}

/* Drawn once the small Bullies are dead. */
int daBDonketu_c::Render() {
    if (mNumBulliesKilled >= 3)
        mAnmModel.Render(0);
    return 1;
}

/* Once func_ov064_0211616c reports nonzero: poof dust, and spawn the star
 * (0x40 | mStarID) 100 units above the actor. */
void daBDonketu_c::UpdateDeathState() {
    int result = func_ov064_0211616c();
    if (result == 0)
        return;
    TriplePoofDust();
    Vector3 pos;
    pos.x = mPos.x;
    pos.y = mPos.y;
    pos.z = mPos.z;
    pos.y += 0x64000;
    UntrackAndSpawnStar(mTrackStarID, (mStarID | 0x40) & 0xff, pos, 4);
}

/* Charge timer, then run. Key function: the inline destructor emits no vtable. */
void daBDonketu_c::UpdateRunState() {
    if (mStateTimer < 10) {
        mHorzSpeed = 0;
        if (TurnTowardPlayer(0x700))
            return;
        if (mStateTimer == 9)
            mStateTimer = 0;
    } else {
        mHorzSpeed = 0x14000;
        if (mStateTimer > 0x23)
            mStateTimer = 0;
    }
}

/* Retail construction order. mwcc emits __sinit_d_a_b_donketu.cpp. */
DonketuModelFilePtr gBossDonketuModelFile(ASSET_BOSS_DONKETU_BMD);
DonketuAnimFileHandle gdaBDonketuDonketuDeadAnimationFile(ASSET_DONKETU_DEAD_BCA);
DonketuAnimFileHandle gdaBDonketuDonketuHitAnimationFile(ASSET_DONKETU_HIT_BCA);
DonketuAnimFileHandle gdaBDonketuDonketuTurnAnimationFile(ASSET_DONKETU_TURN_BCA);
DonketuAnimFileHandle gdaBDonketuDonketuWalkAnimationFile(ASSET_DONKETU_WALK_BCA);
