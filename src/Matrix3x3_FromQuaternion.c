#include "nitro/fx/fx.h"
#include "math/Matrix.h"
#include "math/Quaternion.h"
/* Matrix3x3_FromQuaternion @ 0x020301e8 (arm9, size 0x268)
 * Builds a 3x3 fx32 rotation matrix from quaternion {x,y,z,w}.
 */

void Matrix3x3_FromQuaternion(const struct Quaternion *q, struct Matrix3x3 *mF) {
    mF->m[0] = 0x1000 - ((FX_MUL(q->y, q->y) + FX_MUL(q->z, q->z)) << 1);
    mF->m[1] = (FX_MUL(q->x, q->y) + FX_MUL(q->w, q->z)) << 1;
    mF->m[2] = (FX_MUL(q->x, q->z) - FX_MUL(q->w, q->y)) << 1;
    mF->m[3] = (FX_MUL(q->x, q->y) - FX_MUL(q->w, q->z)) << 1;
    mF->m[4] = 0x1000 - ((FX_MUL(q->x, q->x) + FX_MUL(q->z, q->z)) << 1);
    mF->m[5] = (FX_MUL(q->y, q->z) + FX_MUL(q->w, q->x)) << 1;
    mF->m[6] = (FX_MUL(q->w, q->y) + FX_MUL(q->x, q->z)) << 1;
    mF->m[7] = (FX_MUL(q->y, q->z) - FX_MUL(q->w, q->x)) << 1;
    mF->m[8] = 0x1000 - ((FX_MUL(q->x, q->x) + FX_MUL(q->y, q->y)) << 1);
}
