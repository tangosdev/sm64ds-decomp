// @symbol func_ov016_02112ef4
/* recovered: vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_MovingMeshCollider.h"
#include "decl_common.h"
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV13daObjKi_Ita_c; VT1 = _ZTV10dBgActor_c */
int *func_ov016_02112ef4(int *t)
{
    t[0] = (int)_ZTV13daObjKi_Ita_c;
    t[0] = (int)_ZTV10dBgActor_c;
    t[0] = (int)VT2;
    _ZN18MovingMeshColliderD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    return t;
}
