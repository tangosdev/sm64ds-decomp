#include "math/Matrix.hpp"

struct Quaternion {
    Fix12i x, y, z, w;
};

extern void Matrix3x3_FromQuaternion(
    const struct Quaternion *q,
    struct Matrix3x3 *mF
);

void Matrix4x3_FromQuaternion(const struct Quaternion* q, struct Matrix4x3* mF) {
    Matrix3x3_FromQuaternion(q, (struct Matrix3x3 *)mF);
    mF->m[MATRIX4X3_TRANSLATION_X] = 0;
    mF->m[MATRIX4X3_TRANSLATION_Y] = 0;
    mF->m[MATRIX4X3_TRANSLATION_Z] = 0;
}
