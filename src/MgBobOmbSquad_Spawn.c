// @symbol MgBobOmbSquad_Spawn
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_common.h"
extern int data_ov006_0213d9cc[];
/* recovered: vtable identified */
/* vtable identified: VT0 = data_ov006_0213d9cc */
int *MgBobOmbSquad_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(23608);
    if (p) {
        _ZN11dScMgBase_cC2Ev(p);
        p[0] = (int)data_ov006_0213d9cc;
    }
    return p;
}
