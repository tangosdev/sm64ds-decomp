// @symbol FlyGuy_Spawn
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Enemy.h"
#include "decl_ModelAnim.h"
#include "decl_dCcAc_c.h"
#include "decl_ShadowModel.h"
#include "decl_dBgCh_Actr.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV6FlyGuy */
int *FlyGuy_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(1000);
    if (p) {
        _ZN12dEnemyBase_cC2Ev(p);
        p[0] = (int)_ZTV6FlyGuy;
        _ZN7dCcAc_cC1Ev((char *)p + 0x110);
        _ZN10dBgCh_ActrC1Ev((char *)p + 0x144);
        _ZN9ModelAnimC1Ev((char *)p + 0x300);
        _ZN11ShadowModelC1Ev((char *)p + 0x364);
    }
    return p;
}
