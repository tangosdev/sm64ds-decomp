// @symbol _ZN6ThwompD1Ev
/* recovered: named members + shared header, vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_MovingMeshCollider.h"
#include "decl_ShadowModel.h"
#include "decl_common.h"
/* recovered: named members + shared header, vtable identified */
/* vtable identified: VT0 = _ZTV7daDsn_c; VT1 = _ZTV10dBgActor_c */
extern void _ZN15TextureSequenceD1Ev(void *);
int *_ZN6ThwompD1Ev(int *t)
{
    t[0] = (int)_ZTV7daDsn_c;
    t[0] = (int)_ZTV10dBgActor_c;
    _ZN11ShadowModelD1Ev((char *)t + 0x338);
    _ZN15TextureSequenceD1Ev((char *)t + 0x324);
    t[0] = (int)VT2;
    _ZN18MovingMeshColliderD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    return t;
}
