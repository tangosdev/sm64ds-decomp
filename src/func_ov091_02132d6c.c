// @symbol func_ov091_02132d6c
/* recovered: vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_MovingMeshCollider.h"
#include "decl_ShadowModel.h"
#include "decl_common.h"
extern int data_ov091_021351fc[];
extern int _ZTV8Platform[];
/* recovered: vtable identified */
/* vtable identified: VT0 = data_ov091_021351fc */
extern void _ZN15TextureSequenceD1Ev(void *);
int *func_ov091_02132d6c(int *t)
{
    t[0] = (int)data_ov091_021351fc;
    _ZN11ShadowModelD1Ev((char *)t + 0x338);
    _ZN15TextureSequenceD1Ev((char *)t + 0x324);
    t[0] = (int)_ZTV8Platform;
    _ZN18MovingMeshColliderD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    return t;
}
