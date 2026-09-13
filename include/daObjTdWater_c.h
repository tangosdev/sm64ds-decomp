#ifndef DAOBJTDWATER_C_H
#define DAOBJTDWATER_C_H

#include "types.h"
#include "dBgActor_c.h"
#include "TextureTransformer.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

/* Huge water plane (profile HUGE_WATER / TD_WATER 107). ov032 also has
 * HUGE_COVER(106) / daObjTdFuta_c.
 *
 * WHAT THE CARTRIDGE PROVES ABOUT THE NAME AND THE SHAPE:
 *   _ZTS  ov032 0x0211396c  "14daObjTdWater_c"
 *   _ZTI  ov032 0x02113960  __si_class_type_info; +8 -> _ZTI10dBgActor_c
 *                           (ov002 0x021089ec), so the DIRECT base is
 *                           dBgActor_c and nothing else.
 *   _ZTV  ov032 0x021139a4  the ADDRESS POINT itself: V-8 is a zero
 *                           offset-to-top, V-4 is &_ZTI, V+0 is slot 0
 *                           (InitResources, 0x021127f0).
 *   g_profile_TD_WATER      ov032 0x02113980
 *   size  0x334             the factory's own literal: dBgActor_c's 0x320
 *                           plus TextureTransformer at 0x320 (0x14 bytes).
 * Ugly RTTI name is final. Historical alias: HugeWater.
 *
 * THE VTABLE was diffed slot by slot against _ZTV10dBgActor_c. Only the
 * slots declared below differ; every other slot holds the base's own word
 * and is inherited, so it is deliberately not redeclared here.
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

    /* Leaf operator new. Parameter is size_t (unsigned long on this
       compiler). `return new` relocates to `_Znwm` without this. */
    static void *operator new(unsigned long size) {
        return _ZN7fBase_cnwEj((unsigned)size);
    }
};

typedef char daObjTdWater_c_size_must_be_0x334[sizeof(daObjTdWater_c) == 0x334 ? 1 : -1];

#endif /* DAOBJTDWATER_C_H */
