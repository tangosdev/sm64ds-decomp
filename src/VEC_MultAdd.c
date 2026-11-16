#include "types.h"
#include "nitro/cp/divider.h"
#include "nitro/cp/sqrt.h"
#include "nitro/fx/fx_vec.h"

/* VEC_Normalize for a 4.12 vector (NitroSDK fx_vec.c). */
void VEC_Fx16Normalize(const VecFx16 *in, VecFx16 *out) {
    u64 sq;
    int sqrtResult;
    s64 divResult;
    s64 scale;
    s64 prod;

    sq = (in->x * in->x);
    sq += (in->y * in->y);
    sq += (in->z * in->z);

    CP_SetDiv64_64((u64)0x1000000 << 32, sq);
    CP_SetSqrt64(sq << 2);

    sqrtResult = CP_GetSqrtResult32();
    divResult = CP_GetDivResult64();

    scale = divResult * (s64)sqrtResult;

    prod = scale * (s64)in->x;
    out->x = ((prod + (1LL << 44)) >> 45);
    prod = scale * (s64)in->y;
    out->y = ((prod + (1LL << 44)) >> 45);
    prod = scale * (s64)in->z;
    out->z = ((prod + (1LL << 44)) >> 45);
}

/* out = s * a + b, per axis. */
void VEC_MultAdd(fx32 s, const VecFx32 *a, const VecFx32 *b, VecFx32 *out) {
    out->x = b->x + (fx32)(((fx64)s * a->x) >> FX32_SHIFT);
    out->y = b->y + (fx32)(((fx64)s * a->y) >> FX32_SHIFT);
    out->z = b->z + (fx32)(((fx64)s * a->z) >> FX32_SHIFT);
}
