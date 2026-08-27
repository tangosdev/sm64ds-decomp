// @symbol Trap_Spawn
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Model.h"
#include "decl_Platform.h"
#include "decl_common.h"
extern int _ZTV4Trap[];
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV4Trap */
int *Trap_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(944);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)_ZTV4Trap;
        _ZN5ModelC1Ev((char *)p + 0x320);
    }
    return p;
}
