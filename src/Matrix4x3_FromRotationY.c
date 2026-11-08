#include "common.h"
#include "nitro/fx/fx_trig.h"

extern void func_02052820(Matrix4x3 *m, int s, int c);

void Matrix4x3_FromRotationY(Matrix4x3 *m, int angle) {
    int a = (unsigned short)angle >> 4;
    func_02052820(m, FX_SinIdx(a), FX_CosIdx(a));
}
