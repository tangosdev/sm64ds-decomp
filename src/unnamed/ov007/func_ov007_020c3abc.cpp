//cpp
#include "math/Matrix.h"
#include "nitro/fx/fx_trig.h"

extern "C" {
extern void MTX_RotX33_(int *m, int s, int c);
extern void MTX_RotY33_(int *m, int s, int c);
extern void Matrix3x3_SetRotationZ(int *m, int s, int c);
extern void MulMat3x3Mat3x3(const int *A, const int *B, int *dst);

void func_ov007_020c3abc(Matrix3x3 *out, int ax, int ay, int az) {
    Matrix3x3 tmp;
    int idx;
    if (ax != 0) {
        idx = ax >> 4;
        MTX_RotX33_((int *)&tmp, FX_SinIdx(idx), FX_CosIdx(idx));
        MulMat3x3Mat3x3((const int *)out, (const int *)&tmp, (int *)out);
    }
    if (ay != 0) {
        idx = ay >> 4;
        MTX_RotY33_((int *)&tmp, FX_SinIdx(idx), FX_CosIdx(idx));
        MulMat3x3Mat3x3((const int *)out, (const int *)&tmp, (int *)out);
    }
    if (az != 0) {
        idx = az >> 4;
        Matrix3x3_SetRotationZ((int *)&tmp, FX_SinIdx(idx), FX_CosIdx(idx));
        MulMat3x3Mat3x3((const int *)out, (const int *)&tmp, (int *)out);
    }
}
}
