#ifndef DAOBJMCWATER_C_H
#define DAOBJMCWATER_C_H

#include "dBgActor_c.h"
#include "TextureTransformer.h"

/* daObjMcWater_c -- the water plane inside the castle (profile MC_WATER, ov009).
 * It is a flat, scrolling sheet with a collision mesh on it: the moat and the
 * basement pool the player can swim in. It does not move on its own. Its whole
 * per-frame job is to keep the surface texture sliding at a fixed rate.
 *
 * WHAT THE CARTRIDGE PROVES ABOUT THE NAME AND THE SHAPE:
 *   _ZTS  ov009 0x021139e0  "14daObjMcWater_c"
 *   _ZTI  ov009 0x021139d4  __si_class_type_info; +8 -> _ZTI10dBgActor_c
 *                           (ov002 0x021089ec), so the DIRECT base is
 *                           dBgActor_c and nothing else.
 *   _ZTV  ov009 0x02113a18  the ADDRESS POINT itself: the two words below it
 *                           at 0x02113a10 are a zero offset-to-top and
 *                           &_ZTI (0x021139d4). The factory's literal pool
 *                           word at 0x02111dc0 holds exactly 0x02113a18.
 *   size  0x338             daObjMcWater_c_classInit's own `mov r0, #0x338`
 *                           at 0x02111d90 (824). dBgActor_c ends at 0x31e, so
 *                           the TextureTransformer at 0x320 (0x14 bytes) and
 *                           four trailing bytes account for the remainder.
 * The coined `CastleWater` spelling this class used to carry is gone; the ROM's
 * own type string is where the class name now comes from.
 *
 * Only slots 0, 3, 6, 9, 16 and 17 of the cartridge's 32-word table point
 * inside ov009; every other slot still holds dBgActor_c's arm9 word, including
 * slot 31 (Kill, 0x020ee55c). There is no OnPendingDestroy override here --
 * unlike its two ov009 neighbours Bird and daObjMc_Metalnet_c, which both have
 * one.
 *
 * The destructor is declared FIRST and INLINE on purpose. Out of line mwccarm
 * emits D0 ahead of D1 and the cartridge has D1 first, which rombuild refuses;
 * declaring it first is also what makes this TU the vtable's home.
 */
struct daObjMcWater_c : dBgActor_c {
    virtual ~daObjMcWater_c() {}                  /* slots 16, 17 */

    virtual int InitResources();                  /* slot 0 */
    virtual int CleanupResources();               /* slot 3 */
    virtual int Behavior();                       /* slot 6 */
    virtual int Render();                         /* slot 9 */

    u8 pad_31e[0x2];
    TextureTransformer mTexTransformer;           /* 0x320 -- slides the water's
                                                     surface texture; Behavior
                                                     re-forces its speed to
                                                     0x1000 every frame */
    u8 pad_334[0x4];                              /* 0x334 -- nothing in the
                                                     eight matched bodies reads
                                                     or writes it; only the
                                                     allocation literal proves
                                                     it is there */
};

typedef char daObjMcWater_c_size_must_be_0x338[sizeof(daObjMcWater_c) == 0x338 ? 1 : -1];

#endif /* DAOBJMCWATER_C_H */
