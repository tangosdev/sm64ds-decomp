// HAND-ASM PRIMITIVE: byte-faithful asm-block match (assembly-only primitive:
// IRQ/CPSR control -- FIQ-disable (F bit, 0x40) toggle with dual entry points
// and an indirect literal-pool jump to the shared body). No C to decompile to.
// Per asm policy. Sibling of matched func_0206da64 (CP15 variant).
extern void func_0206d9e4(void); // shared body label at +0x18 inside this function

asm int func_0206d9cc(void) {
    mov r0, #0
    b lbl
    mov r0, #1
    b lbl
lbl:
    ldr r1, =func_0206d9e4
    bx r1
    mrs r3, cpsr
    mov r1, r3
    and r1, r1, #0x40
    mov r2, #0x40
    tst r0, r0
    orreq r3, r3, r2
    bicne r3, r3, r2
    msr cpsr_fc, r3
    tst r1, r1
    moveq r0, #1
    movne r0, #0
    bx lr
}
