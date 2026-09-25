#ifndef DAOBJBKKILLERDAI_C_H
#define DAOBJBKKILLERDAI_C_H

#include "types.h"
#include "dBgActor_c.h"

struct Player;

/* Bullet Bill launcher. The tail word is the unique ID of the bill it
 * last fired: InitResources clears it, Behavior hands it to FindWithID.
 *
 * BASE: dBgActor_c, direct -- _ZTI18daObjBkKillerdai_c at ov079 0x02127f70
 * names _ZTI10dBgActor_c.
 *
 * TWO WITNESSES, and they close on each other:
 *
 *   daObjBkKillerdai_c_classInit  fBase_c::operator new(804 = 0x324),
 *                 dBgActor_c::dBgActor_c(), stores _ZTV18daObjBkKillerdai_c,
 *                 then the members below in this order.
 *   ~daObjBkKillerdai_c  the same members destroyed in reverse, then ~dBgActor_c.
 *
 * SIZE 0x324 is the factory's own literal, and the last member closes exactly on it.
 *
 * THE VTABLE was diffed slot by slot against _ZTV10dBgActor_c. Only the slots declared
 * below differ; every other slot holds the base's own word and is inherited, so it
 * is deliberately not redeclared here.
 */
struct daObjBkKillerdai_c : dBgActor_c {
    u32 mSpawnedUniqueID; /* 0x320 */

    virtual ~daObjBkKillerdai_c();              /* slots 16 (D1), 17 (D0) */

    virtual s32 InitResources();                /* slot  0 */
    virtual s32 CleanupResources();             /* slot  3 */
    virtual s32 Behavior();                     /* slot  6 */
    virtual s32 Render();                       /* slot  9 */
    virtual void OnHitByMegaChar(Player &player); /* slot 27 */
    /* dBgActor_c's own slot, overridden here: _ZTV18daObjBkKillerdai_c+0x7c relocates
       to 0x02126e58 while _ZTV10dBgActor_c+0x7c relocates to _ZN10dBgActor_c4KillEv. An
       override, so it adds no slot and no field. */
    virtual void Kill();                        /* slot 31 */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjBkKillerdai_c_size_must_be_0x324[sizeof(daObjBkKillerdai_c) == 0x324 ? 1 : -1];
#endif

#endif /* DAOBJBKKILLERDAI_C_H */
