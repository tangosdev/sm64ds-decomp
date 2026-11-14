//cpp
#include "types.h"
#include "nitro/fx/fx_cp.h"


Vector3 *Vec3_DivScalarInPlace(Vector3 *v, int s) {
    if (s == 0) {
        if (v->x > 0)
            v->x = 0x7fffffff;
        else if (v->x < 0)
            v->x = (s32)0x80000000;
        if (v->y > 0)
            v->y = 0x7fffffff;
        else if (v->y < 0)
            v->y = (s32)0x80000000;
        if (v->z > 0)
            v->z = 0x7fffffff;
        else if (v->z < 0)
            v->z = (s32)0x80000000;
    } else {
        v->x = FX_Div(v->x, s);
        v->y = FX_Div(v->y, s);
        v->z = FX_Div(v->z, s);
    }
    return v;
}
