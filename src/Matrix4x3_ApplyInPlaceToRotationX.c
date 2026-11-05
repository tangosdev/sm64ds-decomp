/* Matrix4x3_ApplyInPlaceToRotationX at 0x0203bff0
 * Builds a rotation-X matrix on the stack, then multiplies mF by it in-place.
 */

#include "common.h"
#include "math/MtxStack.h"

void Matrix4x3_ApplyInPlaceToRotationX(struct Matrix4x3 *mF, s16 angX)
{
    struct Matrix4x3 tmp;
    Matrix4x3_FromRotationX(&tmp, angX);
    MulMat4x3Mat4x3((const int *)&tmp, (const int *)mF, (int *)mF);
}
