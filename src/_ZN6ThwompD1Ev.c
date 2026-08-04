// @symbol _ZN6ThwompD1Ev
/* recovered: named members + shared header, vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_MovingMeshCollider.h"
#include "decl_ShadowModel.h"
#include "decl_common.h"
extern int data_ov091_021351fc[];
extern int _ZTV6Thwomp[];
extern int _ZTV8Platform[];
/* recovered: named members + shared header, vtable identified */
/* vtable identified: VT0 = _ZTV6Thwomp; VT1 = data_ov091_021351fc */
extern void _ZN15TextureSequenceD1Ev(void *);
int *_ZN6ThwompD1Ev(int *t)
{
    t[0] = (int)_ZTV6Thwomp;
    t[0] = (int)data_ov091_021351fc;
    _ZN11ShadowModelD1Ev((char *)t + 0x338);
    _ZN15TextureSequenceD1Ev((char *)t + 0x324);
    t[0] = (int)_ZTV8Platform;
    _ZN18MovingMeshColliderD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    return t;
}
