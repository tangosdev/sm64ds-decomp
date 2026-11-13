#include "types.h"
#include "nitro/cp/divider.h"
#include "nitro/cp/sqrt.h"

/* v / |v|: 1 / |v|^2 in the divider while the square-root unit takes |v|,
   then each component times their product (NitroSDK fx_vec.c). */
void VEC_Normalize(const Vector3 *v, Vector3 *out) {
    s64 sq;
    int len;
    s64 recip;
    s64 scale;
    s64 prod;

    sq = (s64)v->x * v->x;
    sq += (s64)v->y * v->y;
    sq += (s64)v->z * v->z;

    CP_SetDiv64_64((u64)0x1000000 << 32, sq);
    sq <<= 2;
    CP_SetSqrt64(sq);

    len = CP_GetSqrtResult32();
    recip = CP_GetDivResult64();

    scale = recip * len;

    prod = scale * v->x;
    out->x = ((prod + (1LL << 44)) >> 45);
    prod = scale * v->y;
    out->y = ((prod + (1LL << 44)) >> 45);
    prod = scale * v->z;
    out->z = ((prod + (1LL << 44)) >> 45);
}
