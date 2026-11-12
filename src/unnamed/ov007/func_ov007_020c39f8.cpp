//cpp
#include "common.h"
#include "math/MtxStack.h"
#include "nitro/fx/fx_trig.h"

extern "C" {
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
    MTX_RotZ43_(&t, FX_SinCosTable_[iz], FX_SinCosTable_[iz + 1]);
    MTX_Concat43(mat, &t, mat);
    MTX_RotY43_(&t, FX_SinCosTable_[iy], FX_SinCosTable_[iy + 1]);
    MTX_Concat43(mat, &t, mat);
    MTX_RotX43_(&t, FX_SinCosTable_[ix], FX_SinCosTable_[ix + 1]);
    MTX_Concat43(mat, &t, mat);
}
}
