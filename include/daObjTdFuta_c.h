#ifndef DAOBJTDFUTA_C_H
#define DAOBJTDFUTA_C_H

#include "types.h"
#include "dBgActor_c.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

/* Tiny-Huge Island's lid ("futa"). The cartridge RTTI names this class
 * daObjTdFuta_c (HUGE_COVER, actor 106, ov032).
 *
 *   _ZTS  ov032 0x021138ac  "13daObjTdFuta_c"
 *   _ZTI  ov032 0x021138a0  __si_class_type_info; +8 -> _ZTI10dBgActor_c
 *                           (ov002 0x021089ec), so the DIRECT base is
 *                           dBgActor_c and nothing else.
 *   _ZTV  ov032 0x021138e0  the ADDRESS POINT; 32 slots, same extent as
 *                           the base. Slots 0, 3, 6, 9, 16, 17 are the
 *                           only overrides.
 *   size  0x320             the factory's own literal, which is
 *                           dBgActor_c's size: this class adds no fields.
 *
 * The adjacent 0x021139a4 table is a different class: RTTI names it
 * daObjTdWater_c (HUGE_WATER, 107).
 *
 * The destructor is declared LAST and INLINE on purpose. Class instantiation
 * via the factory's `new` emits the retail D1/D0 pair in cartridge order
 * without a separate leaf D2 body; out of line mwccarm emits D0 ahead of D1
 * and adds the D2 the ROM never carried. */
struct daObjTdFuta_c : dBgActor_c {
    virtual s32 InitResources();           /* slot  0 */
    virtual s32 CleanupResources();        /* slot  3 */
    virtual s32 Behavior();                /* slot  6 */
    virtual s32 Render();                  /* slot  9 */

    /* Leaf size_t operator new. A plain `new daObjTdFuta_c` without this
       relocates to the global `_Znwm`; this routes the factory through
       fBase_c::operator new, the call this TU's classInit actually makes. */
    static void *operator new(unsigned long size) {
        return _ZN7fBase_cnwEj((unsigned)size);
    }

    virtual ~daObjTdFuta_c() {}            /* slots 16 (D1), 17 (D0) */
};

typedef char daObjTdFuta_c_size_must_be_0x320[sizeof(daObjTdFuta_c) == 0x320 ? 1 : -1];

#endif /* DAOBJTDFUTA_C_H */
