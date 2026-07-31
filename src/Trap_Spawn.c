// @symbol Trap_Spawn
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Model.h"
#include "decl_Platform.h"
#include "decl_common.h"
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV14daObjC1_Trap_c */
int *Trap_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(944);
    if (p) {
        _ZN8PlatformC2Ev(p);
        p[0] = (int)_ZTV14daObjC1_Trap_c;
        _ZN5ModelC1Ev((char *)p + 0x320);
    }
    return p;
}
