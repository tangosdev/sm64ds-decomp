// @symbol daObjFl_Block_c_classInit
/* recovered: globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_common.h"
/* recovered: globals resolved */
/* resolved: VT = _ZTV19FloatOnLavaPlatform */
/* Reconstructed source-style name: SM64DS proves daObjFl_Block_c through RTTI,
 * allocation size, vtable identity, and the FL_BLOCK registry profile; later
 * EAD lineage supplies classInit. Exact original spelling is not preserved.
 * The project's FloatOnLavaPlatform implementation aliases remain unchanged.
 * Historical alias: FloatOnLavaPlatform_Spawn. */
int *daObjFl_Block_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(808);
    if (p) { _ZN10dBgActor_cC2Ev(p); p[0] = (int)_ZTV19FloatOnLavaPlatform; }
    return p;
}
