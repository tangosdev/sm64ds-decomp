#ifndef NITRO_CP_SQRT_H
#define NITRO_CP_SQRT_H

/* NitroSDK's hardware square root (nitro/cp/sqrt.h). CP_SetSqrt* selects the
 * mode and writes the parameter, which starts the root; CP_SetSqrtImm*
 * writes the parameter under the mode already set. CP_GetSqrtResult32 waits
 * on SQRTCNT's busy bit, CP_GetSqrtResultImm32 reads without waiting.
 *
 * Every parameter write in the ROM, mode store included, sits inside an IME
 * bracket: OS_DisableIrq before it, OS_RestoreIrq (a dummy IME read, then the
 * saved value) after it. Divider writes have no bracket. The bracket is
 * spelled on REG_IME here; nitro/os/interrupt.h keeps OS_DisableIrq and
 * OS_RestoreIrq out of its body list so its ~19 includers keep their @NNN
 * numbers.
 *
 * Inline bodies: an includer's compiler-numbered @NNN symbols move with
 * them, so only the files that drive the square-root unit include this. */

#include "types.h"
#include "nitro/hw/registers.h"

#define CP_SQRT_32BIT_MODE 0
#define CP_SQRT_64BIT_MODE 1

static inline void CP_SetSqrtImm64(u64 param) {
    u16 ime = REG_IME;
    REG_IME = 0;
    *(volatile u64 *)REG_SQRT_PARAM_ADDR = param;
    (void)REG_IME;
    REG_IME = ime;
}

static inline void CP_SetSqrtImm32(u32 param) {
    u16 ime = REG_IME;
    REG_IME = 0;
    *(volatile u32 *)REG_SQRT_PARAM_ADDR = param;
    (void)REG_IME;
    REG_IME = ime;
}

static inline void CP_SetSqrt64(u64 param) {
    u16 ime = REG_IME;
    REG_IME = 0;
    REG_SQRTCNT = CP_SQRT_64BIT_MODE;
    *(volatile u64 *)REG_SQRT_PARAM_ADDR = param;
    (void)REG_IME;
    REG_IME = ime;
}

static inline void CP_SetSqrt32(u32 param) {
    u16 ime = REG_IME;
    REG_IME = 0;
    REG_SQRTCNT = CP_SQRT_32BIT_MODE;
    *(volatile u32 *)REG_SQRT_PARAM_ADDR = param;
    (void)REG_IME;
    REG_IME = ime;
}

static inline s32 CP_IsSqrtBusy(void) {
    return REG_SQRTCNT & REG_CP_SQRTCNT_BUSY_MASK;
}

static inline void CP_WaitSqrt(void) {
    while (CP_IsSqrtBusy()) {
    }
}

static inline u32 CP_GetSqrtResultImm32(void) {
    return REG_SQRT_RESULT;
}

static inline u32 CP_GetSqrtResult32(void) {
    CP_WaitSqrt();
    return CP_GetSqrtResultImm32();
}

#endif /* NITRO_CP_SQRT_H */
