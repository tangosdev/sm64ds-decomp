#include "nitro/fx/fx.h"
#include "types.h"

extern s16 data_02082214[];
void Vec3_RotateYAndTranslate(Vector3 *out, const Vector3 *trans, short angle, const Vector3 *v) {
    int i = (((unsigned short)angle) >> 4) * 2;
    int c = data_02082214[i];
    int s = data_02082214[i + 1];
    int new_var;
    new_var = FX_MUL(v->x, s);
    out->x = trans->x + (FX_MUL(v->z, c) + new_var);
    out->y = trans->y + v->y;
    out->z = trans->z + (FX_MUL(v->z, s) - FX_MUL(v->x, c));
}
