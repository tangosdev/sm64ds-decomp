// HAND-ASM PRIMITIVE: byte-faithful asm-block match (assembly-only primitive). Per asm policy.
// NitroSDK's OS_InitContext, which the SDK's own os_context.c writes as assembly with this
// instruction sequence. A later SDK revision adds a `tst newsp, #4 / subne newsp, newsp, #4`
// 8-byte-alignment fixup that this build predates. The two functions that follow it here,
// OS_SaveContext (0x020585cc) and OS_LoadContext (0x02058618), come out of the same file and
// are `asm` there too. That is direct evidence for the first row of notes/asm-policy.md's table
// (the original was hand-written asm), not an inference from the instruction set.
//
// What it does: stores the entry pc+4 and the two stack pointers (the SVC stack at the top of
// the thread's stack, the thread's own sp HW_SVC_STACK_SIZE below it), derives the initial
// CPSR from the Thumb bit of the entry address, and clears r0-r12 and lr.
#include "nitro/os/context.h"
#include "nitro/hw/armArch.h"
#include "nitro/hw/mmap_tcm.h"

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
