#include "nitro/fx/fx.h"
/* Matrix3x3_FromQuaternion @ 0x020301e8 (arm9, size 0x268)
 * Builds a 3x3 Fix12 rotation matrix from quaternion {x,y,z,w}.
 */
typedef long long s64;


void Matrix3x3_FromQuaternion(int *q, int *m)
{
    m[0] = 0x1000 - ((FX_MUL(q[1], q[1]) + FX_MUL(q[2], q[2])) << 1);
    m[1] = (FX_MUL(q[0], q[1]) + FX_MUL(q[3], q[2])) << 1;
    m[2] = (FX_MUL(q[0], q[2]) - FX_MUL(q[3], q[1])) << 1;
    m[3] = (FX_MUL(q[0], q[1]) - FX_MUL(q[3], q[2])) << 1;
    m[4] = 0x1000 - ((FX_MUL(q[0], q[0]) + FX_MUL(q[2], q[2])) << 1);
    m[5] = (FX_MUL(q[1], q[2]) + FX_MUL(q[3], q[0])) << 1;
    m[6] = (FX_MUL(q[3], q[1]) + FX_MUL(q[0], q[2])) << 1;
    m[7] = (FX_MUL(q[1], q[2]) - FX_MUL(q[3], q[0])) << 1;
    m[8] = 0x1000 - ((FX_MUL(q[0], q[0]) + FX_MUL(q[1], q[1])) << 1);
}
