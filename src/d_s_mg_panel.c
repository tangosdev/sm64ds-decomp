// @symbol dScMgPanel_c_classInit
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_common.h"
extern int data_ov006_0213e24c[];
/* recovered: vtable identified */
/* vtable identified: VT0 = data_ov006_0213e24c */
/* Reconstructed source-style name: SM64DS proves dScMgPanel_c through RTTI,
 * allocation size, vtable identity, and the MG_PANEL registry profile;
 * later EAD lineage supplies classInit. Exact original spelling is not
 * preserved. Historical alias: MgPuzzlePanelPuzzlePanic_Spawn. */
int *dScMgPanel_c_classInit(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(20460);
    if (p) {
        _ZN11dScMgBase_cC2Ev(p);
        p[0] = (int)data_ov006_0213e24c;
    }
    return p;
}
