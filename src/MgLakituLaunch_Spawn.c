// @symbol MgLakituLaunch_Spawn
/* recovered: vtable identified, declarations from a shared header */
#include "decl_ActorBase.h"
#include "decl_common.h"
extern int data_ov006_0213dbbc[];
/* recovered: vtable identified */
/* vtable identified: VT0 = data_ov006_0213dbbc */
int *MgLakituLaunch_Spawn(void)
{
    int *p = (int *)_ZN7fBase_cnwEj(22140);
    if (p) {
        func_ov004_020b2adc(p);
        p[0] = (int)data_ov006_0213dbbc;
    }
    return p;
}
