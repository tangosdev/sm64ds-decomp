// @symbol func_ov036_02111494
/* recovered: vtable identified, renamed to Class_Method, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_MovingMeshCollider.h"
#include "decl_common.h"
extern int data_ov036_02113b74[];
extern int data_ov002_021091d4[];
extern int _ZTV8Platform[];
/* recovered: vtable identified, renamed to Class_Method */
extern void *data_020a0eac;
int *func_ov036_02111494(int *t)
{
    t[0] = (int)data_ov036_02113b74;
    t[0] = (int)data_ov002_021091d4;
    t[0] = (int)_ZTV8Platform;
    _ZN18MovingMeshColliderD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
