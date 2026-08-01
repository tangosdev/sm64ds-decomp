// @symbol FallBlockLll_Spawn
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_common.h"
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV20daObjFl_Fall_Block_c */
int *FallBlockLll_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(844);
    if (p) {
        _ZN8PlatformC2Ev(p);
        p[0] = (int)_ZTV20daObjFl_Fall_Block_c;
        p[0] = (int)VT1;
    }
    return p;
}
