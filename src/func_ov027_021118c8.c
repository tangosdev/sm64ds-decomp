// @symbol func_ov027_021118c8
/* recovered: vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_ModelAnim.h"
#include "decl_MovingCylinderClsn.h"
#include "decl_MovingMeshCollider.h"
#include "decl_common.h"
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV10daPgDfdr_c; VT1 = _ZTV10dBgActor_c */
extern void _ZN15TextureSequenceD1Ev(void *);
int *func_ov027_021118c8(int *t)
{
    t[0] = (int)_ZTV10daPgDfdr_c;
    _ZN18MovingCylinderClsnD1Ev((char *)t + 0x398);
    _ZN15TextureSequenceD1Ev((char *)t + 0x384);
    _ZN9ModelAnimD1Ev((char *)t + 0x320);
    t[0] = (int)_ZTV10dBgActor_c;
    _ZN18MovingMeshColliderD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    return t;
}
