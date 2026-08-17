// @symbol func_ov065_02119f3c
/* recovered: vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_MovingMeshCollider.h"
#include "decl_ShadowModel.h"
#include "decl_common.h"
extern int data_ov065_0211d0ec[];
extern int _ZTV10dBgActor_c[];
/* recovered: vtable identified */
/* vtable identified: VT0 = data_ov065_0211d0ec; VT1 = _ZTV10dBgActor_c */
int *func_ov065_02119f3c(int *t)
{
    t[0] = (int)data_ov065_0211d0ec;
    _ZN11ShadowModelD1Ev((char *)t + 0x330);
    t[0] = (int)_ZTV10dBgActor_c;
    _ZN18MovingMeshColliderD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    return t;
}
