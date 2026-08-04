// @symbol ClockPaintingPendulum_Spawn
/* recovered: vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_Model.h"
#include "decl_common.h"
extern int data_ov013_02112128[];
/* recovered: vtable identified */
/* vtable identified: VT0 = data_ov013_02112128 */
int *ClockPaintingPendulum_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(296);
    if (p) {
        _ZN5ActorC2Ev(p);
        p[0] = (int)data_ov013_02112128;
        _ZN5ModelC1Ev((char *)p + 0xd4);
    }
    return p;
}
