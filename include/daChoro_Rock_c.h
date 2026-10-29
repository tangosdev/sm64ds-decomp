#ifndef DACHORO_ROCK_C_H
#define DACHORO_ROCK_C_H

#include "types.h"

/* Derives from dEnemyBase_c: the destructor stores this class's vtable, then the
 * base's, then destroys whatever the base owns before chaining further up.
 * Everything this header used to restate below 0x110 belonged to the
 * chain above and is inherited now.
 *
 * SIZE IS THE OBSERVED FIELD SPAN, rounded up. It guards this declaration; it
 * is not independent evidence about the ROM.
 *
 * SM64DS RTTI names the implementation daChoro_Rock_c. The reconstructed
 * factory daChoro_Rock_c_classInit (historical alias
 * MontyMoleRock_Spawn) constructs it for the CHORO_ROCK
 * registry profile.
 */

#include "dEnemyBase_c.h"
#include "dExtSimpleModel_c.h"
#include "dCcAc_c.h"
#include "dBgCh_Actr.h"

/* Actor ID (symbols/actor_debug_names.tsv): 311 CHORO_ROCK. */
enum {
    daChoro_Rock_ACTOR_ID = 0x137
};

struct daChoro_Rock_c : dEnemyBase_c {
    dExtSimpleModel_c mModel; /* 0x110 */
    dCcAc_c mdCcAc_c;/* 0x160 */
    dBgCh_Actr mWithMeshClsn;       /* 0x194 */
    u8 mIsSmall;                       /* 0x350 -- param1 & 1; a big rock (0) breaks into two small ones (1) on landing */

    /* --- vtable --- */
    virtual ~daChoro_Rock_c();

    virtual s32 Behavior();
    virtual s32 CleanupResources();
    virtual s32 InitResources();
    virtual s32 Render();
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char daChoro_Rock_c_size_must_be_0x354[sizeof(daChoro_Rock_c) == 0x354 ? 1 : -1];
#endif

#endif /* DACHORO_ROCK_C_H */
