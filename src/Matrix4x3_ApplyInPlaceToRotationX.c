/* Matrix4x3_ApplyInPlaceToRotationX at 0x0203bff0
 * Builds a rotation-X matrix on the stack, then multiplies mF by it in-place.
 */

#include "common.h"
#include "math/MtxStack.h"

void Matrix4x3_ApplyInPlaceToRotationX(union Matrix4x3 *mF, s16 angX) {
    union Matrix4x3 rot;
    Matrix4x3_FromRotationX(&rot, angX);
    MTX_Concat43(&rot, mF, mF);
}
