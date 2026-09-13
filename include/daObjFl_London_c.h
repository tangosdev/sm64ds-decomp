#ifndef DAOBJFL_LONDON_C_H
#define DAOBJFL_LONDON_C_H

#include "types.h"
#include "dBgActor_c.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

/**
 * Lethal Lava Land lava bridge. ov022 LAVA_BRIDGE (73), profile FL_LONDON.
 *
 * `daObjFl_London_c` is the RTTI name. Direct dBgActor_c subclass: the
 * destructor stores two vptrs. Factory allocates 0x320. Overrides
 * InitResources, CleanupResources, Behavior, Render.
 *
 * mCooldown/mFlag sit at 0x31e/0x31f in dBgActor_c's tail padding (same
 * placement as daObjRc_Guruguru_c's mAngVelY). InitResources seeds
 * mCooldown to 0xf and mFlag to 0; Behavior decrements the cooldown,
 * tilts mPrevAngleZ, and plays bank-3 SFX at the ends of each swing.
 */
struct daObjFl_London_c : dBgActor_c {
    /* The destructor is declared FIRST and defined INLINE: mwcc then emits the
     * D1/D0 pair in retail order and no homeless D2, which is what lets the
     * whole class isolate into one translation unit. */
    virtual ~daObjFl_London_c() {}         /* slots 16 (D1), 17 (D0) */

    virtual s32   InitResources();         /* slot  0 */
    virtual s32   CleanupResources();      /* slot  3 */
    virtual s32   Behavior();              /* slot  6 */
    virtual s32   Render();                /* slot  9 */

    u8  mCooldown;                         /* 0x31e */
    u8  mFlag;                             /* 0x31f */

    /* Leaf until fBase_c::operator new(unsigned long) lands (#2570).
     * MWCC has no size_t here; unsigned long is the new-expression operand. */
    static void *operator new(unsigned long size)
    {
        return _ZN7fBase_cnwEj((unsigned)size);
    }
};

typedef char daObjFl_London_c_size_must_be_0x320[sizeof(daObjFl_London_c) == 0x320 ? 1 : -1];

#endif /* DAOBJFL_LONDON_C_H */
