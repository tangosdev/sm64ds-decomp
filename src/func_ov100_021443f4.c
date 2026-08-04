// @symbol func_ov100_021443f4
/* recovered: vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ModelAnim.h"
#include "decl_common.h"
extern int data_ov100_02148188[];
/* recovered: vtable identified */
/* vtable identified: VT0 = data_ov100_02148188 */
int *func_ov100_021443f4(int *t)
{
    t[0] = (int)data_ov100_02148188;
    _ZN9ModelAnimD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    return t;
}
