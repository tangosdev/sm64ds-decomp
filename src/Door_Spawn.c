// @symbol Door_Spawn
/* recovered: vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_ModelAnim.h"
#include "decl_common.h"
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV8daDoor_c */
int *Door_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(328);
    if (p) {
        _ZN5ActorC2Ev(p);
        p[0] = (int)_ZTV8daDoor_c;
        _ZN9ModelAnimC1Ev((char *)p + 0xd4);
    }
    return p;
}
