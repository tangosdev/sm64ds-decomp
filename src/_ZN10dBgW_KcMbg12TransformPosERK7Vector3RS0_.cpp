//cpp
// @symbol _ZN18MovingMeshCollider12TransformPosERK7Vector3RS0_
#include "MovingMeshCollider.h"
extern "C" void MulVec3Mat4x3(const Vector3 *v, const Matrix4x3 *m, Vector3 *res);

/* Out of the previous frame's space, into the current one. */
int MovingMeshCollider::TransformPos(const Vector3 &pos, Vector3 &res)
{
    Vector3 tmp;
    MulVec3Mat4x3(&pos, &invMat, &tmp);
    MulVec3Mat4x3(&tmp, &mat, &res);
    return 1;
}
