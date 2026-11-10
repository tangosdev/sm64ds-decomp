#include "common.h"
#include "math/MtxStack.h"

void Matrix4x3_ApplyInPlaceToRotationZXYExt(Matrix4x3 *m, int x, int y, int z) {
    Matrix4x3 tmp;
    if (y != 0) {
        Matrix4x3_FromRotationY(&tmp, y);
        MTX_Concat43(&tmp, m, m);
    }
    if (x != 0) {
        Matrix4x3_FromRotationX(&tmp, x);
        MTX_Concat43(&tmp, m, m);
    }
    if (z != 0) {
        Matrix4x3_FromRotationZ(&tmp, z);
        MTX_Concat43(&tmp, m, m);
    }
}
