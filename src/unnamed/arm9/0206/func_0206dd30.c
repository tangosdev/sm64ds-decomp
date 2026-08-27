// HAND-ASM PRIMITIVE: byte-faithful asm-block match. Prebuilt SDK/MSL softfloat
// runtime glue: float OP float -> float wrapper (floats widened in-place into
// uninitialized double slots) forwarding to the per-thread FP-operation
// dispatcher func_0206de14. Ships as ADS-era library code (below-sp argument
// staging with NO sp adjustment) that no mwccarm build/flags reproduces from C.
// Per asm policy; sibling of matched func_0206da64 in the same library.
extern void func_0206de14(void);

asm float func_0206dd30(float a, float b, int op) {
    stmdb sp!, {lr}
    sub sp, sp, #0x24
    str r0, [sp, #8]
    str r1, [sp, #0x10]
    str r2, [sp, #4]
    ldr r1, [sp, #0x10]
    ldr r0, [sp, #0x14]
    sub r2, sp, #4
    str r1, [r2]
    str r0, [r2, #4]
    add r1, sp, #8
    ldmia r2, {r3}
    add r0, sp, #0x18
    ldmia r1, {r1, r2}
    bl func_0206de14
    ldr r0, [sp, #0x18]
    add sp, sp, #0x24
    ldmia sp!, {lr}
    bx lr
}
