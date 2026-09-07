// @symbol daObjIceBlock_c_classInit
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_dCcAc_c.h"
#include "decl_Platform.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV8IceBlock */
/* Reconstructed source-style name: SM64DS proves daObjIceBlock_c through RTTI,
 * allocation size, vtable identity, and the ICE_BLOCK_LL registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. The project's IceBlock implementation aliases remain unchanged.
 * Historical alias: IceBlock_Spawn. */
int *daObjIceBlock_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(872);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)_ZTV8IceBlock;
        _ZN7dCcAc_cC1Ev((char *)p + 0x320);
    }
    return p;
}
