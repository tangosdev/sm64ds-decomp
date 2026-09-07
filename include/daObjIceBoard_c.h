#ifndef DAOBJICEBOARD_C_H
#define DAOBJICEBOARD_C_H

#include "dBgActor_c.h"

/* daObjIceBoard_c -- the shatterable ice sheet (profile ICE_BOARD, ov018).
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
 * The destructor is declared FIRST and INLINE on purpose. Out of line mwccarm
 * emits D0 ahead of D1 and the cartridge has D1 first, which rombuild refuses;
 * declaring it first is also what makes this TU the vtable's home. */
struct daObjIceBoard_c : dBgActor_c {
    virtual ~daObjIceBoard_c() {}                 /* slots 16, 17 */

    virtual int InitResources();                  /* slot  0 */
    virtual int CleanupResources();               /* slot  3 */
    virtual int Behavior();                       /* slot  6 */
    virtual int Render();                         /* slot  9 */
    virtual void OnGroundPounded(dActor_c &other);/* slot 21 */
    virtual void OnHitByMegaChar(Player &player); /* slot 27 */
    virtual void Kill();                          /* slot 31 */
};

typedef char daObjIceBoard_c_size_must_be_0x320[sizeof(daObjIceBoard_c) == 0x320 ? 1 : -1];

#endif /* DAOBJICEBOARD_C_H */
