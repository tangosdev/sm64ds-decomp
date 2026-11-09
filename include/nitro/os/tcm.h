#ifndef NITRO_OS_TCM_H
#define NITRO_OS_TCM_H

/*
 * The DTCM base query of the Nitro SDK's os_tcm.c, under the SDK's own name
 * (pret/pokediamond arm9/lib/NitroSDK/include/OS_tcm.h). The body is
 * hand-written asm, in src/engine/cp15/CP15_TCM.cpp: it reads the DTCM region
 * register (c9,c1,0) and masks off the size field, leaving the 4 KiB-aligned
 * base.
 */

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

u32 OS_GetDTCMAddress(void);

#ifdef __cplusplus
}
#endif

#endif /* NITRO_OS_TCM_H */
