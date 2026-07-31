// @symbol ClockPaintingPendulum_Spawn
/* recovered: vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_Model.h"
#include "decl_common.h"
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV18daObjClockHuriko_c */
int *ClockPaintingPendulum_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(296);
    if (p) {
        _ZN5ActorC2Ev(p);
        p[0] = (int)_ZTV18daObjClockHuriko_c;
        _ZN5ModelC1Ev((char *)p + 0xd4);
    }
    return p;
}
