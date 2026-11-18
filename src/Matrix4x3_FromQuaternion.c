#include "math/Matrix.h"
#include "math/Quaternion.h"

extern void Matrix3x3_FromQuaternion(
    const struct Quaternion *q,
    struct Matrix3x3 *mF
);

void Matrix4x3_FromQuaternion(const struct Quaternion* q, struct Matrix4x3* mF) {
    /* The rotation is the 4x3's first nine words, laid out as a 3x3. */
    Matrix3x3_FromQuaternion(q, (struct Matrix3x3 *)mF);
    mF->m[9] = 0;
    mF->m[10] = 0;
    mF->m[11] = 0;
}
