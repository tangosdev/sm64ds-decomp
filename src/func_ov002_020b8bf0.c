// @symbol func_ov002_020b8bf0
/* recovered: vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_MovingMeshCollider.h"
#include "decl_WithMeshClsn.h"
#include "decl_common.h"
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV16daObjPushblock_c; VT1 = _ZTV10dBgActor_c */
int *func_ov002_020b8bf0(int *t)
{
    t[0] = (int)_ZTV16daObjPushblock_c;
    _ZN12WithMeshClsnD1Ev((char *)t + 0x320);
    t[0] = (int)_ZTV10dBgActor_c;
    _ZN18MovingMeshColliderD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    return t;
}
