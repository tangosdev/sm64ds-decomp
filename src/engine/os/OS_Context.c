/* The thread execution context (the Nitro SDK's os_context.c), arm9 .text
 * 0x02058568..0x0205865c. All three routines are hand-written assembly in
 * the SDK itself; mwccarm emits asm bodies in source order, the order pret's
 * pokediamond os_context.c writes them in. A later SDK revision adds a
 * `tst newsp, #4 / subne newsp, newsp, #4` 8-byte-alignment fixup to
 * OS_InitContext that this build predates.
 */
#include "nitro/os/context.h"
#include "nitro/hw/armArch.h"
#include "nitro/hw/mmap_tcm.h"

/* Stores the entry pc+4 and the two stack pointers (the SVC stack at the top
   of the thread's stack, the thread's own sp HW_SVC_STACK_SIZE below it),
   derives the initial CPSR from the Thumb bit of the entry address, and
   clears r0-r12 and lr. */
asm void OS_InitContext(register OSContext *context, register u32 newpc, register u32 newsp)
{
    add newpc, newpc, #4
    str newpc, [context, #OS_CONTEXT_PC_PLUS4]
    str newsp, [context, #OS_CONTEXT_SP_SVC]
    sub newsp, newsp, #HW_SVC_STACK_SIZE
    str newsp, [context, #OS_CONTEXT_SP]
    ands r1, newpc, #1
    movne r1, #HW_PSR_SYS_MODE | HW_PSR_THUMB_STATE
    moveq r1, #HW_PSR_SYS_MODE | HW_PSR_ARM_STATE
    str r1, [context, #OS_CONTEXT_CPSR]
    mov r1, #0
    str r1, [context, #OS_CONTEXT_R0]
    str r1, [context, #OS_CONTEXT_R1]
    str r1, [context, #OS_CONTEXT_R2]
    str r1, [context, #OS_CONTEXT_R3]
    str r1, [context, #OS_CONTEXT_R4]
    str r1, [context, #OS_CONTEXT_R5]
    str r1, [context, #OS_CONTEXT_R6]
    str r1, [context, #OS_CONTEXT_R7]
    str r1, [context, #OS_CONTEXT_R8]
    str r1, [context, #OS_CONTEXT_R9]
    str r1, [context, #OS_CONTEXT_R10]
    str r1, [context, #OS_CONTEXT_R11]
    str r1, [context, #OS_CONTEXT_R12]
    str r1, [context, #OS_CONTEXT_LR]
    bx lr
}

/* Saves the divider and square-root context through CP_SaveContext, then the
   CPSR, the SVC stack pointer and r0-r14, with the return address set past
   the `mov r0, #0` so a later OS_LoadContext resumes here returning 1. */
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

/* Puts the divider and square-root context back through CPi_RestoreContext,
   then switches to SVC mode with interrupts off, loads the saved CPSR into
   the SPSR, the SVC stack pointer and the user registers, and returns into
   the context with the exception return. */
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
