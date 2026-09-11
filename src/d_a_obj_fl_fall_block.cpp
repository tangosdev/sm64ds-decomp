// @symbol daObjFl_Fall_Block_c_classInit
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_common.h"
extern int _ZTV20daObjFl_Fall_Block_c[];
extern int _ZTV16daObjFallBlock_c[];
/* recovered: vtable identified. Factory for daObjFl_Fall_Block_c, distinct from
   the already-real daObjFallBlock_c (ov098), which is this class's base. */
/* Reconstructed source-style name: SM64DS proves daObjFl_Fall_Block_c through
 * RTTI, allocation size, most-derived vtable identity, and the FL_KUZURE
 * registry profile; later EAD lineage supplies classInit. Exact original
 * spelling is not preserved. The project's FallBlockLll implementation aliases
 * remain unchanged. Historical alias: FallBlockLll_Spawn. */
int *daObjFl_Fall_Block_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(844);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)_ZTV16daObjFallBlock_c;
        p[0] = (int)_ZTV20daObjFl_Fall_Block_c;
    }
    return p;
}
