/* Matrix4x3_ApplyInPlaceToScale at 0x0203c148
 * Builds a scale matrix on the stack (Thumb), then multiplies mF by it in-place.
 */

#include "common.h"
#include "math/MtxStack.h"

extern void MTX_Scale43_(struct Matrix4x3 *mF, fx32 x, fx32 y, fx32 z)
    __attribute__((long_call, target("thumb")));

void Matrix4x3_ApplyInPlaceToScale(struct Matrix4x3 *mF, fx32 x, fx32 y, fx32 z)
{
    struct Matrix4x3 tmp;
    MTX_Scale43_(&tmp, x, y, z);
    MTX_Concat43(&tmp, mF, mF);
}
