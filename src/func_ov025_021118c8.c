// @symbol func_ov025_021118c8
/* recovered: vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_MovingMeshCollider.h"
#include "decl_ShadowModel.h"
#include "decl_common.h"
extern int _ZTV10dBgActor_c[];
/* recovered: vtable identified */
/* vtable identified: VT0 = data_ov025_02113850; VT1 = _ZTV11daDsnBase_c */
extern void _ZN15TextureSequenceD1Ev(void *);
extern int _ZTV11daDsnBase_c[];
extern int data_ov025_02113850[];
int *func_ov025_021118c8(int *t)
{
    t[0] = (int)data_ov025_02113850;
    t[0] = (int)_ZTV11daDsnBase_c;
    _ZN11ShadowModelD1Ev((char *)t + 0x338);
    _ZN15TextureSequenceD1Ev((char *)t + 0x324);
    t[0] = (int)_ZTV10dBgActor_c;
    _ZN18MovingMeshColliderD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN8dActor_cD2Ev(t);
    return t;
}
