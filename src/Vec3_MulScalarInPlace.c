#include "nitro/fx/fx.h"
typedef long long s64;

void Vec3_MulScalarInPlace(int *v, int s)
{
    v[0] = FX_MUL(v[0], s);
    v[1] = FX_MUL(v[1], s);
    v[2] = FX_MUL(v[2], s);
}
