//cpp
// @symbol _ZN18MovingMeshCollider11GetVelocityER7Vector3
#include "MovingMeshCollider.h"

void MovingMeshCollider::GetVelocity(Vector3 &res)
{
    res.x = velocity.x;
    res.y = velocity.y;
    res.z = velocity.z;
}
