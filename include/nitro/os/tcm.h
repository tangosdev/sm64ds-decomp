#ifndef NITRO_OS_TCM_H
#define NITRO_OS_TCM_H

/*
 * The DTCM enable and base query of the Nitro SDK's os_tcm.c, under the SDK's
 * own names (pret/pokediamond arm9/lib/NitroSDK/include/OS_tcm.h;
 * ntrtwl/NitroSDK libraries/os/src/os_tcm.c for OS_EnableDTCM). The bodies
 * are hand-written asm, in src/engine/cp15/CP15_TCM.cpp: OS_EnableDTCM sets
 * the DTCM enable bit in c1, OS_GetDTCMAddress reads the DTCM region register
 * (c9,c1,0) and masks off the size field, leaving the 4 KiB-aligned base.
 */

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

void OS_EnableDTCM(void);
u32 OS_GetDTCMAddress(void);

#ifdef __cplusplus
}
#endif

#endif /* NITRO_OS_TCM_H */
