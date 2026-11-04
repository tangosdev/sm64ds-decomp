/* Matrix4x3_ApplyInPlaceToRotationZ at 0x0203bf90
 * Builds a rotation-Z matrix on the stack, then multiplies mF by it in-place.
 */

#include "common.h"

extern void Matrix4x3_FromRotationZ(struct Matrix4x3 *mF, s16 angZ);
/* local extern: MtxStack.h is C++ only; the helper definition takes words. */
extern void MulMat4x3Mat4x3(const int *m1, const int *m0, int *mF);

void Matrix4x3_ApplyInPlaceToRotationZ(struct Matrix4x3 *mF, s16 angZ)
{
    struct Matrix4x3 tmp;
    Matrix4x3_FromRotationZ(&tmp, angZ);
    MulMat4x3Mat4x3((const int *)&tmp, (const int *)mF, (int *)mF);
}
