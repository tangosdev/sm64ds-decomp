// @symbol MetalNetLift_Spawn
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_common.h"
extern int _ZTV12MetalNetLift[];
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV12MetalNetLift */
extern void _ZN7PathPtrC1Ev(void *);
int *MetalNetLift_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(872);
    if (p) {
        _ZN8PlatformC2Ev(p);
        p[0] = (int)_ZTV12MetalNetLift;
        _ZN7PathPtrC1Ev((char *)p + 0x360);
    }
    return p;
}
