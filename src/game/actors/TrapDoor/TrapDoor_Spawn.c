// @symbol TrapDoor_Spawn
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_Model.h"
#include "decl_dBgW_KcMbg.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV12MansionSteps */
int *TrapDoor_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(852);
    if (p) {
        _ZN8dActor_cC2Ev(p);
        p[0] = (int)_ZTV12MansionSteps;
        _ZN5ModelC1Ev((char *)p + 0xd4);
        _ZN10dBgW_KcMbgC1Ev((char *)p + 0x15c);
    }
    return p;
}
