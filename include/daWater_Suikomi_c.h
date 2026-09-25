#ifndef DAWATER_SUIKOMI_C_H
#define DAWATER_SUIKOMI_C_H

#include "types.h"

/* Derives from dEnemyBase_c: the destructor stores this class's vtable, then the
 * base's, then destroys whatever the base owns before chaining further up.
 * Everything this header used to restate below 0x110 belonged to the
 * chain above and is inherited now.
 *
 * SIZE IS THE OBSERVED FIELD SPAN, rounded up. It guards this declaration; it
 * is not independent evidence about the ROM.
 *
 * SM64DS RTTI names the implementation daWater_Suikomi_c. The reconstructed factory
 * daWater_Suikomi_c_classInit (historical alias WaterSuction_Spawn) installs this class's
 * cartridge vtable; the reconstructed profile global g_profile_WATER_SUIKOMI
 * (historical alias WaterSuction_SpawnInfo) is its registry descriptor.
 */

#ifdef __cplusplus

#include "dEnemyBase_c.h"
#include "dCcAcPos_c.h"
#include "dBgCh_Actr.h"

struct daWater_Suikomi_c : dEnemyBase_c {
    dCcAcPos_c mdCcAcPos_c;/* 0x110 */
    dBgCh_Actr mWithMeshClsn;       /* 0x150 */
    u8  pad_30c[0x8];
    s32 unk_314;                      /* 0x314 */

    /* --- vtable --- */
    virtual ~daWater_Suikomi_c();

    int Behavior();
    int CleanupResources();
    int InitResources();
    void OnPendingDestroy();
    int Render();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daWater_Suikomi_c_size_must_be_0x318[sizeof(daWater_Suikomi_c) == 0x318 ? 1 : -1];
#endif

#else

/* The C spelling of the same object, flat. Kept because the D0 file is a C
   translation unit that reads these fields, and D0 is compiler-generated so it
   can never be migrated. Same arrangement as include/ShadowModel.h. */
struct daWater_Suikomi_c {
    u8  pad_000[0x8];
    s32 param1;            /* 0x008 */
    u8  pad_00c[0x80];
    s16 mAngleX;            /* 0x08c */
    s16 mAngleY;            /* 0x08e */
    s16 mAngleZ;            /* 0x090 */
    s16 mPrevAngleX;            /* 0x092 */
    s16 mPrevAngleY;            /* 0x094 */
    s16 mPrevAngleZ;            /* 0x096 */
    u8  pad_098[0x68];
    u8  mStateTimer;            /* 0x100 */
    u8  pad_101[0xf];
    /* dCcAcPos_c member, named by the class's own destructor calling
       dCcAcPos_c's D1 at +0x110 -- a relocation the ROM build
       checks. Was a u8 marker. [src/_ZN17daWater_Suikomi_cD1Ev.cpp] */
    dCcAcPos_c mdCcAcPos_c;            /* 0x110 */
    u8  mWithMeshClsn;            /* 0x150 */
    u8  pad_151[0x1c3];
    s32 unk_314;            /* 0x314 */
};

#endif /* __cplusplus */

#endif /* DAWATER_SUIKOMI_C_H */
