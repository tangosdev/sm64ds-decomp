// @symbol func_ov029_02111ea4
/* recovered: vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_MovingMeshCollider.h"
#include "decl_common.h"
extern int data_ov029_02113f44[];
extern int data_ov002_02108fdc[];
extern int _ZTV8Platform[];
/* recovered: vtable identified */
/* vtable identified: VT0 = data_ov029_02113f44; VT1 = data_ov002_02108fdc */
int *func_ov029_02111ea4(int *t)
{
    t[0] = (int)data_ov029_02113f44;
    t[0] = (int)data_ov002_02108fdc;
    t[0] = (int)_ZTV8Platform;
    _ZN18MovingMeshColliderD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    return t;
}
