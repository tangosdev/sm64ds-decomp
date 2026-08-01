// @symbol PyramidTop_Spawn
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Model.h"
#include "decl_Platform.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV10PyramidTop */
int *PyramidTop_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(952);
    if (p) {
        _ZN8PlatformC2Ev(p);
        p[0] = (int)_ZTV10PyramidTop;
        _ZN5ModelC1Ev((char *)p + 0x320);
    }
    return p;
}
