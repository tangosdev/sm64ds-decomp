// @symbol FallBlockLll_Spawn
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_common.h"
extern int _ZTV20daObjFl_Fall_Block_c[];
extern int _ZTV16daObjFallBlock_c[];
/* recovered: vtable identified. Factory for daObjFl_Fall_Block_c, distinct from
   the already-real daObjFallBlock_c (ov098), which is this class's base. */
int *FallBlockLll_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(844);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)_ZTV16daObjFallBlock_c;
        p[0] = (int)_ZTV20daObjFl_Fall_Block_c;
    }
    return p;
}
