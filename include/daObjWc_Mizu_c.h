#ifndef DAOBJWC_MIZU_C_H
#define DAOBJWC_MIZU_C_H

#include "types.h"
#include "dBgW_KcMbg.h"

/* Derives from dBgActor_c: the destructor stores this class's vtable, then
 * dBgActor_c's -- inlined -- then destroys the dBgW_KcMbg at 0x124 and
 * the Model at 0xd4 before chaining to dActor_c. All three belong to dBgActor_c.
 * Everything this header used to restate below 0x31e was dActor_c's and
 * dBgActor_c's, and is inherited now.
 *
 * SIZE IS THE OBSERVED FIELD SPAN, rounded up. It guards this declaration; it
 * is not independent evidence about the ROM. Factory literal 840 = 0x348.
 */

#ifdef __cplusplus

#include "dBgActor_c.h"
#include "TextureTransformer.h"

extern "C" void *_ZN7fBase_cnwEj(unsigned size);

/* WDW water (profile WDW_WATER 101). ov029 RTTI:
 *   _ZTI  0x02114098  __si_class_type_info; base dBgActor_c
 *   _ZTS  0x021140a4  "14daObjWc_Mizu_c"
 *   _ZTV  0x021140dc  address point
 *   g_profile_WC_MIZU  0x021140b8
 * Ugly RTTI name is final. Historical alias: WDW_Water.
 */

struct daObjWc_Mizu_c : dBgActor_c {
    u8  pad_31e[0x2];
    TextureTransformer mTextureTransformer; /* 0x320 */
    s32 mTargetPosY;                  /* 0x334 */
    u8 mUseSpawnPosY;                 /* 0x338 -- param1 & 1; when clear InitResources overrides mPosY from the WDW water-level table data_ov029_02112b2c[setting] */
    u8  pad_339[0x3];
    s32 mSoundID;                      /* 0x33c */
    u8 mTrueAreaID;                    /* 0x340 -- stored u8; Behavior reads it signed (ldrsb) */
    u8  pad_341;
    /* Behavior adds 0x200 per frame; (u16)>>4 indexes data_02082214.
       Same sine-table shape as LavaPlank::mPhaseAngle. Was pad_341[3]
       covering this live halfword. */
    s16 mPhaseAngle;                   /* 0x342 */
    s32 mWaterHeight;                  /* 0x344 */

    /* INLINE ON PURPOSE. Out of line, mwccarm emits D0 ahead of D1 plus an
       unhomed D2; the cartridge keeps D1 at 0x021121a4 below D0 at 0x021121f0.
       Defined in the class body it yields the retail D1/D0 pair and no D2.
       First non-inline virtual below (InitResources) is then the key function,
       so this class's TU still homes _ZTV/_ZTI/_ZTS. */
    virtual ~daObjWc_Mizu_c() {}

    virtual int InitResources();       /* slot  0 */
    virtual int CleanupResources();    /* slot  3 */
    virtual int Behavior();            /* slot  6 */
    virtual int Render();              /* slot  9 */

    static void *operator new(unsigned long size) {
        return _ZN7fBase_cnwEj((unsigned)size);
    }
};

typedef char daObjWc_Mizu_c_size_must_be_0x348[sizeof(daObjWc_Mizu_c) == 0x348 ? 1 : -1];

#endif /* __cplusplus */

#endif /* DAOBJWC_MIZU_C_H */
