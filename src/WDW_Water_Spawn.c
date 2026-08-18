// @symbol WDW_Water_Spawn
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_TextureTransformer.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV9WDW_Water */
int *WDW_Water_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(840);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)_ZTV9WDW_Water;
        _ZN18TextureTransformerC1Ev((char *)p + 0x320);
    }
    return p;
}
