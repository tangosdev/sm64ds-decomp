#include "nitro/fx/fx.h"
#include "types.h"

void Vec3_MulScalar(Vector3 *out, const Vector3 *in, int scale)
{
    int rz = FX_MUL(in->z, scale);
    int ry = FX_MUL(in->y, scale);
    int rx = FX_MUL(in->x, scale);
    out->x = rx;
    out->y = ry;
    out->z = rz;
}
