// @symbol FirePiranhaPlantSmall_Spawn
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Enemy.h"
#include "decl_ModelAnim.h"
#include "decl_dCcAc_c.h"
#include "decl_dCcAcPos_c.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV19FirePiranhaPlantBig */
int *FirePiranhaPlantSmall_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(556);
    if (p) {
        _ZN12dEnemyBase_cC2Ev(p);
        p[0] = (int)_ZTV19FirePiranhaPlantBig;
        _ZN9ModelAnimC1Ev((char *)p + 0x110);
        _ZN7dCcAc_cC1Ev((char *)p + 0x174);
        _ZN10dCcAcPos_cC1Ev((char *)p + 0x1a8);
    }
    return p;
}
