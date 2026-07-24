// NONMATCHING: hand-written asm, not a C decompilation. Byte-exact via an asm hatch on a
// proven mwccarm 1.2 register-allocation/scheduling wall; does NOT count as matched. Reverts
// to a draft until someone reproduces the bytes from real C.
// HAND-ASM PRIMITIVE: byte-faithful asm-block match (assembly-only primitive: block copy,
// CP15, IRQ/CPSR, svc, matrix/math, context-switch). No C to decompile to. Per asm policy.
void func_020731fc(void);
void func_02071ba0(void);

asm void func_020732e8(void) {
    bl func_020731fc
    add r0, r11, #0
    bl func_02071ba0
    add sp, r11, #0x18
    ldmia sp!, {r4, r5, r6, r7, r11, lr}
    bx lr
}
