// @symbol MetalNetLift_Spawn
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_common.h"
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV17daObjFl_Amilift_c */
extern void _ZN7PathPtrC1Ev(void *);
int *MetalNetLift_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(872);
    if (p) {
        _ZN8PlatformC2Ev(p);
        p[0] = (int)_ZTV17daObjFl_Amilift_c;
        _ZN7PathPtrC1Ev((char *)p + 0x360);
    }
    return p;
}
