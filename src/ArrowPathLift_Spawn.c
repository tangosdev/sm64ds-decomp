// @symbol ArrowPathLift_Spawn
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV22RotatingUpDownPlatform */
extern void _ZN7PathPtrC1Ev(void *);
int *ArrowPathLift_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(856);
    if (p) {
        _ZN8PlatformC2Ev(p);
        p[0] = (int)_ZTV22RotatingUpDownPlatform;
        _ZN7PathPtrC1Ev((char *)p + 0x344);
    }
    return p;
}
