// @symbol MetalNet_Spawn
/* recovered: globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_common.h"
/* recovered: globals resolved */
/* resolved: VT = _ZTV8MetalNet */
int *MetalNet_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(800);
    if (p) { _ZN8PlatformC2Ev(p); p[0] = (int)_ZTV8MetalNet; }
    return p;
}
