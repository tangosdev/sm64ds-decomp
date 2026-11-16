#include "types.h"
#include "nitro/cp/sqrt.h"
#include "nitro/fx/fx_vec.h"

/* |v|: the squared length through the hardware square root, rounded back to
   20.12 (NitroSDK fx_vec.c). */
fx32 VEC_Mag(const VecFx32 *v) {
    s64 sq = (s64)v->x * v->x;
    sq += (s64)v->y * v->y;
    sq += (s64)v->z * v->z;
    sq <<= 2;
    CP_SetSqrt64((u64)sq);
    return ((s32)CP_GetSqrtResult32() + 1) >> 1;
}
