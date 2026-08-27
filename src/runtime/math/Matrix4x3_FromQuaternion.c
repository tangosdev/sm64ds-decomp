#include "math/Matrix.h"

struct Quaternion {
    Fix12i x, y, z, w;
};

extern void Matrix3x3_FromQuaternion(
    const struct Quaternion *q,
    struct Matrix3x3 *mF
);

void Matrix4x3_FromQuaternion(const struct Quaternion* q, struct Matrix4x3* mF) {
    Matrix3x3_FromQuaternion(q, &mF->r);
    mF->t.x = 0;
    mF->t.y = 0;
    mF->t.z = 0;
}
