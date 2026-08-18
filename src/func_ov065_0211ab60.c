// @symbol func_ov065_0211ab60
/* recovered: vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_MovingMeshCollider.h"
#include "decl_ShadowModel.h"
#include "decl_common.h"
extern int data_ov065_0211d2b4[];
extern int _ZTV10dBgActor_c[];
/* recovered: vtable identified */
/* vtable identified: VT0 = data_ov065_0211d2b4; VT1 = _ZTV10dBgActor_c */
int *func_ov065_0211ab60(int *t)
{
    t[0] = (int)data_ov065_0211d2b4;
    _ZN11ShadowModelD1Ev((char *)t + 0x33c);
    t[0] = (int)_ZTV10dBgActor_c;
    _ZN18MovingMeshColliderD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN8dActor_cD2Ev(t);
    return t;
}
