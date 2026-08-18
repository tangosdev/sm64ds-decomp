// @symbol Door_Spawn
/* recovered: vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_ModelAnim.h"
#include "decl_common.h"
extern int _ZTV4Door[];
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV4Door */
int *Door_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(328);
    if (p) {
        _ZN8dActor_cC2Ev(p);
        p[0] = (int)_ZTV4Door;
        _ZN9ModelAnimC1Ev((char *)p + 0xd4);
    }
    return p;
}
