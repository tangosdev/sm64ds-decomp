//cpp
/* DTCM control (SDK os_tcm.c), .text 0x020593a8..0x020593cc.
 * HAND-ASM PRIMITIVE: mrc/mcr p15 read and write the CP15 control and TCM
 * region registers, which no C construct compiles to, so the asm blocks are
 * the faithful source.
 *
 * OS_EnableDTCM (ntrtwl/NitroSDK libraries/os/src/os_tcm.c, the same three
 * instructions) sets the DTCM enable bit (0x10000) in the control register
 * c1. OS_GetDTCMAddress (pret/pokediamond OS_tcm.c, declared in
 * nitro/os/tcm.h) reads the DTCM region register (c9,c1,0) and masks off the
 * size field, leaving the 4 KiB-aligned base.
 */

#include "nitro/os/tcm.h"

asm void OS_EnableDTCM(void)
{
    mrc p15, 0, r0, c1, c0, 0
    orr r0, r0, #0x10000
    mcr p15, 0, r0, c1, c0, 0
    bx lr
}

asm u32 OS_GetDTCMAddress(void)
{
    mrc p15, 0, r0, c9, c1, 0
    ldr r1, =0xfffff000
    and r0, r0, r1
    bx lr
}
