// @symbol SpinningPlatform_Spawn
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_ShadowModel.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV17RotatingClockHand */
int *SpinningPlatform_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(896);
    if (p) {
        _ZN8PlatformC2Ev(p);
        p[0] = (int)_ZTV17RotatingClockHand;
        _ZN11ShadowModelC1Ev((char *)p + 0x328);
    }
    return p;
}
