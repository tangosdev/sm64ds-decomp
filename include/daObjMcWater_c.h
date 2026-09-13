#ifndef DAOBJMCWATER_C_H
#define DAOBJMCWATER_C_H

#include "dBgActor_c.h"
#include "TextureTransformer.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

/* CASTLE_WATER (338) -- ov009/daObjMcWater_c, the castle water plane.
 * The moat outside and the flooded basement inside are the same actor.
 *
 * ov009 is mixed (DOCK_POLE / CASTLE_WATER / METAL_NET / FLAG / BIRD).
 * RTTI names this class daObjMcWater_c; the debug table names CASTLE_WATER.
 * This is the water plane, not daObjMc_Metalnet_c / daMcFlag_c / Bird.
 *
 *   _ZTS  ov009 0x021139e0  "14daObjMcWater_c"
 *   _ZTI  ov009 0x021139d4  __si_class_type_info; base dBgActor_c
 *                           (ov002 0x021089ec)
 *   _ZTV  ov009 0x02113a18  address point
 *   size  0x338             factory literal at 0x02111d90
 *
 * Only slots 0, 3, 6, 9, 16 and 17 of the 32-word table point inside ov009;
 * every other slot still holds dBgActor_c's arm9 word, including slot 31
 * (Kill, 0x020ee55c). No OnPendingDestroy override.
 *
 * dBgActor_c ends at 0x31e (sizeof 0x320). TextureTransformer at 0x320
 * (0x14 bytes) plus four trailing bytes make 0x338.
 */

struct daObjMcWater_c : dBgActor_c {
    u8 unk_31e[0x2];
    TextureTransformer mTexTransformer; /* 0x320 -- Behavior re-forces
                                           speed to 0x1000 every frame */
    u8 unk_334[0x4];                    /* 0x334 -- allocation tail; nothing
                                           in the eight matched bodies reads
                                           or writes it */

    /* INLINE ON PURPOSE. Out of line, mwccarm emits D0 ahead of D1 plus an
       unhomed D2; the cartridge keeps D1 at 0x02111a70 below D0 at 0x02111abc.
       Defined in the class body it yields the retail D1/D0 pair and no D2.
       First non-inline virtual below (InitResources) is then the key function,
       so this class's TU still homes _ZTV/_ZTI/_ZTS. */
    virtual ~daObjMcWater_c() {}

    virtual int InitResources();       /* slot  0 */
    virtual int CleanupResources();    /* slot  3 */
    virtual int Behavior();            /* slot  6 */
    virtual int Render();              /* slot  9 */

    static void *operator new(unsigned long size) {
        return _ZN7fBase_cnwEj((unsigned)size);
    }
};

typedef char daObjMcWater_c_size_must_be_0x338[sizeof(daObjMcWater_c) == 0x338 ? 1 : -1];

#endif /* DAOBJMCWATER_C_H */
