//cpp
/* func_0203d480 at 0x0203d480
 * Normalizes a 2D fx32 vector in place (divides by its magnitude),
 * then copies the result to an output array.
 */
#include "Vector2Math.h"
#include "nitro/fx/fx_cp.h"

extern "C" {

void func_0203d480(fx32 *out, fx32 *in) {
    fx32 mag;
    mag = Vec2_Len((const Vector2 *)in);
    in[0] = FX_Div(in[0], mag);
    in[1] = FX_Div(in[1], mag);
    out[0] = in[0];
    out[1] = in[1];
}
}
