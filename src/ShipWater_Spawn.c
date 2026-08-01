// @symbol ShipWater_Spawn
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_TextureTransformer.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV9ShipWater */
int *ShipWater_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(832);
    if (p) {
        _ZN8PlatformC2Ev(p);
        p[0] = (int)_ZTV9ShipWater;
        _ZN18TextureTransformerC1Ev((char *)p + 0x320);
    }
    return p;
}
