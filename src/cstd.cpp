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
#include "nitro/hw/registers.h"
#include "nitro/fx/fx_cp.h"

/* |a - b|: the squared distance through the hardware sqrt unit, rounded
   back to 20.12. */
extern "C" {
int VEC_Distance(int *a, int *b) {
    volatile unsigned short *imeReg = (volatile unsigned short *)0x4000208;
    volatile unsigned short *sqrtcnt = (volatile unsigned short *)0x40002b0;
    volatile unsigned int *sqrtParam = (volatile unsigned int *)0x40002b8;
    int dx = a[0] - b[0];
    long long sq = (long long)dx * dx;
    int dy = a[1] - b[1];
    int dz;
    unsigned short savedIME;
    sq += (long long)dy * dy;
    dz = a[2] - b[2];
    sq += (long long)dz * dz;
    sq <<= 2;
    savedIME = *imeReg;
    *imeReg = 0;
    *sqrtcnt = 1;
    sqrtParam[0] = (unsigned int)sq;
    sqrtParam[1] = (unsigned int)(sq >> 32);
    *imeReg;
    *imeReg = savedIME;
    while (*sqrtcnt & 0x8000);
    return (*(volatile int *)0x40002b4 + 1) >> 1;
}
}

extern "C" {

fx32 FX_Div(fx32 numerator, fx32 denominator) {
    FX_DivAsync(numerator, denominator);
    return FX_GetDivResult();
}
}

extern "C" {

fx64c FX_DivFx64c(fx32 a, fx32 b) {
    FX_DivAsync(a, b);
    while (*(volatile unsigned short *)0x4000280 & 0x8000)
        ;
    return *(volatile long long *)0x40002a0;
}
}

extern "C" {
fx32 FX_Inv(fx32 x) {
    FX_InvAsync(x);
    return FX_GetDivResult();
}
}

extern "C" {
fx32 FX_Sqrt(fx32 a) {
    volatile unsigned short *imeReg = (volatile unsigned short *)0x4000208;
    volatile int *sqrtParam = (volatile int *)0x40002b8;
    unsigned short savedIME;
    if (a <= 0) return 0;
    savedIME = *imeReg;
    *imeReg = 0;
    *(volatile unsigned short *)0x40002b0 = 1;
    sqrtParam[0] = 0;
    sqrtParam[1] = a;
    *imeReg;
    *imeReg = savedIME;
    return FX_GetSqrtResult();
}
}

extern "C" {
fx32 FX_InvSqrt(fx32 x) {
    s64 quotient;

    if (x > 0) {
        FX_InvAsync(x);
        FX_SqrtAsync(x);
        quotient = FX_GetDivResultFx64c();

        while (REG_SQRTCNT & SQRT_CONTROL_BUSY)
            ;

        return (s32)((quotient * (s64)REG_SQRT_RESULT + (s64)0x200 * (1LL << 32)) >> (32 + 10));
    }

    return 0;
}
}

extern "C" {

fx64c FX_GetDivResultFx64c(void) {
    while (*(volatile unsigned short *)0x4000280 & 0x8000)
        ;
    return *(volatile long long *)0x40002a0;
}
}

extern "C" {

fx32 FX_GetDivResult(void) {
    while (*(volatile unsigned short *)0x4000280 & 0x8000)
        ;
    return (int)((*(volatile long long *)0x40002a0 + 0x80000) >> 20);
}
}

extern "C" {
void FX_InvAsync(fx32 a) {
    *(volatile unsigned short *)0x4000280 = 1;
    *(volatile unsigned long long *)0x4000290 = ((unsigned long long)0x1000) << 32;
    *(volatile unsigned long long *)0x4000298 = (unsigned)a;
}
}

extern "C" {
void FX_SqrtAsync(fx32 n) {
    volatile unsigned short *imeReg = (volatile unsigned short *)0x4000208;
    volatile unsigned short *sqrtcnt = (volatile unsigned short *)0x40002b0;
    volatile unsigned int *sqrtParam = (volatile unsigned int *)0x40002b8;
    unsigned short savedIME;
    if (n > 0) {
        savedIME = *imeReg;
        *imeReg = 0;
        *sqrtcnt = 1;
        sqrtParam[0] = 0;
        sqrtParam[1] = n;
        *imeReg;
        *imeReg = savedIME;
    } else {
        savedIME = *imeReg;
        *imeReg = 0;
        *sqrtcnt = 1;
        sqrtParam[0] = 0;
        sqrtParam[1] = 0;
        *imeReg;
        *imeReg = savedIME;
    }
}
}

extern "C" {
fx32 FX_GetSqrtResult(void) {
    while (*(volatile unsigned short *)0x40002b0 & 0x8000)
        ;
    return (*(volatile unsigned int *)0x40002b4 + 0x200) >> 10;
}
}

extern "C" {
void FX_DivAsync(fx32 a, fx32 b) {
    *(volatile unsigned short *)0x4000280 = 1;
    *(volatile unsigned long long *)0x4000290 = ((unsigned long long)(unsigned)a) << 32;
    *(volatile unsigned long long *)0x4000298 = (unsigned)b;
}
}

extern "C" {

s32 FX_DivS32(s32 a, s32 b) {
    *(volatile unsigned short *)0x4000280 = 0;
    *(volatile int *)0x4000290 = a;
    *(volatile unsigned long long *)0x4000298 = (unsigned)b;
    while (*(volatile unsigned short *)0x4000280 & 0x8000)
        ;
    return *(volatile int *)0x40002a0;
}
}

extern "C" {

s32 FX_ModS32(s32 a, s32 b) {
    *(volatile unsigned short *)0x4000280 = 0;
    *(volatile int *)0x4000290 = a;
    *(volatile unsigned long long *)0x4000298 = (unsigned)b;
    while (*(volatile unsigned short *)0x4000280 & 0x8000)
        ;
    return *(volatile int *)0x40002a8;
}
}
