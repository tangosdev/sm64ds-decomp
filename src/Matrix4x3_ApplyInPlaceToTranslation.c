/* Matrix4x3_ApplyInPlaceToTranslation at 0x0203c184
 * Builds a translation matrix on the stack, then multiplies mF by it in-place.
 */

#include "common.h"

extern void Matrix4x3_FromTranslation(struct Matrix4x3 *mF, Fix12i x, Fix12i y, Fix12i z);
/* local extern: MtxStack.h is C++ only; the helper definition takes words. */
extern void MulMat4x3Mat4x3(const int *m1, const int *m0, int *mF);

void Matrix4x3_ApplyInPlaceToTranslation(struct Matrix4x3 *mF, Fix12i x, Fix12i y, Fix12i z)
{
    struct Matrix4x3 tmp;
    Matrix4x3_FromTranslation(&tmp, x, y, z);
    MulMat4x3Mat4x3((const int *)&tmp, (const int *)mF, (int *)mF);
}
