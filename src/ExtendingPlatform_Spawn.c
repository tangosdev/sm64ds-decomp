// @symbol ExtendingPlatform_Spawn
/* recovered: vtable identified, globals resolved, declarations from a shared header */
#include "decl_Actor.h"
#include "decl_ActorBase.h"
#include "decl_ExtendingMeshCollider.h"
#include "decl_Model.h"
#include "decl_common.h"
/* recovered: vtable identified, globals resolved */
/* resolved: VT0 = _ZTV8PoleLift */
int *ExtendingPlatform_Spawn(void)
{
    int *p = (int *)_ZN9ActorBasenwEj(808);
    if (p) {
        _ZN5ActorC2Ev(p);
        p[0] = (int)_ZTV8PoleLift;
        _ZN5ModelC1Ev((char *)p + 0xd8);
        _ZN21ExtendingMeshColliderC1Ev((char *)p + 0x158);
    }
    return p;
}
