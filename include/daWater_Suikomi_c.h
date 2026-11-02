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

#include "dEnemyBase_c.h"
#include "dCcAcPos_c.h"
#include "dBgCh_Actr.h"

struct daWater_Suikomi_c : dEnemyBase_c {
    typedef int (daWater_Suikomi_c::*StateFn)();
    /* SetState runs mInit as it installs the state; Behavior runs mMain
       every frame. The one state is InitState0 / State0. */
    struct State {
        StateFn mInit; /* 0x00 */
        StateFn mMain; /* 0x08 */
    };

    dCcAcPos_c mdCcAcPos_c;   /* 0x110 */
    dBgCh_Actr mWithMeshClsn; /* 0x150 */
    State *mState;            /* 0x30c */
    u8 pad_310[0x4];
    s32 unk_314; /* 0x314 */

    /* --- vtable --- */
    virtual ~daWater_Suikomi_c();

    int Behavior();
    int CleanupResources();
    int InitResources();
    void OnPendingDestroy();
    int Render();

    /* --- non-virtual --- */
    void CheckContact();        /* ov026 0x02112280 */
    int State0();               /* ov026 0x021122b0 */
    int InitState0();           /* ov026 0x021122cc */
    int SetState(State *);      /* ov026 0x021122d4 */
    void func_ov026_02112324(); /* ov026 0x02112324, empty */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daWater_Suikomi_c_size_must_be_0x318[sizeof(daWater_Suikomi_c) == 0x318 ? 1 : -1];
#endif

#endif /* DAWATER_SUIKOMI_C_H */
