#ifndef DAOBJWC_OBJ02_C_H
#define DAOBJWC_OBJ02_C_H
#include "types.h"

#ifdef __cplusplus
extern "C" void *_ZN7fBase_cnwEj(unsigned size);

#include "dBgActor_c.h"

/* Wet-Dry World's Arrow Lift (ARROW_LIFT 96).
 *
 * RTTI ov029:0x02113cac names `15daObjWc_Obj02_c`; the type string at
 * 0x02113cb8 is that spelling, and the sole base is dBgActor_c
 * (ov002:0x021089ec). The vtable address point is ov029:0x02113cf0.
 * overlay_actors maps ov029 profile 96 to ARROW_LIFT; the registry
 * descriptor is g_profile_WC_OBJ02. Ugly RTTI names are final -- this is
 * not a compatibility alias.
 *
 * SIZE 0x328, the factory's own literal. dBgActor_c fills 0x000..0x31f
 * (D1 restores _ZTV10dBgActor_c, destroys dBgW_KcMbg at 0x124 and Model
 * at 0x0d4, then chains to dActor_c). This class owns the eight-byte
 * state tail:
 *
 *   0x320  mTravelDist  -- Behavior accumulates mHorzSpeed; wraps at 0x177000
 *   0x324  mStateTimer  -- dwell in states 2/4, 0x3c frames
 *   0x326  mTriggered   -- BeforeClsn helper sets; Behavior consumes
 *   0x327  mState       -- 0 idle, 1/3 travel, 2/4 turn-and-wait
 */

struct daObjWc_Obj02_c : dBgActor_c {
    s32 mTravelDist;          /* 0x320 */
    u16 mStateTimer;          /* 0x324 */
    u8  mTriggered;           /* 0x326 */
    u8  mState;               /* 0x327 */

    /* Inline is load-bearing: out of line mwccarm emits D2, D0, D1, while
     * the ROM has D1 then D0 and no D2. */
    virtual ~daObjWc_Obj02_c() {}

    /* Overrides of fBase_c's slots 0, 3, 6 and 9. Virtualness is inherited. */
    int InitResources();
    int CleanupResources();
    int Behavior();
    int Render();

    static void *operator new(unsigned long size)
    {
        return _ZN7fBase_cnwEj((unsigned)size);
    }
};

typedef char daObjWc_Obj02_c_size_must_be_0x328[sizeof(daObjWc_Obj02_c) == 0x328 ? 1 : -1];

#endif /* __cplusplus */

#endif
