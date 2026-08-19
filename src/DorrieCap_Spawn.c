// @symbol DorrieCap_Spawn
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_Model.h"
#include "decl_MovingCylinderClsn.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV12daDossyCap_c */
int *DorrieCap_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(388);
    if (p) {
        _ZN8dActor_cC2Ev(p);
        p[0] = (int)_ZTV12daDossyCap_c;
        func_ov001_020ab3c4((char *)p + 0xd4);
        _ZN5ModelC1Ev((char *)p + 0xf0);
        _ZN18MovingCylinderClsnC1Ev((char *)p + 0x140);
    }
    return p;
}
