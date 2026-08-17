// @symbol _ZN9PushBlockD1Ev
/* recovered: vtable identified, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_Model.h"
#include "decl_MovingMeshCollider.h"
#include "decl_WithMeshClsn.h"
#include "decl_common.h"
extern int _ZTV9PushBlock[];
extern int _ZTV10dBgActor_c[];
/* recovered: vtable identified */
/* vtable identified: VT0 = _ZTV9PushBlock; VT1 = _ZTV10dBgActor_c */
int *_ZN9PushBlockD1Ev(int *t)
{
    t[0] = (int)_ZTV9PushBlock;
    _ZN12WithMeshClsnD1Ev((char *)t + 0x320);
    t[0] = (int)_ZTV10dBgActor_c;
    _ZN18MovingMeshColliderD1Ev((char *)t + 0x124);
    _ZN5ModelD1Ev((char *)t + 0xd4);
    _ZN5ActorD2Ev(t);
    return t;
}
