// @symbol func_ov007_020c39f8
/* recovered: shared common types */
#include "common.h"
extern short data_02082214[];
extern void func_0205283c(struct Matrix4x3 *out, short a, short b);
extern void func_02052820(struct Matrix4x3 *out, short a, short b);
extern void func_02052800(struct Matrix4x3 *out, short a, short b);
extern void MulMat4x3Mat4x3(struct Matrix4x3 *dst, struct Matrix4x3 *a, struct Matrix4x3 *b);

void func_ov007_020c39f8(struct Matrix4x3 *mat, int rx, int ry, int rz) {
    struct Matrix4x3 t;
    int ix = (rx >> 4) * 2;
    int iy = (ry >> 4) * 2;
    int iz = (rz >> 4) * 2;
    func_0205283c(&t, data_02082214[iz], data_02082214[iz + 1]);
    MulMat4x3Mat4x3(mat, &t, mat);
    func_02052820(&t, data_02082214[iy], data_02082214[iy + 1]);
    MulMat4x3Mat4x3(mat, &t, mat);
    func_02052800(&t, data_02082214[ix], data_02082214[ix + 1]);
    MulMat4x3Mat4x3(mat, &t, mat);
}
