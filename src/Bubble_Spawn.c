// @symbol Bubble_Spawn
/* recovered: vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_MovingCylinderClsn.h"
#include "decl_common.h"
extern int _ZTV6Bubble[];
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV6Bubble */
int *Bubble_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(276);
    if (p) {
        _ZN5ActorC2Ev(p);
        p[0] = (int)_ZTV6Bubble;
        _ZN18MovingCylinderClsnC1Ev((char *)p + 0xd4);
    }
    return p;
}
