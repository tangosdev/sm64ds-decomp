#include "nitro/fx/fx.h"
typedef struct { int x, y, z; } Vec3_Fix12;

void Vec3_MulScalar(Vec3_Fix12 *out, const Vec3_Fix12 *in, int scale)
{
    int rz = FX_MUL(in->z, scale);
    int ry = FX_MUL(in->y, scale);
    int rx = FX_MUL(in->x, scale);
    out->x = rx;
    out->y = ry;
    out->z = rz;
}
