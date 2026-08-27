// @symbol InvisiblePole_Spawn
/* recovered: vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_dCcAc_c.h"
#include "decl_common.h"
extern int _ZTV13InvisiblePole[];
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV13InvisiblePole */
int *InvisiblePole_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(264);
    if (p) {
        _ZN8dActor_cC2Ev(p);
        p[0] = (int)_ZTV13InvisiblePole;
        _ZN7dCcAc_cC1Ev((char *)p + 0xd4);
    }
    return p;
}
