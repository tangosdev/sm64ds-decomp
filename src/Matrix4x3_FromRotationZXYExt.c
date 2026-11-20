#include "common.h"
#include "math/MtxStack.h"

void Matrix4x3_FromRotationZXYExt(Matrix4x3 *m, int x, int y, int z) {
    Matrix4x3 rot;
    if (y != 0)
        Matrix4x3_FromRotationY(m, y);
    else
        *m = IDENTITY_MATRIX4X3;
    if (x != 0) {
        Matrix4x3_FromRotationX(&rot, x);
        MTX_Concat43(&rot, m, m);
    }
    if (z != 0) {
        Matrix4x3_FromRotationZ(&rot, z);
        MTX_Concat43(&rot, m, m);
    }
}
