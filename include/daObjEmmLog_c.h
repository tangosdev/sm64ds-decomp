#ifndef DAOBJEMMLOG_C_H
#define DAOBJEMMLOG_C_H

#include "types.h"
#include "dBgActor_c.h"

/**
 * Rolling log from the ex_m_map stage (Emm; data/stage/ex_m_map in the ROM
 * file table -- not Tiny-Huge Island, whose stage dirs are tibi_deka_*).
 * Actor 174 EMM_LOG (debug table 0x0208f980);
 * ov043 multiplexes the same id as BIG_MOVING_LOG. ov052's other class is
 * daObjEmmYuka_c / EMM_YUKA (175), the SQUARE_PATH_LIFT overlay_actors lists.
 *
 * `daObjEmmLog_c` is the RTTI name. Direct dBgActor_c subclass: the
 * destructor stores two vptrs. Factory allocates 0x328. Overrides
 * InitResources, CleanupResources, Behavior, Render.
 *
 * mBobPhase sits at 0x31e in dBgActor_c's tail padding; mBasePosY /
 * mBobAmplitude follow. InitResources seeds all three; Behavior advances
 * the phase and bobs mPosY on a sine of it. Nothing writes mAngle*.
 */
struct daObjEmmLog_c : dBgActor_c {
    s16 mBobPhase;          /* 0x31e -- seeded from mAngleX, += 0x200 per Behavior */
    s32 mBasePosY;          /* 0x320 -- InitResources copies mPosY */
    s32 mBobAmplitude;      /* 0x324 -- 0x64000, or the spawn byte * 0xa000 */

    /* Inline empty dtor: mwccarm emits D1 then D0, no D2. */
    virtual ~daObjEmmLog_c() {}

    s32 InitResources();      /* slot  0 */
    s32 CleanupResources();   /* slot  3 */
    s32 Behavior();           /* slot  6 */
    s32 Render();             /* slot  9 */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjEmmLog_c_size_must_be_0x328[sizeof(daObjEmmLog_c) == 0x328 ? 1 : -1];
#endif

#endif /* DAOBJEMMLOG_C_H */
