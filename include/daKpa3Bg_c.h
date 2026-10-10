/* MEASURED, from the cartridge -- this class's name is the ROM's own, not a coined one.
 *
 *   _ZTS  ov060 0x0211a954  "10daKpa3Bg_c"
 *   _ZTI  ov060 0x0211a948  __si_class_type_info; +8 -> _ZTI10dBgActor_c
 *                           (0x021089ec), so the DIRECT base is dBgActor_c.
 *   _ZTV  ov060 0x0211a9b0  the address point; 0x0211a9ac holds &_ZTI10daKpa3Bg_c.
 *                           No 17BowserSkyPlatform string exists in the overlay.
 *   size  0x32c             daKpa3Bg_c_classInit's own literal (812).
 *
 * Overridden slots, by diffing the table against dBgActor_c's: 0
 * (InitResources, 0x021182b0), 3 (CleanupResources, 0x021181e8), 6 (Behavior,
 * 0x02118254), 9 (Render, 0x0211822c), 16/17 (D1 0x02117d1c, D0 0x02117d60).
 * The tree used to call this class BowserSkyPlatform. That coined vtable row
 * was the same address as this one.
 *
 * The twelve bytes past dBgActor_c are this class's own:
 *
 *     mKoopaUniqueId   0x320  uniqueID of the daKpa actor (actor id 0x117)
 *     mTimer           0x324  counts while mActive is set
 *     mPhase           0x326  counts in func_ov060_02117db8; destruction past 0x12c
 *     mState           0x328  index into the data_ov060_0211b1ac handler table
 *     mVariant         0x329  param1 & 0xf, index of the model/collision pair
 *     mActive          0x32a
 *     mTouchedKoopa    0x32b  set by func_ov060_021183cc when the other actor is 0x117
 *
 * src/actors/daKpa3Bg_c.cpp licenses 0x02117d1c..0x02118438: D1 through
 * InitResources, then func_ov060_021183cc, its collider veneer
 * func_ov060_021183f4 and daKpa3Bg_c_classInit at 0x02118408. The factory
 * (historical alias BowserSkyPlatform_Spawn) installs _ZTV10daKpa3Bg_c for
 * g_profile_KOOPA3BG (historical alias BowserSkyPlatform_SpawnInfo).
 */
#ifndef DAKPA3BG_C_H
#define DAKPA3BG_C_H
#include "types.h"

#ifdef __cplusplus

#include "dBgActor_c.h"

struct daKpa3Bg_c : dBgActor_c {
    s32 mKoopaUniqueId;   /* 0x320 */
    u16 mTimer;           /* 0x324 */
    u16 mPhase;           /* 0x326 */
    u8  mState;           /* 0x328 */
    u8  mVariant;         /* 0x329 */
    u8  mActive;          /* 0x32a */
    u8  mTouchedKoopa;    /* 0x32b */

    /* Out of line in the cpp, under defer_codegen off: one definition emits
       D1 (0x02117d1c) then D0 (0x02117d60). The homeless D2 is deadstripped. */
    virtual ~daKpa3Bg_c();                 /* slots 16 (D1), 17 (D0) */

    virtual s32 InitResources();           /* slot  0 */
    virtual s32 CleanupResources();        /* slot  3 */
    virtual s32 Behavior();                /* slot  6 */
    virtual s32 Render();                  /* slot  9 */

    /* The mState handlers. Declared here so &daKpa3Bg_c::func_ov060_... is a
       pointer-to-member constant for the data_ov060_0211b1ac table; the TU
       defines them as free functions under these same mangled spellings. */
    void func_ov060_021181b4();
    void func_ov060_021180e0();
    void func_ov060_02117db8();
};

#ifndef SM64DS_PLATFORM_PC
typedef char daKpa3Bg_c_size_must_be_0x32c[sizeof(daKpa3Bg_c) == 0x32c ? 1 : -1];
#endif

#else

struct daKpa3Bg_c {
    u8  pad_000[0x320];
    s32 mKoopaUniqueId;   /* 0x320 */
    u16 mTimer;           /* 0x324 */
    u16 mPhase;           /* 0x326 */
    u8  mState;           /* 0x328 */
    u8  mVariant;         /* 0x329 */
    u8  mActive;          /* 0x32a */
    u8  mTouchedKoopa;    /* 0x32b */
};

#endif /* __cplusplus */

#endif
