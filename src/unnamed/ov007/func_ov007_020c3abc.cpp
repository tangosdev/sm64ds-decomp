//cpp
#include "math/Matrix.h"
#include "nitro/fx/fx_trig.h"

extern "C" {
extern void MTX_RotX33_(Matrix3x3 *m, int s, int c);
extern void MTX_RotY33_(Matrix3x3 *m, int s, int c);
extern void MTX_RotZ33_(Matrix3x3 *m, int s, int c);
extern void MTX_Concat33(const int *A, const int *B, int *dst);

void func_ov007_020c3abc(Matrix3x3 *out, int ax, int ay, int az) {
    Matrix3x3 rot;
    int idx;
    if (ax != 0) {
        idx = ax >> 4;
        MTX_RotX33_(&rot, FX_SinIdx(idx), FX_CosIdx(idx));
        MTX_Concat33((const int *)out, &rot.m[0][0], (int *)out);
    }
    if (ay != 0) {
        idx = ay >> 4;
        MTX_RotY33_(&rot, FX_SinIdx(idx), FX_CosIdx(idx));
        MTX_Concat33((const int *)out, &rot.m[0][0], (int *)out);
    }
    if (az != 0) {
        idx = az >> 4;
        MTX_RotZ33_(&rot, FX_SinIdx(idx), FX_CosIdx(idx));
        MTX_Concat33((const int *)out, &rot.m[0][0], (int *)out);
    }
}
}
