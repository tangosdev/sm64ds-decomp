//cpp
/* CPU-state primitives (SDK os_system.c), .text 0x02059d08..0x02059d8c.
 * HAND-ASM PRIMITIVE: mrs/msr read and write CPSR state, and mcr p15 drives
 * the coprocessor. No C construct compiles to either, so the asm blocks are
 * the faithful source.
 *
 * The interrupt calls take pret/pokediamond's OS_system.c names
 * (OS_EnableInterrupts .. OS_RestoreInterrupts_IrqAndFiq, declared in
 * nitro/os/system.h): each sets or clears CPSR bit 0x80 (IRQ) or 0xc0 (IRQ
 * and FIQ) and returns the previous state for restoration.
 * OS_GetProcMode (pret OS_system.c) returns the CPSR mode field. OS_Halt
 * (pret OS_terminate_proc.c, which a later SDK split out of os_system.c)
 * halts the core until an interrupt arrives (c7,c0,4); the value written is
 * ignored by the hardware.
 * The preceding IRQ-state initializer has its own compiler input.
 */

#include "nitro/os/system.h"

#ifndef SM64DS_PLATFORM_PC
asm OSIntrMode OS_EnableInterrupts(void)
{
    mrs r0, cpsr
    bic r1, r0, #0x80
    msr cpsr_c, r1
    and r0, r0, #0x80
    bx lr
}
#endif

#ifndef SM64DS_PLATFORM_PC
asm OSIntrMode OS_DisableInterrupts(void)
{
    mrs r0, cpsr
    orr r1, r0, #0x80
    msr cpsr_c, r1
    and r0, r0, #0x80
    bx lr
}
#endif

#ifndef SM64DS_PLATFORM_PC
asm OSIntrMode OS_RestoreInterrupts(OSIntrMode state)
{
    mrs r1, cpsr
    bic r2, r1, #0x80
    orr r2, r2, r0
    msr cpsr_c, r2
    and r0, r1, #0x80
    bx lr
}
#endif

#ifndef SM64DS_PLATFORM_PC
asm OSIntrMode OS_DisableInterrupts_IrqAndFiq(void)
{
    mrs r0, cpsr
    orr r1, r0, #0xc0
    msr cpsr_c, r1
    and r0, r0, #0xc0
    bx lr
}
#endif

#ifndef SM64DS_PLATFORM_PC
asm OSIntrMode OS_RestoreInterrupts_IrqAndFiq(OSIntrMode state)
{
    mrs r1, cpsr
    bic r2, r1, #0xc0
    orr r2, r2, r0
    msr cpsr_c, r2
    and r0, r1, #0xc0
    bx lr
}
#endif

#ifndef SM64DS_PLATFORM_PC
asm OSProcMode OS_GetProcMode(void)
{
    mrs r0, cpsr
    and r0, r0, #0x1f
    bx lr
}
#endif

#ifndef SM64DS_PLATFORM_PC
asm void OS_Halt(void)
{
    mov r0, #0
    mcr p15, 0, r0, c7, c0, 4
    bx lr
}
#endif
