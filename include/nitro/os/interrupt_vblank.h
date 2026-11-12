#ifndef NITRO_OS_INTERRUPT_VBLANK_H
#define NITRO_OS_INTERRUPT_VBLANK_H

/* NitroSDK's VBlank counter read (nitro/os/common/interrupt.h): the ARM7
 * bumps the word at HW_VBLANK_COUNT_BUF in the system work area once a
 * frame.
 *
 * Split out of nitro/os/interrupt.h for the reason interrupt_check.h is: an
 * inline body in a shared header renumbers the @NNN symbols of every file
 * that includes it, so only the readers include this. */

#include "types.h"
#include "nitro/hw/mmap_shared.h"

static inline vu32 OS_GetVBlankCount(void) {
    return *(vu32 *)HW_VBLANK_COUNT_BUF;
}

#endif /* NITRO_OS_INTERRUPT_VBLANK_H */
