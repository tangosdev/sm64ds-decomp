// HAND-ASM PRIMITIVE: IRQ/FIQ context save; reads source frame from ip/r12 (not a
// normal C arg) and writes into data_020a63ac. Sibling func_0201cb2c is unrelated
// pointer-setup; this matches func_02059468's callee chain as asm-only.
extern char data_020a63ac;

asm void func_0205947c(int arg0) {
    ldr r1, =data_020a63ac
    mrs r2, cpsr
    str r2, [r1, #0x74]
    str r0, [r1, #0x6c]
    ldr r0, [ip]
    str r0, [r1, #4]
    ldr r0, [ip, #4]
    str r0, [r1, #8]
    ldr r0, [ip, #8]
    str r0, [r1, #0xc]
    ldr r0, [ip, #0xc]
    str r0, [r1, #0x10]
    ldr r2, [ip, #0x10]
    bic r2, r2, #1
    add r0, r1, #0x14
    stmia r0, {r4, r5, r6, r7, r8, r9, r10, r11}
    str ip, [r1, #0x70]
    ldr r0, [r2]
    str r0, [r1, #0x64]
    ldr r3, [r2, #4]
    str r3, [r1]
    ldr r0, [r2, #8]
    str r0, [r1, #0x34]
    ldr r0, [r2, #0xc]
    str r0, [r1, #0x40]
    mrs r0, cpsr
    orr r3, r3, #0x80
    bic r3, r3, #0x20
    msr cpsr_fsxc, r3
    str sp, [r1, #0x38]
    str lr, [r1, #0x3c]
    mrs r2, spsr
    str r2, [r1, #0x7c]
    msr cpsr_fsxc, r0
    bx lr
}