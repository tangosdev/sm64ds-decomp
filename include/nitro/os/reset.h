#ifndef NITRO_OS_RESET_H
#define NITRO_OS_RESET_H

/*
 * The ARM9 soft reset, under the NitroSDK's own names (nitro/os/common/reset.h).
 * OS_InitReset is in os_reset.c (src/engine/os/OS_Reset.c); OS_ResetSystem
 * runs from ITCM (src/OSi_ReloadRomData.c). The parameter of OS_ResetSystem
 * goes to HW_RESET_PARAMETER_BUF for the rebooted image.
 *
 * The parameters are unnamed on purpose: a named parameter in a shared
 * declaration costs every including C++ file an @NNN number.
 */

#include "types.h"


extern "C" void OS_InitReset(void);
extern "C" void OS_ResetSystem(u32);


#endif
