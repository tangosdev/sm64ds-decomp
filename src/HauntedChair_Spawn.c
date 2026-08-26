// @symbol HauntedChair_Spawn
/* The natural C++ spelling `return new HauntedChair` reproduces every
 * instruction, but its allocation relocation names the unavailable global
 * `_Znwm` (linkcheck BLIND-1) where the ROM calls fBase_c::operator new.
 * Keep this truthful C transcription until that allocator spelling is solved. */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_Model.h"
#include "decl_dCcAcPos_c.h"
#include "decl_ShadowModel.h"
#include "decl_dBgCh_Actr.h"
#include "decl_common.h"
int *HauntedChair_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(936);
    if (p) {
        _ZN8dActor_cC2Ev(p);
        p[0] = (int)_ZTV12HauntedChair;
        _ZN5ModelC1Ev((char *)p + 0xd4);
        _ZN11ShadowModelC1Ev((char *)p + 0x124);
        _ZN10dCcAcPos_cC1Ev((char *)p + 0x17c);
        _ZN10dBgCh_ActrC1Ev((char *)p + 0x1bc);
    }
    return p;
}
