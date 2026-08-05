// HAND-ASM PRIMITIVE: byte-faithful asm-block match.
extern char data_020a6134[];
extern void ARMMathSaveState(void);
extern void ARMMathLoadState(void);

asm void func_01ffd9d4(void) {
    ldr     r12, =0x023c0000
    ldrh    r3, [r12]
    mov     r0, #0
    cmp     r3, #0
    beq     _01ffda1c
    strh    r0, [r12]
    ldr     r12, =data_020a6134
    mov     r1, #1
    strh    r1, [r12]
    ldr     r12, [r12, #0xc]
    mov     r2, #1
_01ffda00:
    cmp     r12, #0
    beq     _01ffda1c
    ldr     r0, [r12, #0x6c]
    tst     r3, r1, lsl r0
    strne   r2, [r12, #0x64]
    ldr     r12, [r12, #0x68]
    b       _01ffda00
_01ffda1c:
    ldr     r12, =data_020a6134
    ldrh    r1, [r12]
    cmp     r1, #0
    ldreq   pc, [sp], #4
    mov     r1, #0
    strh    r1, [r12]
    mov     r3, #0xd2
    msr     cpsr_c, r3
    add     r2, r12, #0xc
    ldr     r1, [r2]
_01ffda44:
    cmp     r1, #0
    ldrneh  r0, [r1, #0x64]
    cmpne   r0, #1
    ldrne   r1, [r1, #0x68]
    bne     _01ffda44
    cmp     r1, #0
    bne     _01ffda6c
_01ffda60:
    mov     r3, #0x92
    msr     cpsr_c, r3
    ldr     pc, [sp], #4
_01ffda6c:
    ldr     r0, [r12, #8]
    cmp     r1, r0
    beq     _01ffda60
    ldr     r3, [r12, #0x10]
    cmp     r3, #0
    beq     _01ffda94
    stmdb   sp!, {r0, r1, r12}
    mov     lr, pc
    bx      r3
    ldmia   sp!, {r0, r1, r12}
_01ffda94:
    str     r1, [r12, #8]
    mrs     r2, spsr
    str     r2, [r0, #0]!
    stmdb   sp!, {r0, r1}
    add     r0, r0, #0
    add     r0, r0, #0x48
    ldr     r1, =ARMMathSaveState
    blx     r1
    ldmia   sp!, {r0, r1}
    ldmib   sp!, {r2, r3}
    stmib   r0!, {r2, r3}
    ldmib   sp!, {r2, r3, r12, lr}
    stmib   r0!, {r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, sp, lr}^
    stmib   r0!, {lr}
    mov     r3, #0xd3
    msr     cpsr_c, r3
    stmib   r0!, {sp}
    stmdb   sp!, {r1}
    add     r0, r1, #0
    add     r0, r0, #0x48
    ldr     r1, =ARMMathLoadState
    blx     r1
    ldmia   sp!, {r1}
    ldr     sp, [r1, #0x44]
    mov     r3, #0xd2
    msr     cpsr_c, r3
    ldr     r2, [r1, #0]!
    msr     spsr_fc, r2
    ldr     lr, [r1, #0x40]
    ldmib   r1!, {r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, sp, lr}^
    mov     r0, r0
    stmda   sp!, {r0, r1, r2, r3, r12, lr}
    ldmia   sp!, {pc}
}
