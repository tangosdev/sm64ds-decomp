// HAND-ASM PRIMITIVE: byte-faithful asm-block match. NitroSDK exception entry: saves the
// faulting context (func_0205947c, which reads its frame through ip and is asm itself), then
// runs the user exception handler (func_0205950c). The SDK writes this wrapper as asm, and
// the bytes show it was never compiled: the frame pushes lr alone and calls out with no
// stack pad. mwccarm always 8-aligns sp before a call, so a push-lr-only frame that calls
// gets `sub sp, sp, #4` (454 of the 456 such frames in arm9 have it; the two that do not
// are this function and func_02059824). The plain C (`a(); b();`) compiles to 0x1c bytes
// with the pad under all 25 installed mwccarm builds and under -O0 through -O4,p, and no
// pragma the compiler parses removes it (optimization_level, optimize_for_size, peephole,
// interworking, oldalignment, old_tail_padding, the opt_* family). The missing pad is the
// whole residual and no C controls it.
extern void func_0205947c(void);
extern void func_0205950c(void);
asm void func_02059468(void) {
    stmdb sp!, {lr}
    bl func_0205947c
    bl func_0205950c
    ldmia sp!, {lr}
    bx lr
}
