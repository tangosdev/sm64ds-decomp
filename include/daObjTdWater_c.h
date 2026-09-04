#ifndef DAOBJTDWATER_C_H
#define DAOBJTDWATER_C_H

#include "types.h"
#include "dBgActor_c.h"
#include "TextureTransformer.h"

/* The TD_WATER profile's water surface: a large animated water plane with a
 * collision mesh under it, whose texture scrolls for as long as the actor is
 * alive, and which is only spawned when the level's event bit is clear.
 *
 * TWO WITNESSES, and they close on each other:
 *
 *   daObjTdWater_c_classInit (historical alias HugeWater_Spawn)
 *                 fBase_c::operator new(820 = 0x334), dBgActor_c::dBgActor_c(),
 *                 stores _ZTV14daObjTdWater_c.
 *   ~daObjTdWater_c   its own vptr, then dBgActor_c's -- inlined, because
 *                 dBgActor_c's destructor is defined in its class body -- then
 *                 dBgActor_c's dBgW_KcMbg at 0x124 and Model at 0xd4, then
 *                 dActor_c. All three are the BASE's.
 *
 * SIZE 0x334 is the factory's own literal, and the last member closes exactly
 * on it: dBgActor_c rounds to 0x320 and TextureTransformer is 0x14 bytes.
 *
 * THE VTABLE was diffed slot by slot against _ZTV10dBgActor_c. Only the slots
 * declared below differ; every other slot holds the base's own word and is
 * inherited, so it is deliberately not redeclared here.
 */
struct daObjTdWater_c : dBgActor_c {
    /* dBgActor_c's last field ends at 0x31e and its size rounds to 0x320, so
       the two bytes of its tail padding are unused here -- unlike
       daObjRc_Guruguru_c::mAngVelY, this class's first member is 4-byte
       aligned and starts at 0x320. */
    u8 pad_31e[0x2];

    /* THIS CLASS'S OWN. InitResources hands it the BTA at ov032 0x02112f64 and
       Behavior sets its rate to 0x1000 and advances it every frame -- that is
       the whole scroll. */
    TextureTransformer mTextureTransformer;    /* 0x320 */

    /* MEASURED -- INLINE ON PURPOSE. The class TU is the only place these
       two are emitted; with the body out of line mwcc emits D0 ahead of D1
       and the ROM has D1 first (rombuild refuses the object outright). An
       inline body also drops the D2 variant the cartridge never carried. */
    virtual ~daObjTdWater_c() {}               /* slots 16 (D1), 17 (D0) */

    virtual s32   InitResources();         /* slot  0 */
    virtual s32   CleanupResources();      /* slot  3 */
    virtual s32   Behavior();              /* slot  6 */
    virtual s32   Render();                /* slot  9 */
};

typedef char daObjTdWater_c_size_must_be_0x334[sizeof(daObjTdWater_c) == 0x334 ? 1 : -1];

#endif /* DAOBJTDWATER_C_H */
