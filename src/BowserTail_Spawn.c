// @symbol BowserTail_Spawn
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_MovingCylinderClsn.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV10BowserTail */
int *BowserTail_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(280);
    if (p) {
        _ZN5ActorC2Ev(p);
        p[0] = (int)_ZTV10BowserTail;
        _ZN18MovingCylinderClsnC1Ev((char *)p + 0xd4);
    }
    return p;
}
