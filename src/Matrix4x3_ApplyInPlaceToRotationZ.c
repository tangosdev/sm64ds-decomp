/* Matrix4x3_ApplyInPlaceToRotationZ at 0x0203bf90
 * Builds a rotation-Z matrix on the stack, then multiplies mF by it in-place.
 */

#include "common.h"
#include "math/MtxStack.h"

void Matrix4x3_ApplyInPlaceToRotationZ(struct Matrix4x3 *mF, s16 angZ)
{
    struct Matrix4x3 tmp;
    Matrix4x3_FromRotationZ(&tmp, angZ);
    MulMat4x3Mat4x3(&tmp, mF, mF);
}
