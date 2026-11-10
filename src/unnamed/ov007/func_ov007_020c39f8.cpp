//cpp
#include "common.h"
#include "math/MtxStack.h"

extern "C" {
/* recovered: shared common types */
extern short data_02082214[];
extern void MTX_RotZ43_(struct Matrix4x3 *out, int s, int c);
extern void MTX_RotY43_(struct Matrix4x3 *out, int s, int c);
extern void MTX_RotX43_(struct Matrix4x3 *out, int s, int c);

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
    MTX_RotZ43_(&t, data_02082214[iz], data_02082214[iz + 1]);
    MTX_Concat43(mat, &t, mat);
    MTX_RotY43_(&t, data_02082214[iy], data_02082214[iy + 1]);
    MTX_Concat43(mat, &t, mat);
    MTX_RotX43_(&t, data_02082214[ix], data_02082214[ix + 1]);
    MTX_Concat43(mat, &t, mat);
}
}
