#ifndef DAOBJEMMLOG_C_H
#define DAOBJEMMLOG_C_H

#include "types.h"
#include "dBgActor_c.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

/**
 * Tiny-Huge Island rolling log. Actor 174 EMM_LOG (debug table 0x0208f980);
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
    /* The destructor pair spelled as two plain virtuals on the host, plus
       the non-virtual destructor declaration the src/ definitions need; the
       whole ruling is in include/ModelBase.h. An override takes its base's
       slots, so these carry the SAME TWO NAMES the base declares -- a fresh
       name would append a slot instead of claiming one. */
#ifdef _MSC_VER
    virtual void Destructor1();   /* D1 */
    virtual void Destructor0();   /* D0 */
    ~daObjEmmLog_c() {}   /* no slot */
#else
    virtual ~daObjEmmLog_c() {}   /* D1 and D0 */
#endif

    s32 InitResources();      /* slot  0 */
    s32 CleanupResources();   /* slot  3 */
    s32 Behavior();           /* slot  6 */
    s32 Render();             /* slot  9 */

    /* Leaf until fBase_c::operator new(unsigned long) lands (#2570).
     * MWCC has no size_t here; unsigned long is the new-expression operand. */
    static void *operator new(size_t size)
    {
        return _ZN7fBase_cnwEj((unsigned)size);
    }
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjEmmLog_c_size_must_be_0x328[sizeof(daObjEmmLog_c) == 0x328 ? 1 : -1];
#endif

#endif /* DAOBJEMMLOG_C_H */
