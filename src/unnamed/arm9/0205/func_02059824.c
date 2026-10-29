// HAND-ASM PRIMITIVE: byte-faithful asm-block match. NitroSDK alarm timer interrupt entry:
// a call wrapper around the alarm handler body (func_02059834). The SDK writes this wrapper
// as asm, and the bytes show it was never compiled. Two independent walls: the plain C
// (`f();`) becomes a tail call (`ldr ip, [pc]; bx ip`, 0xc bytes), and once the tail call is
// defeated the frame still gets `sub sp, sp, #4`, because mwccarm 8-aligns sp before every
// call (454 of the 456 push-lr-only frames that call in arm9 carry the pad; the two that do
// not are this function and func_02059468). Measured under all 25 installed mwccarm builds,
// -O0 through -O4,p, and every pragma the compiler parses that touches codegen. The earlier
// "return-fold wall" reading (ldmia sp!,{lr}; bx lr folded into ldmia sp!,{pc}) was wrong:
// the 0xc candidate is the tail call, not a folded return.
extern void func_02059834(void);
asm void func_02059824(void) {
    stmdb sp!, {lr}
    bl func_02059834
    ldmia sp!, {lr}
    bx lr
}
