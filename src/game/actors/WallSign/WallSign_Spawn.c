// @symbol WallSign_Spawn
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_dCcAcPos_c.h"
#include "decl_Platform.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV8WallSign */
int *WallSign_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(872);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)_ZTV8WallSign;
        _ZN10dCcAcPos_cC1Ev((char *)p + 0x320);
    }
    return p;
}
