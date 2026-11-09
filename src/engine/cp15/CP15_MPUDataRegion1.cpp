//cpp
/* OS_SetProtectionRegion1 at 0x020593ec (SDK os_protectionRegion.c; the name
 * is pret/pokediamond's OS_protectionRegion.c one, declared in
 * nitro/os/protectionRegion.h) -- write MPU data region 1 (c6,c1). The value
 * packs base address, size and enable, as init_cp15 (src/init_cp15.c) shows
 * when it programs all eight regions.
 *
 * HAND-ASM PRIMITIVE: `mrc`/`mcr p15` are coprocessor accesses and no C
 * construct compiles to them, so the asm block is the faithful source rather
 * than a transcription of something lost. Layout-free: no `this`, no vtable,
 * no struct. */
#include "nitro/os/protectionRegion.h"

void OS_SetProtectionRegion1(u32 x)
{
    asm { mcr p15,0,x,c6,c1,0 }
}
