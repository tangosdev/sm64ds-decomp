// @symbol func_ov015_021111a0
/* recovered: vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_common.h"
extern int data_ov015_02114360[];
/* recovered: vtable identified */
/* vtable identified: VT0 = data_ov015_02114360 */
int *func_ov015_021111a0(int *t)
{
    t[0] = (int)data_ov015_02114360;
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    return t;
}
