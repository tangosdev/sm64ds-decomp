// @symbol FallBlockBbh_Spawn
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_common.h"
extern int _ZTV12FallBlockBbh[];
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV20daObjTh_Fall_Block_c */
int *FallBlockBbh_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(844);
    if (p) {
        _ZN8PlatformC2Ev(p);
        p[0] = (int)_ZTV20daObjTh_Fall_Block_c;
        p[0] = (int)_ZTV12FallBlockBbh;
    }
    return p;
}
