#ifndef NITRO_CP_DIVIDER_H
#define NITRO_CP_DIVIDER_H

/* NitroSDK's hardware divider (nitro/cp/divider.h). CP_SetDiv* selects the
 * mode and writes both operands, which starts the division; CP_SetDivImm*
 * writes the operands under the mode already set. CP_GetDiv* waits on
 * DIVCNT's busy bit, the *Imm variants read without waiting.
 *
 * Inline bodies: an includer's compiler-numbered @NNN symbols move with
 * them, so only the files that drive the divider include this. */

#include "types.h"
#include "nitro/hw/registers.h"

#define CP_DIV_32_32BIT_MODE 0
#define CP_DIV_64_32BIT_MODE 1
#define CP_DIV_64_64BIT_MODE 2

static inline void CP_SetDivImm32_32(u32 numer, u32 denom) {
    *(volatile u32 *)REG_DIV_NUMER_ADDR = numer;
    *(volatile u64 *)REG_DIV_DENOM_ADDR = denom;
}

static inline void CP_SetDiv32_32(u32 numer, u32 denom) {
    REG_DIVCNT = CP_DIV_32_32BIT_MODE;
    CP_SetDivImm32_32(numer, denom);
}

static inline void CP_SetDivImm64_32(u64 numer, u32 denom) {
    *(volatile u64 *)REG_DIV_NUMER_ADDR = numer;
    *(volatile u64 *)REG_DIV_DENOM_ADDR = denom;
}

static inline void CP_SetDiv64_32(u64 numer, u32 denom) {
    REG_DIVCNT = CP_DIV_64_32BIT_MODE;
    CP_SetDivImm64_32(numer, denom);
}

static inline void CP_SetDivImm64_64(u64 numer, u64 denom) {
    *(volatile u64 *)REG_DIV_NUMER_ADDR = numer;
    *(volatile u64 *)REG_DIV_DENOM_ADDR = denom;
}

static inline void CP_SetDiv64_64(u64 numer, u64 denom) {
    REG_DIVCNT = CP_DIV_64_64BIT_MODE;
    CP_SetDivImm64_64(numer, denom);
}

static inline s32 CP_IsDivBusy(void) {
    return REG_DIVCNT & REG_CP_DIVCNT_BUSY_MASK;
}

static inline void CP_WaitDiv(void) {
    while (CP_IsDivBusy()) {
    }
}

static inline s64 CP_GetDivResultImm64(void) {
    return *(volatile s64 *)REG_DIV_RESULT_ADDR;
}

static inline s32 CP_GetDivResultImm32(void) {
    return *(volatile s32 *)REG_DIV_RESULT_ADDR;
}

static inline s64 CP_GetDivResult64(void) {
    CP_WaitDiv();
    return CP_GetDivResultImm64();
}

static inline s32 CP_GetDivResult32(void) {
    CP_WaitDiv();
    return CP_GetDivResultImm32();
}

static inline s32 CP_GetDivRemainderImm32(void) {
    return *(volatile s32 *)REG_DIVREM_RESULT_ADDR;
}

static inline s32 CP_GetDivRemainder32(void) {
    CP_WaitDiv();
    return CP_GetDivRemainderImm32();
}

#endif /* NITRO_CP_DIVIDER_H */
