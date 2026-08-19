// @symbol MontyMoleRock_Spawn
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Enemy.h"
#include "decl_Model.h"
#include "decl_dCcAc_c.h"
#include "decl_dBgCh_Actr.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV13MontyMoleRock */
int *MontyMoleRock_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(852);
    if (p) {
        _ZN12dEnemyBase_cC2Ev(p);
        p[0] = (int)_ZTV13MontyMoleRock;
        _ZN5ModelC1Ev((char *)p + 0x110);
        _ZN7dCcAc_cC1Ev((char *)p + 0x160);
        _ZN10dBgCh_ActrC1Ev((char *)p + 0x194);
    }
    return p;
}
