// HAND-ASM PRIMITIVE: byte-faithful asm-block match (assembly-only primitive). Per asm policy.
//
// func_01ffd9d4 at 0x01ffd9d4 (ITCM) -- the thread rescheduler / context switch.
// Runs off the end of the IRQ path, so it never pushes a frame of its own: both
// early exits are a bare `ldr pc, [sp], #4` popping the caller's lr.
//
// Qualifies for the assembly exception under notes/asm-policy.md on four counts,
// any one of which is sufficient:
//   * `msr cpsr_c` / `mrs r2, spsr` / `msr spsr_fc` -- processor mode switching
//     (IRQ 0xd2 <-> SVC 0xd3 <-> user-mask 0x92) and the saved status register.
//   * `stmib r0!, {r2-lr}^` / `ldmib r1!, {r0-lr}^` -- the `^` suffix banks the
//     *user-mode* register file while running in IRQ mode. That transfer is the
//     whole point of the routine and C has no way to name those registers.
//   * `stmda sp!, {...}` (decrement-after) and `ldr pc, [sp], #4` as the return.
//   * `mov lr, pc; bx r3` -- a hand-rolled indirect call that keeps the ARM state.
//
// The two math-state calls are the same ARMMathSaveState/ARMMathLoadState pair
// ARMSaveContext.c uses, reached through a pool word and `blx` rather than `bl`.
//
// The context block: +0x00 spsr, +0x04.. the banked user registers, +0x40 lr,
// +0x44 the thread's own sp, +0x48 the math state ARMMath{Save,Load}State own.
// data_020a6134 is the scheduler head: +0x00 a halfword "reschedule pending"
// flag, +0x08 the current thread, +0x0c the thread list, +0x10 an optional
// pre-switch hook. Per-thread: +0x64 state, +0x68 next, +0x6c priority bit.
// data_023c0000 (DTCM) is the halfword wake mask func_02057000 clears.

extern short data_023c0000[];
extern char data_020a6134;
extern void ARMMathSaveState(void);
extern void ARMMathLoadState(void);

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
