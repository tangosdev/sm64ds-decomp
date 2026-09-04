// @symbol daObjDpBrock_c_classInit
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Model.h"
#include "decl_Platform.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV11PyramidStep */
/* Reconstructed source-style name: SM64DS proves daObjDpBrock_c through RTTI,
 * allocation size, vtable identity, and the DP_BROCK registry profile; later
 * EAD lineage supplies classInit. Exact original spelling is not preserved.
 * The project's PyramidStep implementation aliases remain unchanged.
 * Historical alias: PyramidStep_Spawn. */
int *daObjDpBrock_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(932);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)_ZTV11PyramidStep;
        _ZN5ModelC1Ev((char *)p + 0x320);
    }
    return p;
}
