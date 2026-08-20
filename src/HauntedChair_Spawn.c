// @symbol HauntedChair_Spawn
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_Model.h"
#include "decl_dCcAcPos_c.h"
#include "decl_ShadowModel.h"
#include "decl_dBgCh_Actr.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV12HauntedChair */
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
