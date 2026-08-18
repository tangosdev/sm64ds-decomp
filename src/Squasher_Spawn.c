// @symbol Squasher_Spawn
/* recovered: globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_ShadowModel.h"
#include "decl_common.h"
/* recovered: globals resolved */
/* resolved: VT0 = _ZTV8Squasher */
int *Squasher_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(892);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)_ZTV8Squasher;
        _ZN11ShadowModelC1Ev((char *)p + 0x324);
    }
    return p;
}
