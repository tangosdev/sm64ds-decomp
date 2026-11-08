#include "common.h"
#include "nitro/fx/fx_trig.h"

extern void func_0205283c(Matrix4x3 *m, int s, int c);

void Matrix4x3_FromRotationZ(Matrix4x3 *m, int angle) {
    int a = (unsigned short)angle >> 4;
    func_0205283c(m, FX_SinIdx(a), FX_CosIdx(a));
}
