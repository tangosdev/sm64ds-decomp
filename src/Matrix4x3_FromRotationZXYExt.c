#include "common.h"
#include "math/MtxStack.h"

extern Matrix4x3 IDENTITY_MATRIX4X3;

void Matrix4x3_FromRotationZXYExt(Matrix4x3 *m, int x, int y, int z) {
    Matrix4x3 tmp;
    if (y != 0)
        Matrix4x3_FromRotationY(m, y);
    else
        *m = IDENTITY_MATRIX4X3;
    if (x != 0) {
        Matrix4x3_FromRotationX(&tmp, x);
        MulMat4x3Mat4x3(&tmp, m, m);
    }
    if (z != 0) {
        Matrix4x3_FromRotationZ(&tmp, z);
        MulMat4x3Mat4x3(&tmp, m, m);
    }
}
