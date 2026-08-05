// @symbol TTC_MovingBar_Spawn
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_ShadowModel.h"
#include "decl_common.h"
extern int data_ov065_0211d2b4[];
/* recovered: vtable identified */
/* vtable identified: VT0 = data_ov065_0211d2b4 */
int *TTC_MovingBar_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(916);
    if (p) {
        _ZN8PlatformC2Ev(p);
        p[0] = (int)data_ov065_0211d2b4;
        _ZN11ShadowModelC1Ev((char *)p + 0x33c);
    }
    return p;
}
