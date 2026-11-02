#ifndef PRIVATE_ARM_MATH_STATE_H
#define PRIVATE_ARM_MATH_STATE_H

#include "types.h"

/* Saved state of the hardware divider and square-root unit, as
 * ARMMathSaveState captures it from REG_DIVCNT .. REG_SQRT_PARAM
 * (0x04000280..0x040002b8) and ARMMathLoadState puts it back. */
typedef struct ARMMathState {
    u64 div_numer; /* 0x00 -- 0x04000290 */
    u64 div_denom; /* 0x08 -- 0x04000298 */
    u64 sqrt;      /* 0x10 -- 0x040002b8 */
    u16 div_mode;  /* 0x18 -- 0x04000280 (low 2 bits) */
    u16 sqrt_mode; /* 0x1a -- 0x040002b0 (low bit) */
} ARMMathState;

#ifndef SM64DS_PLATFORM_PC
typedef char ARMMathState_size_must_be_0x1c[sizeof(ARMMathState) == 0x1c ? 1 : -1];
#endif

#endif
