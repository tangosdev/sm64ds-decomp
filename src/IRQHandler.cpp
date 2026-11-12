//cpp
/* IRQ entry and thread switch (SDK os_irqHandler.c, the ITCM part),
 * itcm .text 0x01ffd97c..0x01ffdb28.
 *
 * OS_IrqHandler (pret/pokediamond OS_irqHandler.c): read IE & IF,
 * acknowledge the highest-priority (lowest) set bit and jump to its handler
 * in OS_IRQTable (the table OS_SetIrqFunction fills), with lr preloaded so
 * the handler returns into OS_IrqHandler_ThreadSwitch: wake the threads
 * waiting on the DTCM interrupt mask, then, when a reschedule is pending, save the interrupted thread's
 * user-mode registers and switch to the highest-priority ready one.
 *
 * HAND-ASM PRIMITIVE: both are whole-function asm, as in the SDK: `clz`, the
 * banked-register transfers (`stmib ...^`, `ldmib ...^`) and the CPSR/SPSR
 * mode switches have no C expression, and each routine exits through the
 * other's stack frame.
 * Asm-only functions are emitted in source order.
 *
 * The switch addresses the thread, its context and the scheduler head
 * (OSi_ThreadInfo) through the SDK's OS_THREAD_OFFSET_*,
 * OS_CONTEXT_* and OS_THREADINFO_OFFSET_* (nitro/os/thread.h,
 * nitro/os/context.h).
 *
 * leftovers: OS_IRQTable and OSi_ThreadInfo belong to other
 * files and are not claimed here.
 */
#include "nitro/os/interrupt.h"
#include "nitro/os/thread.h" /* OSi_ThreadInfo, the thread scheduler head */
#include "nitro/hw/armArch.h"

extern "C" {
extern OSThreadQueue OSi_IrqThreadQueue; /* in DTCM: threads waiting on an interrupt */
void OS_IrqHandler_ThreadSwitch(void);
}

// Dispatch the lowest pending interrupt.
#ifndef SM64DS_PLATFORM_PC
asm void OS_IrqHandler(void)
{
    stmdb sp!, {lr}
    mov ip, #0x04000000
    add ip, ip, #0x210
    ldr r1, [ip, #-8]
    cmp r1, #0
    ldmeqia sp!, {pc}
    ldmia ip, {r1, r2}
    ands r1, r1, r2
    ldmeqia sp!, {pc}
    mov r3, #0x80000000
_loop:
    clz r0, r1
    bics r1, r1, r3, lsr r0
    bne _loop
    mov r1, r3, lsr r0
    str r1, [ip, #0x4]
    rsbs r0, r0, #0x1f
    ldr r1, =OS_IRQTable
    ldr r0, [r1, r0, lsl #2]
    ldr lr, =OS_IrqHandler_ThreadSwitch
    bx r0
}
#endif

extern "C" {

// OS_IrqHandler_ThreadSwitch: wake waiters, then switch threads if asked.
#ifndef SM64DS_PLATFORM_PC
asm void OS_IrqHandler_ThreadSwitch(void)
{
    ldr ip, =OSi_IrqThreadQueue
    ldrh r3, [ip]
    mov r0, #0
    cmp r3, #0
    beq _resched
    strh r0, [ip]
    ldr ip, =OSi_ThreadInfo
    mov r1, #1
    strh r1, [ip, #OS_THREADINFO_OFFSET_ISNEEDRESCHEDULING]
    ldr ip, [ip, #OS_THREADINFO_OFFSET_LIST]
    mov r2, #OS_THREAD_STATE_READY
_wake:
    cmp ip, #0
    beq _resched
    ldr r0, [ip, #OS_THREAD_OFFSET_ID]
    tst r3, r1, lsl r0
    strne r2, [ip, #OS_THREAD_OFFSET_STATE]
    ldr ip, [ip, #OS_THREAD_OFFSET_NEXT]
    b _wake

_resched:
    ldr ip, =OSi_ThreadInfo
    ldrh r1, [ip, #OS_THREADINFO_OFFSET_ISNEEDRESCHEDULING]
    cmp r1, #0
    ldreq pc, [sp], #4
    mov r1, #0
    strh r1, [ip, #OS_THREADINFO_OFFSET_ISNEEDRESCHEDULING]
    mov r3, #HW_PSR_IRQ_MODE | HW_PSR_FIQ_DISABLE | HW_PSR_IRQ_DISABLE | HW_PSR_ARM_STATE
    msr cpsr_c, r3
    add r2, ip, #OS_THREADINFO_OFFSET_LIST
    ldr r1, [r2]
_pick:
    cmp r1, #0
    ldrneh r0, [r1, #OS_THREAD_OFFSET_STATE]
    cmpne r0, #OS_THREAD_STATE_READY
    ldrne r1, [r1, #OS_THREAD_OFFSET_NEXT]
    bne _pick
    cmp r1, #0
    bne _have
_leave:
    mov r3, #HW_PSR_IRQ_MODE | HW_PSR_IRQ_DISABLE | HW_PSR_ARM_STATE
    msr cpsr_c, r3
    ldr pc, [sp], #4

_have:
    ldr r0, [ip, #OS_THREADINFO_OFFSET_CURRENT]
    cmp r1, r0
    beq _leave
    ldr r3, [ip, #OS_THREADINFO_OFFSET_SWITCHCALLBACK]
    cmp r3, #0
    beq _switch
    stmdb sp!, {r0, r1, ip}
    mov lr, pc
    bx r3
    ldmia sp!, {r0, r1, ip}

_switch:
    str r1, [ip, #OS_THREADINFO_OFFSET_CURRENT]
    mrs r2, spsr
    str r2, [r0, #OS_THREAD_OFFSET_CONTEXT]!
    stmdb sp!, {r0, r1}
    add r0, r0, #OS_THREAD_OFFSET_CONTEXT
    add r0, r0, #OS_CONTEXT_CP_CONTEXT
    ldr r1, =CP_SaveContext
    blx r1
    ldmia sp!, {r0, r1}
    ldmib sp!, {r2, r3}
    stmib r0!, {r2, r3}
    ldmib sp!, {r2, r3, ip, lr}
    stmib r0!, {r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, sp, lr}^
    stmib r0!, {lr}
    mov r3, #HW_PSR_SVC_MODE | HW_PSR_FIQ_DISABLE | HW_PSR_IRQ_DISABLE | HW_PSR_ARM_STATE
    msr cpsr_c, r3
    stmib r0!, {sp}
    stmdb sp!, {r1}
    add r0, r1, #OS_THREAD_OFFSET_CONTEXT
    add r0, r0, #OS_CONTEXT_CP_CONTEXT
    ldr r1, =CPi_RestoreContext
    blx r1
    ldmia sp!, {r1}
    ldr sp, [r1, #OS_THREAD_OFFSET_CONTEXT + OS_CONTEXT_SP_SVC]
    mov r3, #HW_PSR_IRQ_MODE | HW_PSR_FIQ_DISABLE | HW_PSR_IRQ_DISABLE | HW_PSR_ARM_STATE
    msr cpsr_c, r3
    ldr r2, [r1, #OS_THREAD_OFFSET_CONTEXT]!
    msr spsr_fc, r2
    ldr lr, [r1, #OS_CONTEXT_PC_PLUS4 - OS_CONTEXT_CPSR]
    ldmib r1!, {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, sp, lr}^
    nop
    stmda sp!, {r0, r1, r2, r3, r12, lr}
    ldmia sp!, {pc}
}
#else
void OS_IrqHandler_ThreadSwitch(void);
#endif

}
