// @symbol func_ov027_021118c8
/* recovered: vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_ModelAnim.h"
#include "decl_MovingCylinderClsn.h"
#include "decl_MovingMeshCollider.h"
#include "decl_common.h"
extern int data_ov027_02113a90[];
extern int _ZTV8Platform[];
/* recovered: vtable identified */
/* vtable identified: VT0 = data_ov027_02113a90; VT1 = _ZTV8Platform */
extern void _ZN15TextureSequenceD1Ev(void *);
int *func_ov027_021118c8(int *t)
{
    t[0] = (int)data_ov027_02113a90;
    _ZN18MovingCylinderClsnD1Ev((char *)t + 0x398);
    _ZN15TextureSequenceD1Ev((char *)t + 0x384);
    _ZN9ModelAnimD1Ev((char *)t + 0x320);
    t[0] = (int)_ZTV8Platform;
    _ZN18MovingMeshColliderD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    return t;
}
