// @symbol dScMgLuigi_c_classInit
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_common.h"
extern int data_ov006_0213cf10[];
/* recovered: vtable identified */
/* vtable identified: VT0 = data_ov006_0213cf10 */
/* Reconstructed source-style name: SM64DS proves dScMgLuigi_c through RTTI,
 * allocation size, vtable identity, and the MG_LUIGI registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: MgWanted_Spawn. */
int *dScMgLuigi_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(21596);
    if (p) {
        _ZN11dScMgBase_cC2Ev(p);
        p[0] = (int)data_ov006_0213cf10;
    }
    return p;
}
