//cpp
/* CPU-state primitives (SDK os_system.c), .text 0x02059d08..0x02059d8c.
 * HAND-ASM PRIMITIVE: mrs/msr read and write CPSR state, and mcr p15 drives
 * the coprocessor. No C construct compiles to either, so the asm blocks are
 * the faithful source.
 *
 * The IRQ operations set or clear CPSR bit 0x80 (IRQ) or 0xc0 (IRQ and FIQ)
 * and return the previous state for restoration. ARMProcessorMode returns
 * the CPSR mode field. CP15::WaitForInterrupt halts the core until an
 * interrupt arrives (c7,c0,4); the value written is ignored by the hardware.
 * The preceding IRQ-state initializer has its own compiler input.
 */

namespace IRQ {

// @symbol _ZN3IRQ6EnableEv
asm unsigned int Enable(void)
{
    mrs r0, cpsr
    bic r1, r0, #0x80
    msr cpsr_c, r1
    and r0, r0, #0x80
    bx lr
}

// @symbol _ZN3IRQ7DisableEv
asm unsigned int Disable(void)
{
    mrs r0, cpsr
    orr r1, r0, #0x80
    msr cpsr_c, r1
    and r0, r0, #0x80
    bx lr
}

// @symbol _ZN3IRQ7RestoreEj
asm unsigned int Restore(unsigned int state)
{
    mrs r1, cpsr
    bic r2, r1, #0x80
    orr r2, r2, r0
    msr cpsr_c, r2
    and r0, r1, #0x80
    bx lr
}

// @symbol _ZN3IRQ10DisableAllEv
asm unsigned int DisableAll(void)
{
    mrs r0, cpsr
    orr r1, r0, #0xc0
    msr cpsr_c, r1
    and r0, r0, #0xc0
    bx lr
}

// @symbol _ZN3IRQ10RestoreAllEj
asm unsigned int RestoreAll(unsigned int state)
{
    mrs r1, cpsr
    bic r2, r1, #0xc0
    orr r2, r2, r0
    msr cpsr_c, r2
    and r0, r1, #0xc0
    bx lr
}

}

// @symbol ARMProcessorMode
extern "C" asm int ARMProcessorMode(void)
{
    mrs r0, cpsr
    and r0, r0, #0x1f
    bx lr
}

namespace CP15 {

// @symbol _ZN4CP1516WaitForInterruptEv
asm void WaitForInterrupt(void)
{
    mov r0, #0
    mcr p15, 0, r0, c7, c0, 4
    bx lr
}

}
