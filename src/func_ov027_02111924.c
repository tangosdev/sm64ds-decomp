// @symbol func_ov027_02111924
// recovered name: daPgDfdr_c_OnYoshiTryEat
/* recovered: vtable identified, renamed to Class_Method, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_ModelAnim.h"
#include "decl_MovingCylinderClsn.h"
#include "decl_MovingMeshCollider.h"
#include "decl_common.h"
extern int data_ov027_02113a90[];
extern int _ZTV8Platform[];
/* recovered: vtable identified, renamed to Class_Method */
/* daPgDfdr_c::OnYoshiTryEat - recovered from vtable slot identity */
extern void _ZN15TextureSequenceD1Ev(void *);
extern void *data_020a0eac;
int *func_ov027_02111924(int *t)
{
    t[0] = (int)data_ov027_02113a90;
    _ZN18MovingCylinderClsnD1Ev((char *)t + 0x398);
    _ZN15TextureSequenceD1Ev((char *)t + 0x384);
    _ZN9ModelAnimD1Ev((char *)t + 0x320);
    t[0] = (int)_ZTV8Platform;
    _ZN18MovingMeshColliderD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
