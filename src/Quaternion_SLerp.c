#include "nitro/fx/fx.h"
typedef long long s64;
void Quaternion_SLerp(int *q0, int *q1, int t, int *out) {
    int u = 0x1000 - t;
    out[0] = FX_MUL(q0[0], u) + FX_MUL(q1[0], t);
    out[1] = FX_MUL(q0[1], u) + FX_MUL(q1[1], t);
    out[2] = FX_MUL(q0[2], u) + FX_MUL(q1[2], t);
    out[3] = FX_MUL(q0[3], u) + FX_MUL(q1[3], t);
}
