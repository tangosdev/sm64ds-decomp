//cpp
// @symbol _ZN16MeshColliderBase11GetVelocityER7Vector3
#include "MeshColliderBase.h"

/* A non-moving collider has no velocity. The chained stores mirror the
   ROM's z-then-y-then-x order. */
void MeshColliderBase::GetVelocity(Vector3 &res)
{
    res.z = 0;
    res.y = res.z;
    res.x = res.y;
}
