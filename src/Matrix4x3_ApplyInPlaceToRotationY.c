/* Matrix4x3_ApplyInPlaceToRotationY at 0x0203bfc0
 * Builds a rotation-Y matrix on the stack, then multiplies mF by it in-place.
 */

#include "math/Matrix.h"
#include "math/MtxStack.h"

void Matrix4x3_ApplyInPlaceToRotationY(union Matrix4x3 *mF, s16 angY) {
    union Matrix4x3 rot;
    Matrix4x3_FromRotationY(&rot, angY);
    MTX_Concat43(&rot, mF, mF);
}
