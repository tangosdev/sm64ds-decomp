// @symbol OrangeBallBillboard_Spawn
/* recovered: globals resolved, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_Model.h"
#include "decl_common.h"
/* recovered: globals resolved */
/* resolved: VT0 = _ZTV19OrangeBallBillboard */
int *OrangeBallBillboard_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(292);
    if (p) {
        _ZN5ActorC2Ev(p);
        p[0] = (int)_ZTV19OrangeBallBillboard;
        _ZN5ModelC1Ev((char *)p + 0xd4);
    }
    return p;
}
