//cpp
/* NitroSDK's coprocessor divide and square-root routines, FX_cp.c (pret
 * pokediamond arm9/lib/NitroSDK/src/FX_cp.c), arm9 .text 0x02052ef4..
 * 0x02053274, plus VEC_Distance at the top of the run. VEC_Distance is the
 * last live function of fx_vec.c (NitroSDK libraries/fx/src/fx_vec.c; its
 * VEC_Fx16Distance is dead-stripped), so this TU's upper boundary is still
 * the shard fold's and not the SDK's.
 *
 * mwccarm emits .text in reverse source order here, so definitions run from
 * the highest address (VEC_Distance) down to the lowest (FX_ModS32), the
 * order pret's FX_cp.c writes them in. FX_DivFx64c, FX_InvSqrt and
 * FX_SqrtAsync are the later SDK's additions to the same file.
 *
 * These were the coined cstd:: namespace (fdiv, div, mod, ldiv, fdiv_async,
 * reciprocal_async, fdiv_result, ldiv_result) and two func_ names; the ROM
 * calls them from C, so they are C symbols under the SDK's names.
 */
#include "types.h"
#include "nitro/cp/divider.h"
#include "nitro/cp/sqrt.h"
#include "nitro/fx/fx_cp.h"

/* |a - b|: the squared distance through the hardware sqrt unit, rounded
   back to 20.12. */

int VEC_Distance(int *a, int *b) {
    int dx = a[0] - b[0];
    long long sq = (long long)dx * dx;
    int dy = a[1] - b[1];
    int dz;
    sq += (long long)dy * dy;
    dz = a[2] - b[2];
    sq += (long long)dz * dz;
    sq <<= 2;
    CP_SetSqrt64((u64)sq);
    return ((s32)CP_GetSqrtResult32() + 1) >> 1;
}




fx32 FX_Div(fx32 numerator, fx32 denominator) {
    FX_DivAsync(numerator, denominator);
    return FX_GetDivResult();
}




fx64c FX_DivFx64c(fx32 a, fx32 b) {
    FX_DivAsync(a, b);
    return CP_GetDivResult64();
}



fx32 FX_Inv(fx32 x) {
    FX_InvAsync(x);
    return FX_GetDivResult();
}



fx32 FX_Sqrt(fx32 a) {
    if (a > 0) {
        CP_SetSqrt64((u64)a << 32);
        return FX_GetSqrtResult();
    }
    return 0;
}



fx32 FX_InvSqrt(fx32 x) {
    s64 quotient;

    if (x > 0) {
        FX_InvAsync(x);
        FX_SqrtAsync(x);
        quotient = FX_GetDivResultFx64c();
        CP_WaitSqrt();

        /* The register read, not CP_GetSqrtResultImm32(): the inline's
           return temporary takes a different register pair. */
        return (s32)((quotient * (s32)REG_SQRT_RESULT + (s64)0x200 * (1LL << 32)) >> (32 + 10));
    }

    return 0;
}




fx64c FX_GetDivResultFx64c(void) {
    CP_WaitDiv();
    return CP_GetDivResultImm64();
}




fx32 FX_GetDivResult(void) {
    CP_WaitDiv();
    return (fx32)((CP_GetDivResultImm64() + 0x80000) >> 20);
}



void FX_InvAsync(fx32 a) {
    CP_SetDiv64_32((u64)0x1000 << 32, (u32)a);
}



void FX_SqrtAsync(fx32 n) {
    if (n > 0) {
        CP_SetSqrt64((u64)n << 32);
    } else {
        CP_SetSqrt64(0);
    }
}



fx32 FX_GetSqrtResult(void) {
    CP_WaitSqrt();
    return (fx32)((CP_GetSqrtResultImm32() + 0x200) >> 10);
}



void FX_DivAsync(fx32 a, fx32 b) {
    CP_SetDiv64_32((u64)(u32)a << 32, (u32)b);
}




s32 FX_DivS32(s32 a, s32 b) {
    CP_SetDiv32_32((u32)a, (u32)b);
    return CP_GetDivResult32();
}




s32 FX_ModS32(s32 a, s32 b) {
    CP_SetDiv32_32((u32)a, (u32)b);
    return CP_GetDivRemainder32();
}
