// @symbol MgShuffleShell_Spawn
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_common.h"
extern int data_ov006_0213c304[];
/* recovered: vtable identified */
/* vtable identified: VT0 = data_ov006_0213c304 */
int *MgShuffleShell_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(20204);
    if (p) {
        _ZN11dScMgBase_cC2Ev(p);
        p[0] = (int)data_ov006_0213c304;
    }
    return p;
}
