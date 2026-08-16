// @symbol func_ov047_021113f8
/* recovered: vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_MovingMeshCollider.h"
#include "decl_common.h"
extern int data_ov047_0211244c[];
extern int _ZTV13daObjKuruma_c[];
extern int _ZTV8Platform[];
/* recovered: vtable identified */
/* vtable identified: VT0 = data_ov047_0211244c; VT1 = _ZTV13daObjKuruma_c */
int *func_ov047_021113f8(int *t)
{
    t[0] = (int)data_ov047_0211244c;
    t[0] = (int)_ZTV13daObjKuruma_c;
    t[0] = (int)_ZTV8Platform;
    _ZN18MovingMeshColliderD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    return t;
}
