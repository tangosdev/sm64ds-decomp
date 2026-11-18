/* fBase vtable, display and IRQ state.
 * The _ZTV7fBase_c slot array, sIsDispOn, GXi_DmaId and OS_IRQTable.
 * Consumer: fBase_c, GX, OS interrupt dispatch.
 * ROM 0x02099fe4..0x0209a03c, 88 bytes, 1 configured symbols.
 */
#include "types.h"
#include "nitro/os/interrupt.h"

/* 0x02099fe4: 88 bytes. */
OSIrqFunction OS_IRQTable[22] = {
    (void (*)(void))&OS_IrqDummy,   (void (*)(void))&OS_IrqDummy,   (void (*)(void))&OS_IrqDummy,
    (void (*)(void))&OSi_IrqTimer0, (void (*)(void))&OSi_IrqTimer1, (void (*)(void))&OSi_IrqTimer2,
    (void (*)(void))&OSi_IrqTimer3, (void (*)(void))&OS_IrqDummy,   (void (*)(void))&OSi_IrqDma0,
    (void (*)(void))&OSi_IrqDma1,   (void (*)(void))&OSi_IrqDma2,   (void (*)(void))&OSi_IrqDma3,
    (void (*)(void))&OS_IrqDummy,   (void (*)(void))&OS_IrqDummy,   (void (*)(void))&OS_IrqDummy,
    (void (*)(void))&OS_IrqDummy,   (void (*)(void))&OS_IrqDummy,   (void (*)(void))&OS_IrqDummy,
    (void (*)(void))&OS_IrqDummy,   (void (*)(void))&OS_IrqDummy,   (void (*)(void))&OS_IrqDummy,
    (void (*)(void))&OS_IrqDummy,
};
