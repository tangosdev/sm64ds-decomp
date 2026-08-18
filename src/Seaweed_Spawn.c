// @symbol Seaweed_Spawn
/* recovered: vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_ModelAnim.h"
#include "decl_common.h"
extern int _ZTV7Seaweed[];
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV7Seaweed */
int *Seaweed_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(312);
    if (p) {
        _ZN8dActor_cC2Ev(p);
        p[0] = (int)_ZTV7Seaweed;
        _ZN9ModelAnimC1Ev((char *)p + 0xd4);
    }
    return p;
}
