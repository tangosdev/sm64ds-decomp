// @symbol MgPuzzlePanelPuzzlePanic_Spawn
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_common.h"
extern int data_ov006_0213e24c[];
/* recovered: vtable identified */
/* vtable identified: VT0 = data_ov006_0213e24c */
int *MgPuzzlePanelPuzzlePanic_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(20460);
    if (p) {
        _ZN11dScMgBase_cC2Ev(p);
        p[0] = (int)data_ov006_0213e24c;
    }
    return p;
}
