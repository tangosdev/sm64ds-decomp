/* claimed-storage */
/* arm9 .data 0x02099fe4..0x0209a064. 8 storage addresses, one object. */
#include "types.h"

extern void OS_IrqDummy(void);
extern void OSi_IrqTimer0(void);
extern void OSi_IrqTimer1(void);
extern void OSi_IrqTimer2(void);
extern void OSi_IrqTimer3(void);
extern void OSi_IrqDma0(void);
extern void OSi_IrqDma1(void);
extern void OSi_IrqDma2(void);
extern void OSi_IrqDma3(void);
extern void func_02057e30(void);
extern u32 fsi_def_digest_key[];

#pragma explicit_zero_data on
u32 OS_IRQTable[] = {
    (u32)&OS_IrqDummy,
    (u32)&OS_IrqDummy,
    (u32)&OS_IrqDummy,
    (u32)&OSi_IrqTimer0,
    (u32)&OSi_IrqTimer1,
    (u32)&OSi_IrqTimer2,
    (u32)&OSi_IrqTimer3,
    (u32)&OS_IrqDummy,
    (u32)&OSi_IrqDma0,
    (u32)&OSi_IrqDma1,
    (u32)&OSi_IrqDma2,
    (u32)&OSi_IrqDma3,
    (u32)&OS_IrqDummy,
    (u32)&OS_IrqDummy,
    (u32)&OS_IrqDummy,
    (u32)&OS_IrqDummy,
    (u32)&OS_IrqDummy,
    (u32)&OS_IrqDummy,
    (u32)&OS_IrqDummy,
    (u32)&OS_IrqDummy,
    (u32)&OS_IrqDummy,
    (u32)&OS_IrqDummy,
    (u32)&func_02057e30,
    0xffffffff, 0x00002f3a, 0x006d6f72,
    (u32)&fsi_def_digest_key,
    0x00000040, 0x5a827999, 0x6ed9eba1, 0x8f1bbcdc, 0xca62c1d6,
};
#pragma explicit_zero_data off
