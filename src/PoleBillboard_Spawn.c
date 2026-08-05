// @symbol PoleBillboard_Spawn
/* recovered: vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_Model.h"
#include "decl_common.h"
extern int data_ov015_02114360[];
/* recovered: vtable identified */
/* vtable identified: VT0 = data_ov015_02114360 */
int *PoleBillboard_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(292);
    if (p) {
        _ZN5ActorC2Ev(p);
        p[0] = (int)data_ov015_02114360;
        _ZN5ModelC1Ev((char *)p + 0xd4);
    }
    return p;
}
