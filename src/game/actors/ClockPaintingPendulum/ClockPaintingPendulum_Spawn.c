// @symbol ClockPaintingPendulum_Spawn
/* recovered: vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_Model.h"
#include "decl_common.h"
extern int _ZTV21ClockPaintingPendulum[];
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV21ClockPaintingPendulum */
int *ClockPaintingPendulum_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(296);
    if (p) {
        _ZN8dActor_cC2Ev(p);
        p[0] = (int)_ZTV21ClockPaintingPendulum;
        _ZN5ModelC1Ev((char *)p + 0xd4);
    }
    return p;
}
