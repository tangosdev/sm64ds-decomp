#include "nitro/os/context.h"
#include "nitro/hw/armArch.h"

// HAND-ASM PRIMITIVE: byte-faithful asm-block match (assembly-only primitive). Per asm policy.
// NitroSDK's OS_LoadContext (os_context.c, asm there too): puts the divider and square-root
// context back through CPi_RestoreContext, then switches to SVC mode with interrupts off,
// loads the saved CPSR into the SPSR, the SVC stack pointer and the user registers, and
// returns into the context with the exception return.
asm void OS_LoadContext(register OSContext *context)
{
    stmdb sp!, {r0, lr}
    add r0, r0, #OS_CONTEXT_CP_CONTEXT
    ldr r1, =CPi_RestoreContext
    blx r1
    ldmia sp!, {r0, lr}
    mrs r1, cpsr
    bic r1, r1, #HW_PSR_CPU_MODE_MASK
    orr r1, r1, #HW_PSR_SVC_MODE | HW_PSR_IRQ_DISABLE | HW_PSR_FIQ_DISABLE
    msr cpsr_c, r1
    ldr r1, [r0], #OS_CONTEXT_R0 - OS_CONTEXT_CPSR
    msr spsr_fsxc, r1
    ldr sp, [r0, #OS_CONTEXT_SP_SVC - OS_CONTEXT_R0]
    ldr lr, [r0, #OS_CONTEXT_PC_PLUS4 - OS_CONTEXT_R0]
    ldmia r0, {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, sp, lr} ^
    nop
    subs pc, lr, #4
}
