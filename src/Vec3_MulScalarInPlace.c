#include "nitro/fx/fx.h"
#include "types.h"

void Vec3_MulScalarInPlace(Vector3 *v, int s)
{
    v->x = FX_MUL(v->x, s);
    v->y = FX_MUL(v->y, s);
    v->z = FX_MUL(v->z, s);
}
