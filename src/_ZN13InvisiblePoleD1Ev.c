// @symbol func_ov002_020b05d0
/* recovered: vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_MovingCylinderClsn.h"
#include "decl_common.h"
extern int data_ov002_02108480[];
/* recovered: vtable identified */
/* vtable identified: VT0 = data_ov002_02108480 */
int *func_ov002_020b05d0(int *t)
{
    t[0] = (int)data_ov002_02108480;
    _ZN18MovingCylinderClsnD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    return t;
}
