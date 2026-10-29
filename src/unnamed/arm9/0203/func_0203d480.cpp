//cpp
/* func_0203d480 at 0x0203d480
 * Normalizes a 2D Fix12i vector in place (divides by its magnitude),
 * then copies the result to an output array.
 */
#include "cstd.h"

extern "C" {

typedef int Fix12i;

extern Fix12i Vec2_Len(Fix12i *vec2); /* sqrt(vec2[0]^2 + vec2[1]^2): 0x0203d614 */

void func_0203d480(Fix12i *out, Fix12i *in)
{
    Fix12i mag;
    mag = Vec2_Len(in);
    in[0] = cstd::fdiv(in[0], mag);
    in[1] = cstd::fdiv(in[1], mag);
    out[0] = in[0];
    out[1] = in[1];
}
}
