/* Matrix3x3_FromQuaternion @ 0x020301e8 (arm9, size 0x268)
 * Builds a 3x3 Fix12 rotation matrix from quaternion {x,y,z,w}.
 */
typedef long long s64;

#define FMUL(a, b) ((int)(((s64)(a) * (b) + 0x800) >> 12))

void Matrix3x3_FromQuaternion(int *q, int *m)
{
    m[0] = 0x1000 - ((FMUL(q[1], q[1]) + FMUL(q[2], q[2])) << 1);
    m[1] = (FMUL(q[0], q[1]) + FMUL(q[3], q[2])) << 1;
    m[2] = (FMUL(q[0], q[2]) - FMUL(q[3], q[1])) << 1;
    m[3] = (FMUL(q[0], q[1]) - FMUL(q[3], q[2])) << 1;
    m[4] = 0x1000 - ((FMUL(q[0], q[0]) + FMUL(q[2], q[2])) << 1);
    m[5] = (FMUL(q[1], q[2]) + FMUL(q[3], q[0])) << 1;
    m[6] = (FMUL(q[3], q[1]) + FMUL(q[0], q[2])) << 1;
    m[7] = (FMUL(q[1], q[2]) - FMUL(q[3], q[0])) << 1;
    m[8] = 0x1000 - ((FMUL(q[0], q[0]) + FMUL(q[1], q[1])) << 1);
}
