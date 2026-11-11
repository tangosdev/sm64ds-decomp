#include "common.h"
#include "math/MtxStack.h"

extern Matrix4x3 IDENTITY_MATRIX4X3;

void Matrix4x3_FromRotationXYZExt(Matrix4x3 *m, int x, int y, int z) {
    Matrix4x3 tmp;
    if (z != 0)
        Matrix4x3_FromRotationZ(m, z);
    else
        *m = IDENTITY_MATRIX4X3;
    if (y != 0) {
        Matrix4x3_FromRotationY(&tmp, y);
        MTX_Concat43(&tmp, m, m);
    }
    if (x != 0) {
        Matrix4x3_FromRotationX(&tmp, x);
        MTX_Concat43(&tmp, m, m);
    }
}
