//cpp
/* daBDonketu_c -- the Big Bully (BOSS_DONKETU), ov064 0x021174a0..0x0211791c.
 *
 * One translation unit, seven functions, the way the cartridge's own build had
 * it. This replaces seven one-function shards. Their bodies are unchanged; what
 * changed is that their local declarations are collected into the single extern
 * block below, and that the destructor is now inline in include/daBDonketu_c.h.
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
 * The factory is NOT in this TU. daBDonketu_c_classInit begins at 0x0211791c,
 * immediately past this entry's end, and keeps its own source.
 *
 * FUNCTION ORDER IS THE REVERSE OF THE ROM'S -- mwccarm 2004/b56 emits one
 * .text section per function in reverse source order, so the highest-address ROM
 * function is written first. Do not reorder.
 */

#include "daBDonketu_c.h"
#include "types.h"
#include "decl_common.h"
#include "dBgCh_Gnd.h"

extern "C" {
extern int func_ov064_0211616c(void *self);
extern int _ZN5Sound15PlaySecretSoundEP8dActor_cPt(void* a, u16* p);
extern s16 data_02082214[];
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 6 -- _ZN12daBDonketu_c13InitResourcesEv, 0x02117784, size 0x198 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBDonketu_c13InitResourcesEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
int daBDonketu_c::InitResources()
{
    int saved;

    *(void**)((char*)&mFileTable) = &data_ov064_0211b93c;
    saved = InitResourcesCommon();
    mStarID = param1 & 0xf;
    unk_3fd = (u8)TrackStar(mStarID, 2);
    mSecretSoundCounter = 0;

    if ((param1 & 0xff00) == 0x100) {
        mNumBulliesKilled = 0;

        dBgCh_Gnd ground;
        Vector3 pos;
        Vector3 v;
        int i;
        int ang;

        {
            int tz = mPosZ;
            int ty = mPosY + 0x32000;
            int tx = mPosX;
            v.x = tx;
            v.y = ty;
            v.z = tz;
        }
        ground.SetObjAndPos(v, this);

        if (ground.DetectClsn() != 0) {
            pos.y = ground.clsnY;
        }

        i = 0;
        ang = 0;
        do {
            int idx = (u16)(s16)ang >> 4;
            dActor_c* spawned;

            pos.x = mPosX + data_02082214[idx * 2] * 500 - 0x64000;
            pos.z = mPosZ - data_02082214[idx * 2 + 1] * 500;

            spawned = dActor_c::Spawn(0xd7, -1, pos, (Vector3_16 *)0, mAreaId, -1);
            if (spawned != 0) {
                *(int*)((char*)spawned + 0x3fc) = uniqueID;
            } else {
                return 0;
            }

            i++;
            ang += 0x5555;
        } while (i < 3);

        goto done;
    }

    mNumBulliesKilled = 0xff;
done:
    return saved;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 5 -- _ZN12daBDonketu_c8BehaviorEv, 0x02117684, size 0x100 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBDonketu_c8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
/* recovered: named members + shared header, real C++ method */
int daBDonketu_c::Behavior()
{
    u8 s = mNumBulliesKilled;
    if (s >= 4) {
        if (mSecretSoundCounter != 0) {
            if (_ZN5Sound15PlaySecretSoundEP8dActor_cPt(((char*)this), (u16*)((char*)&mSecretSoundCounter)) != 0)
                mSecretSoundCounter = 0;
        }
        return BehaviorCommon();
    }
    if (s == 3) {
        int t;
        int m;
        int* p;
        if (_ZN5Sound15PlaySecretSoundEP8dActor_cPt(((char*)this), (u16*)((char*)&mSecretSoundCounter)) != 0)
            mSecretSoundCounter = 0;
        t = mVertSpeed + mVertAccel;
        m = mTerminalVelocity;
        if (t >= m) m = t;
        mVertSpeed = m;
        p = (int*)(((int)((char*)this) + 0x60));
        *p = *p + mVertSpeed;
        UpdateWMClsn(mWithMeshClsn, 0);
        if (mWithMeshClsn.IsOnGround() != 0) {
            u8* q;
            func_0200fa8c(((char*)this), 0);
            q = (u8*)(((int)((char*)this) + 0x3fe));
            *q = *q + 1;
        }
        func_ov064_02116bac(((char*)this));
    }
    return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 4 -- _ZN12daBDonketu_c6RenderEv, 0x0211764c, size 0x38 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBDonketu_c6RenderEv
/* recovered: named members + shared header, real C++ method */
int daBDonketu_c::Render()
{
  if(mNumBulliesKilled >= 3)
    mModelAnim.Render(0);
  return 1;
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 3 -- _ZN12daBDonketu_c16UpdateDeathStateEv, 0x021175cc, size 0x80 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBDonketu_c16UpdateDeathStateEv
void daBDonketu_c::UpdateDeathState()
{
    int result = func_ov064_0211616c(this);
    if (result == 0)
        return;
    TriplePoofDust();
    Vector3 pos;
    pos.x = mPosX;
    pos.y = mPosY;
    pos.z = mPosZ;
    pos.y += 0x64000;
    UntrackAndSpawnStar(*(s8 *)&unk_3fd, (mStarID | 0x40) & 0xff, pos, 4);
}

/* -------------------------------------------------------------------------- */
/* ROM ordinal 2 -- _ZN12daBDonketu_c14UpdateRunStateEv, 0x0211755c, size 0x70 */
/* -------------------------------------------------------------------------- */
// @symbol _ZN12daBDonketu_c14UpdateRunStateEv
int daBDonketu_c::UpdateRunState()
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
    mHorzSpeed = 0x14000;
    u16 *timer = (u16 *)&mStateTimer;
    u32 value = *timer;
    if (value > 0x23) {
        value = 0;
        *timer = 0;
    }
    return value;
}
