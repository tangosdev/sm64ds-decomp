#include "types.h"
/* MeshColliderBase::GetAngularVelY() at 0x02039428
 * Base implementation: a non-moving mesh collider has no angular velocity,
 * so this always returns 0. (MovingMeshCollider overrides it.)
 */
struct MeshColliderBase;

u16 _ZN16MeshColliderBase14GetAngularVelYEv(struct MeshColliderBase* self)
{
    (void)self;
    return 0;
}
