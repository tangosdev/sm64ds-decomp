#include "types.h"
#include "nitro/cp/sqrt.h"
#include "nitro/fx/fx_vec.h"

/* |v| for a 4.12 vector: the squared length through the hardware square
   root (NitroSDK fx_vec.c). */
fx32 VEC_Fx16Mag(const VecFx16 *v) {
    s64 sq = v->x * v->x;
    sq += v->y * v->y;
    sq += v->z * v->z;
    sq <<= 2;
    CP_SetSqrt64(sq);
    return ((s32)CP_GetSqrtResult32() + 1) >> 1;
}
