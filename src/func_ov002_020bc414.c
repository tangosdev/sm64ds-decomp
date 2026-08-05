// @symbol func_ov002_020bc414
/* recovered: vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ModelAnim.h"
#include "decl_common.h"
extern int data_ov002_02109bb8[];
/* recovered: vtable identified */
/* vtable identified: VT0 = data_ov002_02109bb8 */
int *func_ov002_020bc414(int *t)
{
    t[0] = (int)data_ov002_02109bb8;
    _ZN9ModelAnimD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    return t;
}
