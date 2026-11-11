#ifndef NITRO_HW_MMAP_GLOBAL_H
#define NITRO_HW_MMAP_GLOBAL_H

/* The ARM9 global memory map, under NitroSDK's names
 * (nitro/hw/ARM9/mmap_global.h). DTCM sits wherever the linker put the DTCM
 * autoload: HW_DTCM is the linker's SDK_AUTOLOAD_DTCM_START, 0x023c0000 in
 * this ROM. The first object in that autoload is os_irqHandler.c's
 * OSi_IrqThreadQueue, so the two names share an address. */

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

extern u32 SDK_AUTOLOAD_DTCM_START[];

#ifdef __cplusplus
}
#endif

#define HW_DTCM      ((u32)SDK_AUTOLOAD_DTCM_START)
#define HW_DTCM_SIZE 0x4000
#define HW_DTCM_END  (HW_DTCM + HW_DTCM_SIZE)

#endif /* NITRO_HW_MMAP_GLOBAL_H */
