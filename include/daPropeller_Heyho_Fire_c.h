#ifndef DAPROPELLER_HEYHO_FIRE_C_H
#define DAPROPELLER_HEYHO_FIRE_C_H

#include "types.h"

/* The propeller Shy Guy's fireball (registry profile PROPELLER_HEYHO_FIRE).
 * The cartridge's RTTI names the class: _ZTI24daPropeller_Heyho_Fire_c at
 * 0x0210d608, _ZTS24daPropeller_Heyho_Fire_c at 0x0210d614, and the vtable
 * _ZTV24daPropeller_Heyho_Fire_c at 0x0210d654 that the factory and destructor store.
 *
 * Derives from dEnemyBase_c, on the evidence of its own destructor: `_ZN24daPropeller_Heyho_Fire_cD1Ev`
 * stores this vtable, destroys its members in reverse declaration order, then
 * calls `dEnemyBase_c::~dEnemyBase_c`. Everything this header used to restate below 0x110
 * belongs to that chain and is inherited now.
 *
 * The members close exactly on one another:
 *
 *     0x110 dCcAc_c         0x34   -> 0x144
 *     0x144 dBgCh_Actr               0x1bc  -> 0x300
 *     0x300 dExtSimpleModel_c                      0x50   -> 0x350
 *
 * Member NAMES are the ones this header already used -- a rebase should not
 * also rename things its callers spell.
 *
 * SIZE IS THE OBSERVED FIELD SPAN, rounded up. It guards this declaration; it
 * is not independent evidence about the ROM.
 */

#include "dEnemyBase_c.h"
#include "dExtSimpleModel_c.h"
#include "dExtAnmModel_c.h"
#include "dCcAc_c.h"
#include "dCcAcPos_c.h"
#include "dExtShadowModel_c.h"
#include "dExtAnmTexSRT_c.h"
#include "dBgCh_Actr.h"

struct daPropeller_Heyho_Fire_c : dEnemyBase_c {
    /* What mCurrentState points at: two member-function pointers.
       SetState calls mInit (when set) as it installs the state;
       Behavior calls mMain (when set). The one state's pair is
       InitState0 / State0 (data_ov002_02111190). */
    struct State {
        int (daPropeller_Heyho_Fire_c::*mInit)(); /* 0x00 */
        int (daPropeller_Heyho_Fire_c::*mMain)(); /* 0x08 */
    };

    dCcAc_c mdCcAc_c;         /* 0x110 */
    dBgCh_Actr mWithMeshClsn; /* 0x144 */
    dExtSimpleModel_c mModel; /* 0x300 */
    State *mCurrentState;     /* 0x350 */
    s32 mDeflected; /* 0x354 -- set when a metal player deflected it; State0 then skips the player-hit test */
    s32 unk_358; /* 0x358 */

    /* --- vtable --- */
    virtual ~daPropeller_Heyho_Fire_c();

    int Behavior();
    int CleanupResources();
    int InitResources();
    void OnPendingDestroy();
    int Render();
    /* --- non-virtual --- */
    void CheckPlayerHit();    /* ov002 0x020feb50 */
    int State0();             /* ov002 0x020fec94 */
    int InitState0();         /* ov002 0x020fed18 */
    int SetState(State *);    /* ov002 0x020fed2c */
    void UpdateModelMatrix(); /* ov002 0x020fed7c */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char
    daPropeller_Heyho_Fire_c_size_must_be_0x35c[sizeof(daPropeller_Heyho_Fire_c) == 0x35c ? 1 : -1];
#endif

#endif /* DAPROPELLER_HEYHO_FIRE_C_H */
