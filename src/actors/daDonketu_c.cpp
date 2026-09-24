//cpp
/* daDonketu_c -- the small Bully (DONKETU), ov064 0x02117070..0x02117444.
 *
 * One translation unit, ten functions, the way the cartridge's own build had
 * it; the destructor is inline in include/daDonketu_c.h.
 *
 * daDonketu_c derives from daOts_c, the shared Bully base whose own promoted TU
 * sits immediately below this one in the same overlay (0x02115ee0..0x02117070).
 * daBDonketu_c, the Big Bully, is this class's SIBLING, not its parent: both
 * derive from daOts_c. The one thing that couples them is mBigBullyID --
 * InitResources zeroes it, the spawner writes the Big Bully's unique actor ID
 * into it, and both of the death paths below look that actor up and bump the
 * kill count it keeps.
 *
 * THIS TU OWNS THE CLASS VTABLE. Measured, not assumed: tools/tubuild.py verify
 * reports this one object emitting _ZTV11daDonketu_c, this class's own
 * _ZTI/_ZTS pair, and the typeinfo and type-name records for the whole
 * daOts_c/dEnemyBase_c/dActor_c/dBase_c/fBase_c ancestor chain. None of that
 * data lies inside this entry's licensed .text range, and it could not be
 * licensed as one interval even if it were: the cartridge puts g_profile_DONKETU
 * (0x0211b818) and the file table data_ov064_0211b834 between this class's
 * typeinfo pair (_ZTI 0x0211b7fc, _ZTS 0x0211b808) and the vtable storage at
 * 0x0211b868 -- public address point 0x0211b870, two words in. So this
 * promotion stays text-only: dsd keeps supplying the cartridge's own bytes and
 * production isolation discards the emitted duplicates, exactly as the
 * already-promoted sibling TUs ov064/daOts_c and ov064/daBDonketu_c do for
 * their own vtables. The manifest's compiler_only_output block licenses each one
 * at its measured ROM home.
 *
 * Inlining the destructor is what puts D1 and D0 in cartridge order. Written out
 * of line, mwcc emits the synthesized D0 ahead of the written D1 and the
 * licensed .text is no longer ROM-ascending; inlining restores D1 0x02117070,
 * D0 0x021170c4 and additionally deletes the homeless D2 that no module gives a
 * symbol to. All ten functions still byte-match either way.
 *
 * The factory is NOT in this TU. daDonketu_c_classInit begins at 0x02117444,
 * immediately past this entry's end, and keeps its own source.
 *
 * FUNCTION ORDER IS THE REVERSE OF THE ROM'S -- mwccarm 2004/b56 emits one
 * .text section per function in reverse source order, so the highest-address ROM
 * function is written first. Do not reorder.
 */

#include "daDonketu_c.h"
#include "common.h"
#include "daBDonketu_c.h"
#include "decl_common.h"

/* ROM symbols with no header of their own. func_ov064_02116110,
 * func_ov064_0211616c and data_ov064_0211b834 are deliberately absent:
 * include/decl_common.h already declares all three, and the shards' own
 * re-declarations of the first two are dropped in favour of it. */
extern "C" {
extern void func_0201267c(u32 soundID, const Vector3 *pos);
extern int RandomIntInternal(int *seed);
extern int data_0209e650;
}

// @symbol _ZN11daDonketu_c13InitResourcesEv
int daDonketu_c::InitResources()
{
    mBigBullyID = 0;
    mFileTable = (int)&data_ov064_0211b834;
    InitResourcesCommon();
}

// @symbol _ZN11daDonketu_c8BehaviorEv
/* The bully's whole update is delegated: dEnemyBase_c::UpdateKillByInvincibleChar
 * decides, from the mesh collision and the animation, whether anything happened
 * this frame. 0 means nothing did and the shared behaviour worker runs; 1 means
 * handled; 2 is the death case, and only that arm has a body. That arm is the
 * same coin drop UpdateDeathState performs, written out again rather than
 * shared -- the cartridge has both bodies whole, one at 0x02117310 and one at
 * 0x02117220, with no call between them.
 *
 * On death the bully drops a coin (actor 0x120, subtype 2) 0x136000 above
 * itself, facing its own heading turned 0x8000 (180 degrees) plus a random
 * 10-bit spread, then gives the coin an upward launch: 0xa000 of vertical speed
 * and 0x50000 of terminal velocity, with the two acceleration slots zeroed.
 * Finally it looks up the actor whose unique ID sits in mBigBullyID and bumps
 * that Big Bully's kill count.
 *
 * mAreaId is passed to Spawn as the area, which is what pins the s8 read;
 * mPrevAngleY (not mAngleY) is the heading the coin inherits.
 *
 * THE POSITION TEMPORARIES ARE LOAD-BEARING. Collapsing them into
 * `pos.y = mPosY + 0x136000` and friends costs 9 words of 69: the ROM reads y,
 * then z, then x, and materialises the offset into its own register before the
 * add. Measured, not assumed -- the sibling substitutions in the same sweep (the
 * angle temporary, and the `(long long)` round-trip on the tally) were both free
 * and are gone.
 */
int daDonketu_c::Behavior()
{
    int outcome = UpdateKillByInvincibleChar(mWithMeshClsn, mModelAnim, 3);
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
                coin->unk_0a4 = 0;
                coin->mVertSpeed = 0x50000;
                coin->unk_0ac = 0;
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
void daDonketu_c::UpdateDeathState()
{
    if (func_ov064_0211616c(this) == 0)
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
        coin->unk_0a4 = 0;
        coin->mVertSpeed = 0x50000;
        coin->unk_0ac = 0;
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
int daDonketu_c::UpdateRunState()
{
    if (*(u16 *)&mStateTimer < 0xa) {
        mHorzSpeed = 0;
        int result = func_ov064_02116110((char *)this, 0x700);
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
void daDonketu_c::PlayStepSound()
{
    if (mModelAnim.WillHitFrame(4) == 0) {
        if (mModelAnim.WillHitFrame(7) == 0)
            return;
    }
    func_0201267c(0xca, (const Vector3 *)&mCamSpacePosX);
}

// @symbol _ZN11daDonketu_c12PlayHitSoundEv
void daDonketu_c::PlayHitSound()
{
    func_0201267c(0xcb, (const Vector3 *)&mCamSpacePosX);
}

// @symbol _ZN11daDonketu_c17PlayShellHitSoundEv
void daDonketu_c::PlayShellHitSound()
{
    func_0201267c(0xc9, (const Vector3 *)&mCamSpacePosX);
}

// @symbol _ZN11daDonketu_c14PlayDeathSoundEv
void daDonketu_c::PlayDeathSound()
{
    func_0201267c(0xc8, (const Vector3 *)&mCamSpacePosX);
}
