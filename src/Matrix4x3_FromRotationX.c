#include "common.h"
#include "nitro/fx/fx_trig.h"
#include "math/MtxStack.h"

void Matrix4x3_FromRotationX(Matrix4x3 *m, int angle) {
    int a = (unsigned short)angle >> 4;
    MTX_RotX43_(m, FX_SinIdx(a), FX_CosIdx(a));
}
