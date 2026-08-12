// @symbol func_ov002_020f03c4
/* recovered: vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_MovingCylinderClsn.h"
#include "decl_common.h"
extern int data_ov002_0210b030[];
/* recovered: vtable identified */
/* vtable identified: VT0 = data_ov002_0210b030 */
int *func_ov002_020f03c4(int *t)
{
    t[0] = (int)data_ov002_0210b030;
    _ZN18MovingCylinderClsnD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    return t;
}
