/* Matrix4x3_ApplyInPlaceToRotationZ at 0x0203bf90
 * Builds a rotation-Z matrix on the stack, then multiplies mF by it in-place.
 */

#include "common.h"
#include "math/MtxStack.h"

void Matrix4x3_ApplyInPlaceToRotationZ(union Matrix4x3 *mF, s16 angZ) {
    union Matrix4x3 rot;
    Matrix4x3_FromRotationZ(&rot, angZ);
    MTX_Concat43(&rot, mF, mF);
}
