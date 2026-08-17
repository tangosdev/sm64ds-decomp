// @symbol func_ov025_02111928
/* recovered: vtable identified, renamed to Class_Method, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_MovingMeshCollider.h"
#include "decl_ShadowModel.h"
#include "decl_common.h"
/* recovered: vtable identified, renamed to Class_Method */
extern void _ZN15TextureSequenceD1Ev(void *);
extern void *data_020a0eac;
extern int _ZTV11daDsnBase_c[];
extern int _ZTV10dBgActor_c[];
extern void *data_020a0eac;
extern int data_ov025_02113850[];
int *func_ov025_02111928(int *t)
{
    t[0] = (int)data_ov025_02113850;
    t[0] = (int)_ZTV11daDsnBase_c;
    _ZN11ShadowModelD1Ev((char *)t + 0x338);
    _ZN15TextureSequenceD1Ev((char *)t + 0x324);
    t[0] = (int)_ZTV10dBgActor_c;
    _ZN18MovingMeshColliderD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    _ZN6Memory10DeallocateEPvP4Heap(t, data_020a0eac);
    return t;
}
