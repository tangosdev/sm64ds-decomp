#include "types.h"
#include "nitro/cp/divider.h"
#include "nitro/cp/sqrt.h"

/* VEC_Normalize for a 4.12 vector (NitroSDK fx_vec.c). */
void VEC_Fx16Normalize(s16 *in, s16 *out) {
    u64 sq;
    int sqrtResult;
    s64 divResult;
    s64 scale;
    s64 prod;

    sq = (in[0] * in[0]);
    sq += (in[1] * in[1]);
    sq += (in[2] * in[2]);

    CP_SetDiv64_64((u64)0x1000000 << 32, sq);
    CP_SetSqrt64(sq << 2);

    sqrtResult = CP_GetSqrtResult32();
    divResult = CP_GetDivResult64();

    scale = divResult * (s64)sqrtResult;

    prod = scale * (s64)in[0];
    out[0] = ((prod + (1LL << 44)) >> 45);
    prod = scale * (s64)in[1];
    out[1] = ((prod + (1LL << 44)) >> 45);
    prod = scale * (s64)in[2];
    out[2] = ((prod + (1LL << 44)) >> 45);
}

void VEC_MultAdd(int s, int *a, int *b, int *out) {
    out[0] = b[0] + (int)(((long long)s * a[0]) >> 12);
    out[1] = b[1] + (int)(((long long)s * a[1]) >> 12);
    out[2] = b[2] + (int)(((long long)s * a[2]) >> 12);
}
