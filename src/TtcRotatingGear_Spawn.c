// @symbol TtcRotatingGear_Spawn
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_ShadowModel.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV13TTC_MovingBar */
int *TtcRotatingGear_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(892);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)_ZTV13TTC_MovingBar;
        _ZN11ShadowModelC1Ev((char *)p + 0x324);
    }
    return p;
}
