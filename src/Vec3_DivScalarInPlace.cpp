//cpp
#include "cstd.h"

extern "C" {

typedef int s32;

s32 *Vec3_DivScalarInPlace(s32 *v, s32 s)
{
    if (s == 0) {
        if (v[0] > 0) v[0] = 0x7fffffff;
        else if (v[0] < 0) v[0] = (s32)0x80000000;
        if (v[1] > 0) v[1] = 0x7fffffff;
        else if (v[1] < 0) v[1] = (s32)0x80000000;
        if (v[2] > 0) v[2] = 0x7fffffff;
        else if (v[2] < 0) v[2] = (s32)0x80000000;
    } else {
        v[0] = cstd::fdiv(v[0], s);
        v[1] = cstd::fdiv(v[1], s);
        v[2] = cstd::fdiv(v[2], s);
    }
    return v;
}
}
