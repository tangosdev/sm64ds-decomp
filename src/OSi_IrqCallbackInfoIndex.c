/* os_irqTable.c's OSi_IrqCallbackInfoIndex: the IE bit of each one-shot
   callback slot in OSi_IrqCallbackInfo, DMA 0-3 then timers 0-3. OSi_IrqCallback
   reads it to set the bit in the IRQ check word and to mask the interrupt
   again. */
#include "types.h"
#include "nitro/hw/registers.h"

u16 OSi_IrqCallbackInfoIndex[8] = {
    REG_OS_IE_D0_SHIFT, REG_OS_IE_D1_SHIFT, REG_OS_IE_D2_SHIFT, REG_OS_IE_D3_SHIFT,
    REG_OS_IE_T0_SHIFT, REG_OS_IE_T1_SHIFT, REG_OS_IE_T2_SHIFT, REG_OS_IE_T3_SHIFT,
};
