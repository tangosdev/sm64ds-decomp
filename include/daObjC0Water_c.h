#ifndef DAOBJC0WATER_C_H
#define DAOBJC0WATER_C_H

#include "dBgActor_c.h"
#include "TextureTransformer.h"

/* daObjC0Water_c -- Castle basement water (profile C0_WATER / BASEMENT_WATER,
 * actor 35, ov012). It sits still until the switch pillars set the shared
 * group flag in data_0209caa0[2], then sinks (mPosY -= 0x5000) to mLoweredY
 * and publishes that height through data_0209f32c.
 *
 * WHAT THE CARTRIDGE PROVES ABOUT THE NAME AND THE SHAPE:
 *   _ZTS  ov012 0x021123d0  "14daObjC0Water_c"
 *   _ZTI  ov012 0x021123c4  __si_class_type_info; +8 -> _ZTI10dBgActor_c
 *                           (ov002 0x021089ec), so the DIRECT base is
 *                           dBgActor_c and nothing else.
 *   _ZTV  ov012 0x02112408  the ADDRESS POINT itself: the two words below it
 *                           at 0x02112400 are a zero offset-to-top and
 *                           &_ZTI (0x021123c4). Do not move this address.
 *   size  0x340             daObjC0Water_c_classInit's own literal (832).
 *                           dBgActor_c ends at 0x31e, so the four fields below
 *                           account for 0x320..0x33f exactly.
 * Ugly RTTI name is final. The coined `BasementWater` spelling this class used
 * to carry is gone; the ROM's own type string is where the class name now
 * comes from. overlay_actors.md: ov012 BASEMENT_WATER(35).
 *
 * Only slots 0, 3, 6, 9, 16 and 17 of the cartridge's table point inside
 * ov012; every other slot still holds dBgActor_c's word -- there is no Kill
 * override and no collision-response override here.
 *
 * The destructor is declared FIRST and INLINE on purpose. Out of line mwccarm
 * emits D0 ahead of D1 and the cartridge has D1 first; defined in the class
 * body, with the factory's `new` instantiating the class, it yields the retail
 * D1/D0 pair and no leaf D2. First non-inline virtual below (InitResources)
 * is then the key function, so this class's TU still homes _ZTV/_ZTI/_ZTS.
 */

struct daObjC0Water_c : dBgActor_c {
    virtual ~daObjC0Water_c() {}                  /* slots 16, 17 */

    virtual int InitResources();                  /* slot 0 */
    virtual int CleanupResources();               /* slot 3 */
    virtual int Behavior();                       /* slot 6 */
    virtual int Render();                         /* slot 9 */

    u8  pad_31e[0x2];
    TextureTransformer mTextureTransformer;       /* 0x320 -- scrolls the water's
                                                     surface texture every frame */
    s32 mLoweredY;                                /* 0x334 -- drain target:
                                                     InitResources stores
                                                     mPosY - 0x4b0000 */
    u32 mSoundID;                                 /* 0x338 -- looping drain
                                                     sound handle */
    u16 mSoundTimer;                              /* 0x33c -- PlaySecretSound
                                                     counter while draining */
    u8  mWasJustDrained;                          /* 0x33e -- latched once the
                                                     flag is set and the water
                                                     starts sinking */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daObjC0Water_c_size_must_be_0x340[sizeof(daObjC0Water_c) == 0x340 ? 1 : -1];
#endif

#endif /* DAOBJC0WATER_C_H */
