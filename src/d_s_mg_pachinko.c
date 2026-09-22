// @symbol dScMgPachinko_c_classInit
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_common.h"
extern int data_ov006_0213d9cc[];
/* recovered: vtable identified */
/* vtable identified: VT0 = data_ov006_0213d9cc */
/* Reconstructed source-style name: SM64DS proves dScMgPachinko_c through RTTI,
 * allocation size, vtable identity, and the MG_PACHINKO registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: MgBobOmbSquad_Spawn. */
int *dScMgPachinko_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(23608);
    if (p) {
        _ZN11dScMgBase_cC2Ev(p);
        p[0] = (int)data_ov006_0213d9cc;
    }
    return p;
}
