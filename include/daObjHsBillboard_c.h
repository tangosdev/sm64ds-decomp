#ifndef DAOBJHSBILLBOARD_C_H
#define DAOBJHSBILLBOARD_C_H

#include "types.h"
#include "Model.h"

/* ov031 -- the slide's four decoration billboards. Actor ids 0x12e..0x131
 * (blue smiley, orange smiley, yellow star, silver star) share ONE
 * implementation and ONE vtable; InitResources reads the actor id to pick
 * which of the four SharedFilePtrs in data_ov031_02111424 to load.
 *
 * MEASURED from extracted/dsd/arm9_overlays/ov031.bin, base 0x021111a0:
 *   _ZTS18daObjHsBillboard_c  0x021118f4  bytes spell "18daObjHsBillboard_c"
 *   _ZTI18daObjHsBillboard_c  0x021118e8  +4 -> _ZTS, +8 -> 0x0208e390 = _ZTI8dActor_c
 *   _ZTV18daObjHsBillboard_c  0x02111984  V-8 = 0, V-4 = &_ZTI  (the address point)
 *
 * THE DIRECT BASE IS dActor_c, not dBgActor_c -- the RTTI base pointer says so
 * and the 0x128 allocation agrees.
 *
 * Vtable diff against dActor_c: only slots 0, 3, 9, 16 and 17 hold words inside
 * ov031. Every other slot of 0..19 still points into arm9, so there is NO
 * Behavior override at slot 6 -- the billboards never tick.
 *
 * SM64DS RTTI names the implementation daObjHsBillboard_c. The reconstructed factory
 * daObjHsBillboard_c_classInit_HS_MOON (historical alias SlideDecorationSilverStar_Spawn) installs this class's
 * cartridge vtable; the reconstructed profile global g_profile_HS_MOON
 * (historical alias SlideDecorationSilverStar_SpawnInfo) is its registry descriptor.
 *
 * SM64DS RTTI names the implementation daObjHsBillboard_c. The reconstructed factory
 * daObjHsBillboard_c_classInit_HS_STAR (historical alias SlideDecorationYellowStar_Spawn) installs this class's
 * cartridge vtable; the reconstructed profile global g_profile_HS_STAR
 * (historical alias SlideDecorationYellowStar_SpawnInfo) is its registry descriptor.
 *
 * SM64DS RTTI names the implementation daObjHsBillboard_c. The reconstructed factory
 * daObjHsBillboard_c_classInit_HS_Y_STAR (historical alias SlideDecorationOrangeSmiley_Spawn) installs this class's
 * cartridge vtable; the reconstructed profile global g_profile_HS_Y_STAR
 * (historical alias SlideDecorationOrangeSmiley_SpawnInfo) is its registry descriptor.
 *
 * SM64DS RTTI names the implementation daObjHsBillboard_c. The reconstructed factory
 * daObjHsBillboard_c_classInit_HS_B_STAR (historical alias SlideDecorationBlueSmiley_Spawn) installs this class's
 * cartridge vtable; the reconstructed profile global g_profile_HS_B_STAR
 * (historical alias SlideDecorationBlueSmiley_SpawnInfo) is its registry descriptor.
 */

#ifdef __cplusplus

#include "dActor_c.h"

struct daObjHsBillboard_c : dActor_c {
    u8    pad_0d0[0x4];  /* 0x0d0 */
    Model mModel;        /* 0x0d4 -- constructed by all four spawn factories */
    u8    mVariant;      /* 0x124 -- index into data_ov031_02111424 */

    /* --- vtable --- */
    /* MEASURED -- INLINE ON PURPOSE, and declared first, so this TU is the
       vtable's home. Both ROM bodies are empty; an inline body also drops the
       D2 variant the cartridge never carried. */
    virtual ~daObjHsBillboard_c() {}  /* slots 16 (D1), 17 (D0) */

    /* declared in reverse of ROM address order, as the TU emits them */
    int InitResources();              /* slot  0 -- 0x021112a8 */
    int Render();                     /* slot  9 -- 0x02111280 */
    int CleanupResources();           /* slot  3 -- 0x02111254 */
};

typedef char daObjHsBillboard_c_size_must_be_0x128[sizeof(daObjHsBillboard_c) == 0x128 ? 1 : -1];

#endif /* __cplusplus */

#endif /* DAOBJHSBILLBOARD_C_H */
