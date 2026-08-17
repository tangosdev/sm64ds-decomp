// @symbol func_ov025_021111a0
/* recovered: vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_MovingMeshCollider.h"
#include "decl_common.h"
extern int _ZTV10dBgActor_c[];
extern int data_ov025_02113760[];
/* recovered: vtable identified */
/* vtable identified: VT0 = data_ov025_02113760; VT1 = _ZTV10dBgActor_c */
int *func_ov025_021111a0(int *t)
{
    t[0] = (int)data_ov025_02113760;
    t[0] = (int)_ZTV10dBgActor_c;
    _ZN18MovingMeshColliderD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    return t;
}
