// @symbol SwitchActivatedPlank_Spawn
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Model.h"
#include "decl_Platform.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV20SwitchActivatedPlank */
int *SwitchActivatedPlank_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(936);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)_ZTV20SwitchActivatedPlank;
        _ZN5ModelC1Ev((char *)p + 0x320);
    }
    return p;
}
