#ifndef DAOBJWC_OBJ05_C_H
#define DAOBJWC_OBJ05_C_H

#include "types.h"

#ifdef __cplusplus
extern "C" void *_ZN7fBase_cnwEj(unsigned size);

#include "dBgActor_c.h"

/* Wet-Dry World's Cage Lift (CAGE_LIFT 99).
 *
 * RTTI ov029:0x02113e30 names `15daObjWc_Obj05_c`; the type string at
 * 0x02113e3c is that spelling, and the sole base is dBgActor_c
 * (ov002:0x021089ec). The vtable address point is ov029:0x02113e74.
 * overlay_actors maps ov029 profile 99 to CAGE_LIFT; the registry
 * descriptor is g_profile_WC_OBJ05. Ugly RTTI names are final -- this is
 * not a compatibility alias, and it is not daObjWc_Obj02_c / Obj03_c /
 * Obj04_c / Mizu_c.
 *
 * SIZE 0x330, the factory's own literal. dBgActor_c fills 0x000..0x31f
 * (D1 restores _ZTV10dBgActor_c, destroys dBgW_KcMbg at 0x124 and Model
 * at 0x0d4, then chains to dActor_c). This class owns the state tail:
 *
 *   0x320  unk_320  -- rest height; InitResources snapshots mPosY
 *   0x324  unk_324  -- Sound::PlayLong handle
 *   0x328  unk_328  -- dwell timer in state 2, 0x6e frames
 *   0x32a  unk_32a  -- BeforeClsn helper sets; Behavior consumes
 *   0x32b  unk_32b  -- previous-frame copy of unk_32a
 *   0x32c  unk_32c  -- 0 idle, 1 descend, 2 ascend, 3 param1-bit0 locked
 *
 * LEAF: a scan of extracted/ for the word 0x02113e30 finds only this
 * class's own _ZTV-4. Nothing names it as a base, so `return new` is
 * legal and the inline destructor is safe.
 */

struct daObjWc_Obj05_c : dBgActor_c {
    s32 unk_320;                    /* 0x320 */
    s32 unk_324;                    /* 0x324 -- Sound::PlayLong handle */
    s16 unk_328;                    /* 0x328 */
    u8  unk_32a;                    /* 0x32a */
    u8  unk_32b;                    /* 0x32b */
    u8  unk_32c;                    /* 0x32c -- state selector */
    u8  pad_32d[0x3];

    /* Inline is load-bearing: out of line mwccarm emits D2, D0, D1, while
     * the ROM has D1 then D0 and no D2. */
    virtual ~daObjWc_Obj05_c() {}          /* slots 16 (D1), 17 (D0) */

    /* Overrides of fBase_c's slots 0, 3, 6 and 9. Virtualness is inherited. */
    virtual s32   InitResources();         /* slot  0 */
    virtual s32   CleanupResources();      /* slot  3 */
    virtual s32   Behavior();              /* slot  6 */
    virtual s32   Render();                /* slot  9 */

    static void *operator new(unsigned long size)
    {
        return _ZN7fBase_cnwEj((unsigned)size);
    }
};

typedef char daObjWc_Obj05_c_size_must_be_0x330[sizeof(daObjWc_Obj05_c) == 0x330 ? 1 : -1];

#endif /* __cplusplus */

#endif /* DAOBJWC_OBJ05_C_H */
