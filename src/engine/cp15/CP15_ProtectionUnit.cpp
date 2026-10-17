//cpp
/* Protection unit on/off (SDK os_protectionUnit.c), .text 0x020593cc..0x020593ec.
 * HAND-ASM PRIMITIVE: mrc/mcr p15 read-modify-write the CP15 control register
 * c1, which no C construct compiles to, so the asm blocks are the faithful
 * source. Bit 0 enables the MPU.
 */

namespace CP15 {

// @symbol _ZN4CP159EnableMPUEv
asm void EnableMPU(void)
{
    mrc p15, 0, r0, c1, c0, 0
    orr r0, r0, #1
    mcr p15, 0, r0, c1, c0, 0
    bx lr
}

// @symbol _ZN4CP1510DisableMPUEv
asm void DisableMPU(void)
{
    mrc p15, 0, r0, c1, c0, 0
    bic r0, r0, #1
    mcr p15, 0, r0, c1, c0, 0
    bx lr
}

}
