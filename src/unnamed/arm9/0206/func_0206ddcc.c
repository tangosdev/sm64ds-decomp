// HAND-ASM PRIMITIVE: byte-faithful asm-block match. Prebuilt SDK/MSL softfloat
// runtime glue: unary float -> float wrapper (float widened in-place into an
// uninitialized double slot, duplicated into both dispatcher operand slots)
// forwarding to the per-thread FP-operation dispatcher func_0206de14. Ships as
// ADS-era library code (below-sp argument staging with NO sp adjustment) that
// no mwccarm build/flags reproduces from C. Per asm policy; sibling of matched
// func_0206da64 in the same library.
extern void func_0206de14(void);

asm float func_0206ddcc(float a, int op) {
    stmdb sp!, {lr}
    sub sp, sp, #0x1c
    str r0, [sp, #8]
    str r1, [sp, #4]
    ldr r1, [sp, #8]
    ldr r0, [sp, #0xc]
    sub r2, sp, #4
    str r1, [r2]
    str r0, [r2, #4]
    add r1, sp, #8
    ldmia r2, {r3}
    add r0, sp, #0x10
    ldmia r1, {r1, r2}
    bl func_0206de14
    ldr r0, [sp, #0x10]
    add sp, sp, #0x1c
    ldmia sp!, {lr}
    bx lr
}
