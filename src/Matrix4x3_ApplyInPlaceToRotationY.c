/* Matrix4x3_ApplyInPlaceToRotationY at 0x0203bfc0
 * Builds a rotation-Y matrix on the stack, then multiplies mF by it in-place.
 */

#include "math/Matrix.h"

extern void Matrix4x3_FromRotationY(struct Matrix4x3 *mF, s16 angY);
extern void MulMat4x3Mat4x3(const struct Matrix4x3 *m1, const struct Matrix4x3 *m0, struct Matrix4x3 *mF);

void Matrix4x3_ApplyInPlaceToRotationY(struct Matrix4x3 *mF, s16 angY)
{
    struct Matrix4x3 tmp;
    Matrix4x3_FromRotationY(&tmp, angY);
    MulMat4x3Mat4x3(&tmp, mF, mF);
}
