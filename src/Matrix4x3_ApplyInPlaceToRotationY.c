/* Matrix4x3_ApplyInPlaceToRotationY at 0x0203bfc0
 * Builds a rotation-Y matrix on the stack, then multiplies mF by it in-place.
 */

#include "math/Matrix.h"

extern void Matrix4x3_FromRotationY(struct Matrix4x3 *mF, s16 angY);
/* local extern: MtxStack.h is C++ only; the helper definition takes words. */
extern void MulMat4x3Mat4x3(const int *m1, const int *m0, int *mF);

void Matrix4x3_ApplyInPlaceToRotationY(struct Matrix4x3 *mF, s16 angY)
{
    struct Matrix4x3 tmp;
    Matrix4x3_FromRotationY(&tmp, angY);
    MulMat4x3Mat4x3((const int *)&tmp, (const int *)mF, (int *)mF);
}
