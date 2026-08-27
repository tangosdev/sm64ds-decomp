// HAND-ASM PRIMITIVE: byte-faithful asm-block match (assembly-only primitive). Per asm policy.
//
// func_01ffafd4 at 0x01ffafd4 (ITCM) -- resets the floating-point runtime state that the
// soft-float block above it (0x01ff8000-0x01ffafd0: the double add/multiply, the float
// and double conversions, and the integer division helpers) reads and updates.
//
// It clears the status word func_0207322c points at, then the five-word table
// func_02073238 points at plus its head. Those two accessors are the same pair the rest
// of the runtime uses: src/func_0206de14.c takes its dispatch table from func_02073238()
// and its mode bits from func_01ffb008(0, 0).
//
// Assembly in the original, like the whole runtime block it belongs to: it keeps r0 live
// across a `bl` with only `{lr}` pushed, relying on the callee returning its result in r0
// and clobbering nothing else, and it reuses the second callee's return value as the base
// for six stores without ever re-materialising it. mwccarm compiling equivalent C emits a
// callee-saved spill and a real frame; this has neither.

extern int func_0207322c(void);
extern int func_02073238(void);

asm void func_01ffafd4(void)
{
    stmdb sp!, {lr}
    bl func_0207322c
    mov r1, #0
    str r1, [r0]
    bl func_02073238
    mov r1, #0
    str r1, [r0, #4]
    str r1, [r0, #8]
    str r1, [r0, #0xc]
    str r1, [r0, #0x10]
    str r1, [r0, #0x14]
    ldmia sp!, {lr}
    bx lr
}
