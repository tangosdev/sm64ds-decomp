// NONMATCHING: hand-written asm, not a C decompilation. Byte-exact via an asm hatch on a
// proven mwccarm 1.2 register-allocation/scheduling wall; does NOT count as matched. Reverts
// to a draft until someone reproduces the bytes from real C.
// HAND-ASM PRIMITIVE: byte-faithful asm-block match (assembly-only primitive: block copy,
// CP15, IRQ/CPSR, svc, matrix/math, context-switch). No C to decompile to. Per asm policy.
asm void func_02059d98(void *p, unsigned int n) {
    mov r1, r1, lsr #5
    mov r1, r1, lsl #5
    add r2, r0, r1
    mov r3, #0
    mov r4, r3
    mov r5, r3
    mov r6, r3
    mov r7, r3
    mov r8, r3
    mov r9, r3
    mov r10, r3
loop:
    cmp r0, r2
    stmltia r0!, {r3, r4, r5, r6, r7, r8, r9, r10}
    blt loop
    bx lr
}
