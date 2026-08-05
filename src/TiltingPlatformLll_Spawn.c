// @symbol TiltingPlatformLll_Spawn
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_common.h"
extern int data_ov002_02109084[];
extern int _ZTV12MetalNetLift[];
/* recovered: vtable identified */
/* vtable identified: VT0 = data_ov002_02109084 */
int *TiltingPlatformLll_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(848);
    if (p) {
        _ZN8PlatformC2Ev(p);
        p[0] = (int)data_ov002_02109084;
        p[0] = (int)_ZTV12MetalNetLift;
    }
    return p;
}
