/* One of the arm9 Vector3 helpers declared in include/math/CalcLib.h: shift v
   right per axis, in place. */
#include "types.h"

int *Vec3_AsrInPlace(Vector3 *v, int sh) {
    v->x >>= sh;
    v->y >>= sh;
    v->z >>= sh;
    return &v->x;
}
