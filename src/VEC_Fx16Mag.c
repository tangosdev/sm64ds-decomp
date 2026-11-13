#include "types.h"
#include "nitro/cp/sqrt.h"

/* |v| for a 4.12 vector: the squared length through the hardware square
   root (NitroSDK fx_vec.c). */
int VEC_Fx16Mag(s16 *v) {
    s64 sq = v[0] * v[0];
    sq += v[1] * v[1];
    sq += v[2] * v[2];
    sq <<= 2;
    CP_SetSqrt64(sq);
    return ((s32)CP_GetSqrtResult32() + 1) >> 1;
}
