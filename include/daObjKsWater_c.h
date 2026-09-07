#ifndef DAOBJKSWATER_C_H
#define DAOBJKSWATER_C_H

#include "dBgActor_c.h"
#include "TextureTransformer.h"

/* daObjKsWater_c -- the rising water inside the sunken ship (profile KS_MIZU,
 * ov017). It sits still until every treasure chest in the room has been opened,
 * then climbs, frame by frame, until it has risen 0x92e000 from where it began.
 *
 * WHAT THE CARTRIDGE PROVES ABOUT THE NAME AND THE SHAPE:
 *   _ZTS  ov017 0x02111bc0  "14daObjKsWater_c"
 *   _ZTI  ov017 0x02111bb4  __si_class_type_info; +8 -> _ZTI10dBgActor_c
 *                           (ov002 0x021089ec), so the DIRECT base is
 *                           dBgActor_c and nothing else.
 *   _ZTV  ov017 0x02111bf8  the ADDRESS POINT itself: the two words below it
 *                           at 0x02111bf0 are a zero offset-to-top and
 *                           &_ZTI (0x02111bb4).
 *   size  0x340             daObjKsWater_c_classInit's own literal (832).
 *                           dBgActor_c ends at 0x31e, so the four fields below
 *                           account for 0x320..0x33f exactly.
 * The coined `ShipWater` spelling this class used to carry is gone; the ROM's
 * own type string is where the class name now comes from.
 *
 * Only slots 0, 3, 6, 9, 16 and 17 of the cartridge's 34-word table point
 * inside ov017; every other slot still holds dBgActor_c's arm9 word -- there is
 * no Kill override and no collision-response override here.
 *
 * The destructor is declared FIRST and INLINE on purpose. Out of line mwccarm
 * emits D0 ahead of D1 and the cartridge has D1 first, which rombuild refuses;
 * declaring it first is also what makes this TU the vtable's home.
 */
struct daObjKsWater_c : dBgActor_c {
    virtual ~daObjKsWater_c() {}                  /* slots 16, 17 */

    virtual int InitResources();                  /* slot 0 */
    virtual int CleanupResources();               /* slot 3 */
    virtual int Behavior();                       /* slot 6 */
    virtual int Render();                         /* slot 9 */

    u8  pad_31e[0x2];
    TextureTransformer mTextureTransformer;       /* 0x320 -- scrolls the water's
                                                     surface texture every frame */
    s32 mOriginalPosY;                            /* 0x334 -- the height it starts
                                                     at, and the one the rise is
                                                     measured against */
    u8  mChestsOpen;                              /* 0x338 -- latched once every
                                                     actor-ID-0xd chest is open */
    u8  pad_339[0x3];
    s32 mSoundID;                                 /* 0x33c -- the looping rising-
                                                     water sound's handle */
};

typedef char daObjKsWater_c_size_must_be_0x340[sizeof(daObjKsWater_c) == 0x340 ? 1 : -1];

#endif /* DAOBJKSWATER_C_H */
