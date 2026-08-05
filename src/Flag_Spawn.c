// @symbol Flag_Spawn
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_ModelAnim.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV4Flag */
int *Flag_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(312);
    if (p) {
        _ZN5ActorC2Ev(p);
        p[0] = (int)_ZTV4Flag;
        _ZN9ModelAnimC1Ev((char *)p + 0xd4);
    }
    return p;
}
