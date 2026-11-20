//cpp
/* func_0203d480 at 0x0203d480
 * Normalizes a 2D fx32 vector in place (divides by its magnitude),
 * then copies the result to an output array.
 */
#include "Vector2.h"
#include "Vector2Math.h"
#include "nitro/fx/fx_cp.h"

extern "C" {

void func_0203d480(Vector2 *out, Vector2 *v) {
    fx32 mag;
    mag = Vec2_Len(v);
    v->x = FX_Div(v->x, mag);
    v->y = FX_Div(v->y, mag);
    out->x = v->x;
    out->y = v->y;
}
}
