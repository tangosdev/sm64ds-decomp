/* Matrix4x3_ApplyInPlaceToTranslation at 0x0203c184
 * Builds a translation matrix on the stack, then multiplies mF by it in-place.
 */

#include "common.h"
#include "math/MtxStack.h"

void Matrix4x3_ApplyInPlaceToTranslation(struct Matrix4x3 *mF, fx32 x, fx32 y, fx32 z)
{
    struct Matrix4x3 tmp;
    Matrix4x3_FromTranslation(&tmp, x, y, z);
    MTX_Concat43(&tmp, mF, mF);
}
