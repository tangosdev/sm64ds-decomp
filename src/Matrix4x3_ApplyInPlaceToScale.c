/* Matrix4x3_ApplyInPlaceToScale at 0x0203c148
 * Builds a scale matrix on the stack (Thumb), then multiplies mF by it in-place.
 */

#include "common.h"

extern void func_020527e8(struct Matrix4x3 *mF, Fix12i x, Fix12i y, Fix12i z) __attribute__((long_call, target("thumb")));
/* local extern: MtxStack.h is C++ only; the helper definition takes words. */
extern void MulMat4x3Mat4x3(const int *m1, const int *m0, int *mF);

void Matrix4x3_ApplyInPlaceToScale(struct Matrix4x3 *mF, Fix12i x, Fix12i y, Fix12i z)
{
    struct Matrix4x3 tmp;
    func_020527e8(&tmp, x, y, z);
    MulMat4x3Mat4x3((const int *)&tmp, (const int *)mF, (int *)mF);
}
