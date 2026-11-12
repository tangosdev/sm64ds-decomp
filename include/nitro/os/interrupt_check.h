#ifndef NITRO_OS_INTERRUPT_CHECK_H
#define NITRO_OS_INTERRUPT_CHECK_H

/* NitroSDK's IRQ check word (nitro/os/common/interrupt.h): a handler sets
 * its interrupt's bit in HW_INTR_CHECK_BUF, and OS_WaitIrq sleeps on
 * OSi_IrqThreadQueue until the bit it waits for is up.
 *
 * In the SDK this sits in interrupt.h next to OS_ClearIrqCheckFlag and
 * OS_GetIrqCheckFlag. It is split out here because an inline body in a
 * shared header renumbers the @NNN symbols of every file that includes it,
 * and nitro/os/interrupt.h has ~19 includers; only the handlers that set the
 * word include this. The other two are left out: nothing in this ROM clears
 * or reads the word in C, and each unused inline still costs its includers
 * @NNN numbers. */

#include "types.h"
#include "nitro/hw/mmap_tcm.h"
#include "nitro/os/interrupt.h"

static inline void OS_SetIrqCheckFlag(OSIrqMask intr) {
    *(vu32 *)HW_INTR_CHECK_BUF |= (u32)intr;
}

#endif /* NITRO_OS_INTERRUPT_CHECK_H */
