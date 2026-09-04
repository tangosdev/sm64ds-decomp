// @symbol dScMgCurling_c_classInit
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_common.h"
extern int data_ov006_0213c304[];
/* recovered: vtable identified */
/* vtable identified: VT0 = data_ov006_0213c304 */
/* Reconstructed source-style name: SM64DS proves the dScMgCurling_c RTTI
 * identity, MG_CURLING registry ID, base-profile/factory relationship,
 * allocation size, and vtable identity; later EAD lineage supplies the
 * classInit spelling prior. Exact original SM64DS spelling is not preserved.
 * Historical project alias: MgShuffleShell_Spawn. */
int *dScMgCurling_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(20204);
    if (p) {
        _ZN11dScMgBase_cC2Ev(p);
        p[0] = (int)data_ov006_0213c304;
    }
    return p;
}
