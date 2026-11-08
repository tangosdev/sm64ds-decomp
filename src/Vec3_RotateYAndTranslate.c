#include "nitro/fx/fx_trig.h"
#include "types.h"

void Vec3_RotateYAndTranslate(Vector3 *out, const Vector3 *trans, short angle, const Vector3 *v) {
    fx16 s = FX_SinIdx((u16)angle >> 4);
    fx16 c = FX_CosIdx((u16)angle >> 4);
    out->x = trans->x + (FX_MUL(v->x, c) + FX_MUL(v->z, s));
    out->y = trans->y + v->y;
    out->z = trans->z + (FX_MUL(v->z, c) - FX_MUL(v->x, s));
}
