// @symbol InvisiblePole_Spawn
/* recovered: vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_MovingCylinderClsn.h"
#include "decl_common.h"
extern int data_ov002_02108480[];
/* recovered: vtable identified */
/* vtable identified: VT0 = data_ov002_02108480 */
int *InvisiblePole_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(264);
    if (p) {
        _ZN5ActorC2Ev(p);
        p[0] = (int)data_ov002_02108480;
        _ZN18MovingCylinderClsnC1Ev((char *)p + 0xd4);
    }
    return p;
}
