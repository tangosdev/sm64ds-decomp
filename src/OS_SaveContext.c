#include "nitro/os/context.h"
#include "nitro/hw/armArch.h"

// HAND-ASM PRIMITIVE: byte-faithful asm-block match (assembly-only primitive). Per asm policy.
// NitroSDK's OS_SaveContext (os_context.c, asm there too): saves the divider and square-root
// context through CP_SaveContext, then the CPSR, the SVC stack pointer and r0-r14, with the
// return address set past the `mov r0, #0` so a later OS_LoadContext resumes here returning 1.
asm int OS_SaveContext(register OSContext *context)
{
    stmdb sp!, {r0, lr}
    add r0, r0, #OS_CONTEXT_CP_CONTEXT
    ldr r1, =CP_SaveContext
    blx r1
    ldmia sp!, {r0, lr}
    add r1, r0, #OS_CONTEXT_CPSR
    mrs r2, cpsr
    str r2, [r1], #OS_CONTEXT_R0 - OS_CONTEXT_CPSR
    mov r0, #HW_PSR_SVC_MODE | HW_PSR_IRQ_DISABLE | HW_PSR_FIQ_DISABLE | HW_PSR_ARM_STATE
    msr cpsr_c, r0
    str sp, [r1, #OS_CONTEXT_SP_SVC - OS_CONTEXT_R0]
    msr cpsr_c, r2
    mov r0, #1
    stmia r1, {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, sp, lr}
    add r0, pc, #8
    str r0, [r1, #OS_CONTEXT_PC_PLUS4 - OS_CONTEXT_R0]
    mov r0, #0
    bx lr
}
