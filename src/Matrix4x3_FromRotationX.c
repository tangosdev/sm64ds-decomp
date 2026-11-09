#include "common.h"
#include "nitro/fx/fx_trig.h"

extern void func_02052800(Matrix4x3 *m, int s, int c);

void Matrix4x3_FromRotationX(Matrix4x3 *m, int angle) {
    int a = (unsigned short)angle >> 4;
    func_02052800(m, FX_SinIdx(a), FX_CosIdx(a));
}
