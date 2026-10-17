//cpp
/* IRQ entry and thread switch (SDK os_irqHandler.c, the ITCM part),
 * itcm .text 0x01ffd97c..0x01ffdb28.
 *
 * IRQ::UserInterruptHandler is OS_IrqHandler: read IE & IF, acknowledge the
 * highest-priority (lowest) set bit and jump to its handler in data_02099fe4
 * (the table IRQ::SetIRQHandler fills), with lr preloaded so the handler
 * returns into func_01ffd9d4. That is OS_IrqHandler_ThreadSwitch: wake the
 * threads waiting on the DTCM interrupt mask, then, when a reschedule is
 * pending, save the interrupted thread's user-mode registers and switch to
 * the highest-priority ready one.
 *
 * HAND-ASM PRIMITIVE: both are whole-function asm, as in the SDK: `clz`, the
 * banked-register transfers (`stmib ...^`, `ldmib ...^`) and the CPSR/SPSR
 * mode switches have no C expression, and each routine exits through the
 * other's stack frame.
 * Asm-only functions are emitted in source order.
 *
 * Context block (per thread): +0x00 spsr, +0x04.. the banked user registers,
 * +0x40 lr, +0x44 sp, +0x48 the math state ARMMath{Save,Load}State own.
 * Scheduler head data_020a6134: +0x00 reschedule-pending halfword, +0x08 the
 * current thread, +0x0c the thread list, +0x10 an optional pre-switch hook.
 * Per thread: +0x64 state, +0x68 next, +0x6c the wait-mask bit.
 *
 * leftovers: data_02099fe4, data_023c0000 and data_020a6134 belong to other
 * files and are not claimed here.
 */
#include "IRQ.h"

extern "C" {
extern IRQ::Handler data_02099fe4[]; /* one handler per IE bit */
extern short data_023c0000[];        /* DTCM: interrupts threads wait on */
extern int data_020a6134[];          /* the thread scheduler head */
struct ARMMathState;                 /* divider/sqrt unit state */
void ARMMathSaveState(ARMMathState *state);
void ARMMathLoadState(const ARMMathState *state);
void func_01ffd9d4(void);
}

namespace IRQ {

// @symbol _ZN3IRQ20UserInterruptHandlerEv
// OS_IrqHandler: dispatch the lowest pending interrupt.
asm void UserInterruptHandler(void)
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
    ldr r1, =data_02099fe4
    ldr r0, [r1, r0, lsl #2]
    ldr lr, =func_01ffd9d4
    bx r0
}

}

extern "C" {

// @symbol func_01ffd9d4
// OS_IrqHandler_ThreadSwitch: wake waiters, then switch threads if asked.
asm void func_01ffd9d4(void)
{
    ldr ip, =data_023c0000
    ldrh r3, [ip]
    mov r0, #0
    cmp r3, #0
    beq _resched
    strh r0, [ip]
    ldr ip, =data_020a6134
    mov r1, #1
    strh r1, [ip]
    ldr ip, [ip, #0xc]
    mov r2, #1
_wake:
    cmp ip, #0
    beq _resched
    ldr r0, [ip, #0x6c]
    tst r3, r1, lsl r0
    strne r2, [ip, #0x64]
    ldr ip, [ip, #0x68]
    b _wake

_resched:
    ldr ip, =data_020a6134
    ldrh r1, [ip]
    cmp r1, #0
    ldreq pc, [sp], #4
    mov r1, #0
    strh r1, [ip]
    mov r3, #0xd2
    msr cpsr_c, r3
    add r2, ip, #0xc
    ldr r1, [r2]
_pick:
    cmp r1, #0
    ldrneh r0, [r1, #0x64]
    cmpne r0, #1
    ldrne r1, [r1, #0x68]
    bne _pick
    cmp r1, #0
    bne _have
_leave:
    mov r3, #0x92
    msr cpsr_c, r3
    ldr pc, [sp], #4

_have:
    ldr r0, [ip, #8]
    cmp r1, r0
    beq _leave
    ldr r3, [ip, #0x10]
    cmp r3, #0
    beq _switch
    stmdb sp!, {r0, r1, ip}
    mov lr, pc
    bx r3
    ldmia sp!, {r0, r1, ip}

_switch:
    str r1, [ip, #8]
    mrs r2, spsr
    str r2, [r0, #0]!
    stmdb sp!, {r0, r1}
    add r0, r0, #0
    add r0, r0, #0x48
    ldr r1, =ARMMathSaveState
    blx r1
    ldmia sp!, {r0, r1}
    ldmib sp!, {r2, r3}
    stmib r0!, {r2, r3}
    ldmib sp!, {r2, r3, ip, lr}
    stmib r0!, {r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, sp, lr}^
    stmib r0!, {lr}
    mov r3, #0xd3
    msr cpsr_c, r3
    stmib r0!, {sp}
    stmdb sp!, {r1}
    add r0, r1, #0
    add r0, r0, #0x48
    ldr r1, =ARMMathLoadState
    blx r1
    ldmia sp!, {r1}
    ldr sp, [r1, #0x44]
    mov r3, #0xd2
    msr cpsr_c, r3
    ldr r2, [r1, #0]!
    msr spsr_fc, r2
    ldr lr, [r1, #0x40]
    ldmib r1!, {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, sp, lr}^
    mov r0, r0
    stmda sp!, {r0, r1, r2, r3, r12, lr}
    ldmia sp!, {pc}
}

}
