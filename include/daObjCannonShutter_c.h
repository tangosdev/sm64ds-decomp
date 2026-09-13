#ifndef DAOBJCANNONSHUTTER_C_H
#define DAOBJCANNONSHUTTER_C_H

#include "types.h"

/* Cannon Shutter (CANNON_SHUTTER 14).
 *
 * RTTI ov002:0x02109cf0 names the class daObjCannonShutter_c
 * (_ZTS at 0x02109cfc is `20daObjCannonShutter_c`); overlay_actors and the
 * debug table name the profile CANNON_SHUTTER. Base is dBgActor_c
 * (RTTI edge ov002:0x021089ec). The factory spelling
 * daObjCannonShutter_c_classInit (historical alias daObjCannonShutter_c_Spawn)
 * and g_profile_CANNON_SHUTTER (historical alias daObjCannonShutter_c_SpawnInfo)
 * are Tier B reconstructions -- evidence-bounded proposals, not recovered
 * SM64DS symbols.
 *
 * SIZE 0x330. dBgActor_c members end at 0x31e and sizeof rounds to 0x320;
 * pad_31e reuses that tail padding so mHomePosX lands at 0x320. pad_31e is
 * not live -- it is alignment, not unk_31e.
 *
 * dBgActor_c.h FIRST so common.h beats Model.h on Matrix4x3.
 */

#ifdef __cplusplus

#include "dBgActor_c.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

struct daObjCannonShutter_c : dBgActor_c {
    u8  pad_31e[0x2];
    s32 mHomePosX;                      /* 0x320 */
    s32 mHomePosY;                      /* 0x324 */
    s32 mHomePosZ;                      /* 0x328 */
    u8 mOpening;                       /* 0x32c */
    u8 mOpenPhase;                       /* 0x32d */
    u8 mCannonOpen;                       /* 0x32e */

    /* INLINE IS LOAD-BEARING. Out of line, mwccarm emits D0 before D1
       (cartridge is 0x020bc8f4 D1 then 0x020bc938 D0) plus a D2 with no ROM
       home. Empty body: this class adds no member with a destructor; the
       vptr store and dBgActor_c's two member teardowns are synthesised. */
    virtual ~daObjCannonShutter_c() {}

    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();

    /* Leaf operator new until #2570 puts the same allocator on fBase_c.
       Parameter is size_t (unsigned long on this compiler). `return new`
       relocates to `_Znwm` without this. */
    static void *operator new(unsigned long size) {
        return _ZN7fBase_cnwEj((unsigned)size);
    }
};

typedef char daObjCannonShutter_c_size_must_be_0x330[sizeof(daObjCannonShutter_c) == 0x330 ? 1 : -1];

#endif /* __cplusplus */

#endif /* DAOBJCANNONSHUTTER_C_H */
