//cpp
#include "common.h"
#include "math/MtxStack.h"

extern "C" {
/* recovered: shared common types */
extern short data_02082214[];
extern void func_0205283c(struct Matrix4x3 *out, int s, int c);
extern void func_02052820(struct Matrix4x3 *out, int s, int c);
extern void func_02052800(struct Matrix4x3 *out, int s, int c);

/* The table reads stay hand-indexed: the ROM doubles all three indices up
   front, before the first call. Through FX_SinIdx/FX_CosIdx
   (nitro/fx/fx_trig.h) each index is doubled at its own call instead, and
   the function grows by 8 bytes, whether the shifted angles are locals (in
   either order, declared together or at each call) or inline arguments. */
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
}
