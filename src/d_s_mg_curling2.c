// @symbol dScMgCurling2_c_classInit
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_common.h"
extern int data_ov006_0213c510[];
/* recovered: vtable identified */
/* vtable identified: VT0 = data_ov006_0213c510 */
/* Reconstructed source-style name: SM64DS proves the dScMgCurling2_c RTTI
 * identity, MG_CURLING_J registry ID, base-profile/factory relationship,
 * allocation size, and vtable identity; later EAD lineage supplies the
 * classInit spelling prior. Exact original SM64DS spelling is not preserved.
 * Historical project alias: func_ov006_020e6bf4. */
int *dScMgCurling2_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(21956);
    if (p) {
        _ZN11dScMgBase_cC2Ev(p);
        p[0] = (int)data_ov006_0213c510;
    }
    return p;
}
