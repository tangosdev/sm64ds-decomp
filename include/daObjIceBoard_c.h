#ifndef DAOBJICEBOARD_C_H
#define DAOBJICEBOARD_C_H

#include "dBgActor_c.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

/* daObjIceBoard_c -- Cool Cool Mountain's shatterable ice sheet (profile
 * ICE_BOARD, actor 295, ov018).
 *
 * WHAT THE CARTRIDGE PROVES ABOUT THE NAME AND THE SHAPE:
 *   _ZTS  ov018 0x02113afc  "15daObjIceBoard_c"
 *   _ZTI  ov018 0x02113af0  __si_class_type_info; +8 -> _ZTI10dBgActor_c
 *                           (ov002 0x021089ec), so the DIRECT base is
 *                           dBgActor_c and nothing else.
 *   _ZTV  ov018 0x02113b34  the ADDRESS POINT itself: V-8 is a zero
 *                           offset-to-top and V-4 is &_ZTI (0x02113af0).
 *                           32 slots; slot 31 is this class's own Kill.
 *   size  0x320             daObjIceBoard_c_classInit's own literal, which is
 *                           also dBgActor_c's own size: this class adds no
 *                           fields, and the destructor confirms it -- it
 *                           destroys only the inherited dBgW_KcMbg and Model
 *                           before chaining to dActor_c.
 * The coined `IceSheet` spelling this class used to carry is gone; the ROM's
 * own type string is where the class name now comes from.
 *
 * Nine of the 32 slots point inside ov018 and are listed below; every other
 * slot still holds dBgActor_c's arm9 word, so nothing else is overridden.
 *
 * The destructor is declared LAST and INLINE on purpose. Class instantiation
 * via the factory's `new` emits the retail D1/D0 pair in cartridge order
 * without a separate leaf D2 body; out of line mwccarm emits D0 ahead of D1
 * and adds the D2 the ROM never carried. */
struct daObjIceBoard_c : dBgActor_c {
    virtual int InitResources();                  /* slot  0 */
    virtual int CleanupResources();               /* slot  3 */
    virtual int Behavior();                       /* slot  6 */
    virtual int Render();                         /* slot  9 */
    virtual void OnGroundPounded(dActor_c &other);/* slot 21 */
    virtual void OnHitByMegaChar(Player &player); /* slot 27 */
    virtual void Kill();                          /* slot 31 */

    /* Leaf size_t operator new. A plain `new daObjIceBoard_c` without this
       relocates to the global `_Znwm`; this routes the factory through
       fBase_c::operator new, the call this TU's classInit actually makes. */
    static void *operator new(unsigned long size) {
        return _ZN7fBase_cnwEj((unsigned)size);
    }

    virtual ~daObjIceBoard_c() {}                 /* slots 16, 17 */
};

typedef char daObjIceBoard_c_size_must_be_0x320[sizeof(daObjIceBoard_c) == 0x320 ? 1 : -1];

#endif /* DAOBJICEBOARD_C_H */
