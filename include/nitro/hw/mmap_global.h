#ifndef NITRO_HW_MMAP_GLOBAL_H
#define NITRO_HW_MMAP_GLOBAL_H

/* NitroSDK's ARM9 memory map (nitro/hw/ARM9/mmap_global.h).
 *
 * The palette and OAM blocks of both display engines; the DB_ names are the
 * sub engine's.
 *
 * DTCM sits wherever the linker put the DTCM autoload: HW_DTCM is the
 * linker's SDK_AUTOLOAD_DTCM_START, 0x023c0000 in this ROM. The first object
 * in that autoload is os_irqHandler.c's OSi_IrqThreadQueue, so the two names
 * share an address. */

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

extern u32 SDK_AUTOLOAD_DTCM_START[];

#ifdef __cplusplus
}
#endif

#define HW_PLTT        0x05000000
#define HW_BG_PLTT     0x05000000
#define HW_OBJ_PLTT    0x05000200
#define HW_DB_BG_PLTT  0x05000400
#define HW_DB_OBJ_PLTT 0x05000600

#define HW_OAM    0x07000000
#define HW_DB_OAM 0x07000400

#define HW_MAIN_MEM  0x02000000
#define HW_CTRDG_ROM 0x08000000

#define HW_DTCM      ((u32)SDK_AUTOLOAD_DTCM_START)
#define HW_DTCM_SIZE 0x4000
#define HW_DTCM_END  (HW_DTCM + HW_DTCM_SIZE)

#endif /* NITRO_HW_MMAP_GLOBAL_H */
