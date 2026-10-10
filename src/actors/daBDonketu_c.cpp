//cpp
/* daBDonketu_c -- the Big Bully (BOSS_DONKETU), ov064 0x021174a0..0x02117978.
 *
 * One translation unit, eight functions, the way the cartridge's own build had
 * it. It replaces seven one-function shards whose bodies are unchanged; their
 * local declarations are collected into the single extern block below, and the
 * destructor is inline in include/daBDonketu_c.h.
 *
 * daBDonketu_c derives from daOts_c, the shared Bully base whose own promoted TU
 * sits immediately below this one in the same overlay (0x02115ee0..0x02117070).
 * It is daDonketu_c's SIBLING, not its subclass: both derive from daOts_c. The
 * small Bullies this class spawns in InitResources report their own deaths back
 * through mNumBulliesKilled, and that count is the only coupling between them.
 *
 * THIS TU OWNS THE CLASS VTABLE. The destructor is inline in the class body, so
 * the key function is UpdateRunState -- the first virtual this class declares
 * out of line -- and that anchors _ZTV12daBDonketu_c (ov064 public address point
 * 0x0211b978, storage 0x0211b970) here, together with the typeinfo and
 * type-name records it drags in for the whole ancestor chain. None of that data
 * lies inside this entry's licensed .text range, and it could not be licensed as
 * one interval even if it were: the cartridge puts g_profile_BOSS_DONKETU and
 * the file table between this class's typeinfo pair at 0x0211b904 and the vtable
 * storage at 0x0211b970. So dsd keeps supplying the cartridge's own bytes and
 * production isolation discards the emitted duplicates; the manifest's
 * compiler_only_output block licenses each one at its measured ROM home, exactly
 * as the already-promoted daOts_c does for its own vtable.
 *
 * Inlining the destructor is also what puts D1 and D0 in cartridge order.
 * Written out of line, mwcc emits the synthesized D0 ahead of the written D1 and
 * the licensed .text is no longer ROM-ascending; inlining additionally deletes
 * the homeless D2 that no module gives a symbol to.
 *
 * The registry factory daBDonketu_c_classInit (0x0211791c) is the last
 * function, `new daBDonketu_c()`; the unit is 0x021174a0..0x02117978, eight
 * functions.
 *
 * FUNCTION ORDER IS THE REVERSE OF THE ROM'S -- mwccarm 2004/b56 emits one
 * .text section per function in reverse source order, so the highest-address ROM
 * function is written first. Do not reorder.
 *
 * Known limits:
 * - UpdateRunState returns a pointer cast to int on one path; the source keeps
 *   the shape that reproduces the ROM.
 * - func_ov064_02116110, func_ov064_0211616c and func_ov064_02116bac are
 *   daOts_c workers that still carry ROM-address names.
 * - InitResources leaves spawnPos.y unset when the ground probe finds nothing.
 */

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
    unsigned int words[2];
    DonketuModelFilePtr(unsigned int fileId);
    ~DonketuModelFilePtr();
};
struct DonketuAnimFileHandle : SharedFilePtr {
    unsigned int words[2];
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

/* ROM ordinal 7 -- daBDonketu_c_classInit, 0x0211791c, size 0x5c. Written
 * first so reverse-order emission puts it last. Reconstructed source-style
 * name: SM64DS proves daBDonketu_c through RTTI, allocation size, vtable
 * identity, and the BOSS_DONKETU registry profile; later EAD lineage supplies
 * classInit. Exact original spelling is not preserved. Historical alias:
 * BigBully_Spawn. */
// @symbol daBDonketu_c_classInit
extern "C" daBDonketu_c *daBDonketu_c_classInit()
{
    return new daBDonketu_c();
}

/* ROM ordinal 6, 0x02117784, size 0x198.
 * param1 bits 8..15 equal to 1 make this the boss with its three small
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

/* ROM ordinal 5, 0x02117684, size 0x100.
 * mNumBulliesKilled below 3: nothing to do. At 3 (all three small Bullies
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

/* ROM ordinal 4, 0x0211764c, size 0x38. Drawn once the count reaches 3. */
// @symbol _ZN12daBDonketu_c6RenderEv
int daBDonketu_c::Render()
{
    if (mNumBulliesKilled >= 3)
        mModelAnim.Render(0);
    return 1;
}

/* ROM ordinal 3, 0x021175cc, size 0x80.
 * Once func_ov064_0211616c reports nonzero: poof dust, and spawn the star
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

/* ROM ordinal 2, 0x0211755c, size 0x70. */
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

/* Retail construction order. mwcc emits __sinit_daBDonketu_c.cpp. */
DonketuModelFilePtr data_ov064_0211c6cc(0x2de);
DonketuAnimFileHandle data_ov064_0211c6d4(0x2e0);
DonketuAnimFileHandle data_ov064_0211c6c4(0x2e1);
DonketuAnimFileHandle data_ov064_0211c6dc(0x2e2);
DonketuAnimFileHandle data_ov064_0211c6e4(0x2e3);
