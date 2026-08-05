// @symbol func_ov080_02126fbc
/* recovered: vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_MovingMeshCollider.h"
#include "decl_common.h"
extern int data_ov080_02128338[];
extern int _ZTV8Platform[];
/* recovered: vtable identified */
/* vtable identified: VT0 = data_ov080_02128338 */
extern void *data_020a0eac;
int *func_ov080_02126fbc(int *t)
{
    t[0] = (int)data_ov080_02128338;
    t[0] = (int)_ZTV8Platform;
    _ZN18MovingMeshColliderD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
