#ifndef DAOBJC0_SWITCH_C_H
#define DAOBJC0_SWITCH_C_H

#include "types.h"
#include "dBgActor_c.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

/* daObjC0_Switch_c -- Castle basement switch pillar (profile C0_SWITCH /
 * SWITCH_PILLAR, actor 34, ov012). ov012 also has BASEMENT_WATER(35); this
 * class is the pillar, not the water.
 *
 * WHAT THE CARTRIDGE PROVES ABOUT THE NAME AND THE SHAPE:
 *   _ZTS  ov012 0x0211230c  "16daObjC0_Switch_c"
 *   _ZTI  ov012 0x02112300  __si_class_type_info; +8 -> _ZTI10dBgActor_c
 *                           (ov002 0x021089ec), so the DIRECT base is
 *                           dBgActor_c and nothing else.
 *   _ZTV  ov012 0x02112344  the ADDRESS POINT itself: V-8 is a zero
 *                           offset-to-top and V-4 is &_ZTI (0x02112300).
 *   size  0x320             daObjC0_Switch_c_classInit's own literal, which
 *                           is also dBgActor_c's own size. mPressed sits at
 *                           0x31e, in the base's tail padding (same placement
 *                           as daObjRc_Guruguru_c's mAngVelY).
 * The coined `SwitchPillar` spelling this class used to carry is gone; the
 * ROM's own type string is where the class name now comes from.
 *
 * Seven of the 32 slots point inside ov012 -- 0, 3, 6, 9, 16, 17 and 21 -- and
 * every other slot still holds dBgActor_c's arm9 word, so nothing else is
 * overridden.
 *
 * mPressed is this pillar's own pressed state. InitResources and
 * OnGroundPounded both guard on and set it; OnGroundPounded no-ops if already
 * set, then returns at the first non-this actorID-0x22 pillar and, if that
 * one is already pressed, sets a shared flag in data_0209caa0[2].
 *
 * The destructor is declared LAST and INLINE on purpose. Class instantiation
 * via the factory's `new` emits the retail D1/D0 pair in cartridge order
 * without a separate leaf D2 body; out of line mwccarm emits D0 ahead of D1
 * and adds the D2 the ROM never carried. */
struct daObjC0_Switch_c : dBgActor_c {
    u8  mPressed;            /* 0x31e */

    virtual s32   InitResources();         /* slot  0 */
    virtual s32   CleanupResources();      /* slot  3 */
    virtual s32   Behavior();              /* slot  6 */
    virtual s32   Render();                /* slot  9 */
    virtual void  OnGroundPounded(dActor_c &other); /* slot 21 */

    /* Leaf size_t operator new. A plain `new daObjC0_Switch_c` without this
       relocates to the global `_Znwm`; this routes the factory through
       fBase_c::operator new, the call this TU's classInit actually makes. */
    static void *operator new(unsigned long size) {
        return _ZN7fBase_cnwEj((unsigned)size);
    }

    virtual ~daObjC0_Switch_c() {}          /* slots 16 (D1), 17 (D0) */
};

typedef char daObjC0_Switch_c_size_must_be_0x320[sizeof(daObjC0_Switch_c) == 0x320 ? 1 : -1];

#endif /* DAOBJC0_SWITCH_C_H */
