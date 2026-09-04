// @symbol daObjRc_Tikuwa_c_classInit
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_Platform.h"
#include "decl_dBgCh_Actr.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV10DonutBlock */
/* Reconstructed source-style name: the SM64DS ROM carries the
 * daObjRc_Tikuwa_c RTTI string and proves its allocation, vtable identity, and
 * RC_TIKUWA registry profile; later EAD lineage supplies classInit. Exact
 * original spelling is not preserved. The project's DonutBlock implementation
 * alias remains unchanged. Historical alias: DonutBlock_Spawn. */
int *daObjRc_Tikuwa_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(1260);
    if (p) {
        _ZN10dBgActor_cC2Ev(p);
        p[0] = (int)_ZTV10DonutBlock;
        _ZN10dBgCh_ActrC1Ev((char *)p + 0x320);
    }
    return p;
}
