//cpp
#include "common.h"
#include "nitro/hw/registers.h"

#define REG_IME         (*(volatile u16 *)0x04000208)
#define REG_DIVCNT      (*(volatile u16 *)0x04000280)
#define REG_DIV_NUMER   (*(volatile u64 *)0x04000290)
#define REG_DIV_DENOM   (*(volatile u64 *)0x04000298)
#define REG_DIV_RESULT  (*(volatile s64 *)0x040002a0)
#define REG_SQRT_PARAM  (*(volatile u64 *)0x040002b8)

#define DIV_CONTROL_64_64 2
#define DIV_CONTROL_BUSY  (1u << 15)
#define SQRT_CONTROL_64   1

/* NitroSDK's inline IME (os_interrupt.h) and divider/square-root (cp.h) helpers.
 * File-local because the tree has no nitro/os interrupt or nitro/cp header, and an
 * inline body in a shared header renumbers the @NNN symbols of every includer. */
static inline u16 OS_DisableIrq(void) {
    u16 prep = REG_IME;
    REG_IME = 0;
    return prep;
}

static inline u16 OS_RestoreIrq(u16 enable) {
    u16 prep = REG_IME;
    REG_IME = enable;
    return prep;
}

static inline void CP_SetDiv64_64(u64 numer, u64 denom) {
    REG_DIVCNT = DIV_CONTROL_64_64;
    REG_DIV_NUMER = numer;
    REG_DIV_DENOM = denom;
}

static inline void CP_SetDivImm64_64(u64 numer, u64 denom) {
    REG_DIV_NUMER = numer;
    REG_DIV_DENOM = denom;
}

static inline void CP_WaitDiv(void) {
    while (REG_DIVCNT & DIV_CONTROL_BUSY) {
    }
}

static inline s64 CP_GetDivResultImm64(void) {
    return REG_DIV_RESULT;
}

static inline void CP_SetSqrt64(u64 param) {
    REG_SQRTCNT = SQRT_CONTROL_64;
    REG_SQRT_PARAM = param;
}

static inline void CP_SetSqrtImm64(u64 param) {
    REG_SQRT_PARAM = param;
}

static inline void CP_WaitSqrt(void) {
    while (REG_SQRTCNT & SQRT_CONTROL_BUSY) {
    }
}

static inline u32 CP_GetSqrtResultImm32(void) {
    return REG_SQRT_RESULT;
}

extern "C" {
/* Normalises a run of Vector3_16s to unit length using the DS hardware divider
   and square-root units, pipelined: the length of vector n+1 is queued into the
   square-root unit while the reciprocal for vector n is still in flight, so the
   loop body always consumes the results started on the previous pass. That is
   why the first length is set up before the loop and the last one is consumed
   after it.

   Interrupts are masked around each square-root start because an interrupt
   handler that used the unit mid-write would corrupt the parameter. */

void func_ov007_020c368c(Vector3_16 *v, int count, int clamp) {
    int i;
    s64 len2;
    u16 ime_saved;
    Vector3_16 *next;
    s64 nextLen2;
    int root;
    s64 recip;
    s64 scale;

    len2 = v->x * v->x + v->y * v->y + v->z * v->z;
    CP_SetDiv64_64((u64)0x1000000 << 32, len2);
    ime_saved = OS_DisableIrq();
    CP_SetSqrt64((u64)len2 << 2);
    OS_RestoreIrq(ime_saved);

    if (count > 1) {
        i = 1;
        next = v + 1;
        do {
            nextLen2 = next->x * next->x + next->y * next->y + next->z * next->z;
            CP_WaitSqrt();
            root = CP_GetSqrtResultImm32();
            ime_saved = OS_DisableIrq();
            CP_SetSqrtImm64((u64)nextLen2 << 2);
            OS_RestoreIrq(ime_saved);
            CP_WaitDiv();
            recip = CP_GetDivResultImm64();
            CP_SetDivImm64_64((u64)0x1000000 << 32, nextLen2);
            scale = recip * root;
            v->x = (s16)((scale * v->x + (1LL << 44)) >> 45);
            v->y = (s16)((scale * v->y + (1LL << 44)) >> 45);
            v->z = (s16)((scale * v->z + (1LL << 44)) >> 45);
            if (clamp != 0) {
                if (v->x > 0xfff)
                    v->x = 0xfff;
                if (v->y > 0xfff)
                    v->y = 0xfff;
                if (v->z > 0xfff)
                    v->z = 0xfff;
            }
            v = next;
            next = next + 1;
            i++;
        } while (i < count);
    }

    CP_WaitSqrt();
    root = CP_GetSqrtResultImm32();
    CP_WaitDiv();
    recip = CP_GetDivResultImm64();
    scale = recip * root;
    v->x = (s16)((scale * v->x + (1LL << 44)) >> 45);
    v->y = (s16)((scale * v->y + (1LL << 44)) >> 45);
    v->z = (s16)((scale * v->z + (1LL << 44)) >> 45);
    if (clamp != 0) {
        if (v->x > 0xfff)
            v->x = 0xfff;
        if (v->y > 0xfff)
            v->y = 0xfff;
        if (v->z > 0xfff)
            v->z = 0xfff;
    }
}
}
