#ifndef DAOBJWC_OBJ05_C_H
#define DAOBJWC_OBJ05_C_H

#include "types.h"

#ifdef __cplusplus

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
 * (D1 restores _ZTV10dBgActor_c, destroys dBgW_KcMbg at 0x124 and dExtSimpleModel_c
 * at 0x0d4, then chains to dActor_c). This class owns the state tail:
 *
 *   0x320  mRestPosY      -- rest height; InitResources snapshots mPosY
 *   0x324  mSoundHandle   -- Sound::PlayLong handle
 *   0x328  mDwellTimer    -- dwell timer in state 2, 0x6e frames
 *   0x32a  mPlayerOnTop   -- BeforeClsn helper sets; Behavior consumes
 *   0x32b  mPrevPlayerOnTop -- previous-frame copy of mPlayerOnTop
 *   0x32c  mState         -- 0 idle, 1 descend, 2 dwell+ascend, 3 locked
 *
 * LEAF: a scan of extracted/ for the word 0x02113e30 finds only this
 * class's own _ZTV-4. Nothing names it as a base, so `return new` is
 * legal and the inline destructor is safe.
 */

struct daObjWc_Obj05_c : dBgActor_c {
    /* InitResources copies mPosY here; state 1 descends to
     * mRestPosY - 0xa28000 and state 2 climbs back and snaps to it. */
    s32 mRestPosY; /* 0x320 */
    /* Sound::PlayLong's returned handle, fed back as its first argument
     * on every frame states 1 and 2 play the lift loop (sound 0x8d). */
    s32 mSoundHandle; /* 0x324 */
    /* Zeroed on entering state 2 (and by InitResources); state 2 counts
     * it up as u16 and starts the ascent once it reaches 0x6e. */
    s16 mDwellTimer; /* 0x328 */
    /* OnStoodOn, called through the mesh collider's stood-on callback, sets
     * it to 1 when the touching actor's actorID is PLAYER (0xbf); Behavior
     * reads it and clears it at the end of every frame. */
    u8 mPlayerOnTop; /* 0x32a */
    /* Behavior's state 0 copies mPlayerOnTop here; it starts the descent
     * only on the rising edge (mPlayerOnTop set, mPrevPlayerOnTop clear). */
    u8 mPrevPlayerOnTop; /* 0x32b */
    /* Behavior's switch: 0 idle, 1 descend, 2 dwell then ascend,
     * 3 locked (InitResources picks it when param1 bit 0 is set). */
    u8 mState;       /* 0x32c */
    u8 pad_32d[0x3]; /* 0x32d -- alignment tail, untouched */

    /* Inline is load-bearing: out of line mwccarm emits D2, D0, D1, while
     * the ROM has D1 then D0 and no D2. */
    virtual ~daObjWc_Obj05_c() {} /* slots 16 (D1), 17 (D0) */

    /* Overrides of fBase_c's slots 0, 3, 6 and 9. Virtualness is inherited. */
    virtual s32 InitResources();    /* slot  0 */
    virtual s32 CleanupResources(); /* slot  3 */
    virtual s32 Behavior();         /* slot  6 */
    virtual s32 Render();           /* slot  9 */

    void OnStoodOn(dActor_c *);                                         /* ov029 0x02111e40 */
    static void StoodOnCallback(void *, daObjWc_Obj05_c *, dActor_c *); /* ov029 0x02111e60 */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjWc_Obj05_c_size_must_be_0x330[sizeof(daObjWc_Obj05_c) == 0x330 ? 1 : -1];
#endif

#endif /* __cplusplus */

#endif /* DAOBJWC_OBJ05_C_H */
