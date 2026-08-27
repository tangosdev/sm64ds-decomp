//cpp
// @symbol _ZN10dBgW_KcMbg12TransformPosERK7Vector3RS0_
#include "dBgW_KcMbg.h"
extern "C" void MulVec3Mat4x3(const Vector3 *v, const Matrix4x3 *m, Vector3 *res);

/* Out of the previous frame's space, into the current one. */
int dBgW_KcMbg::TransformPos(const Vector3 &pos, Vector3 &res)
{
    Vector3 tmp;
    MulVec3Mat4x3(&pos, &invMat, &tmp);
    MulVec3Mat4x3(&tmp, &mat, &res);
    return 1;
}
