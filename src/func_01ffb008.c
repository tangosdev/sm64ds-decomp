// HAND-ASM PRIMITIVE: byte-faithful asm-block match (assembly-only primitive). Per asm policy.
//
// func_01ffb008 at 0x01ffb008 (ITCM) -- read-modify-write of the floating-point runtime
// status word, the accessor the soft-float block at 0x01ff8000 uses on every exceptional
// path (see the `bl` sites inside the float/double converters at 0x01ffa344 and
// 0x01ffa440). Returns the OLD word, so a plain `func_01ffb008(0, 0)` is a pure read --
// which is exactly how src/func_0206de14.c calls it to recover its mode bits.
//
//     old = *p;  *p = (old & ~mask) ^ value;  return old;
//
// Assembly in the original: the two arguments are preserved across the `bl` by pushing
// the argument registers themselves, `stmdb sp!, {r0, r1, r2, r12, lr}`. Pushing r0-r2
// and ip is not a frame any compiler builds -- mwccarm would move the arguments into
// callee-saved registers and push those instead. Saving ip alongside them only makes
// sense for a hand-written routine keeping the stack eight-byte aligned by hand.

extern int func_0207322c(void);

asm unsigned int func_01ffb008(unsigned int mask, unsigned int value)
{
    stmdb sp!, {r0, r1, r2, r12, lr}
    bl func_0207322c
    mov r3, r0
    ldmia sp!, {r0, r1, r2, r12, lr}
    ldr r2, [r3]
    bic r12, r2, r0
    eor r12, r12, r1
    str r12, [r3]
    mov r0, r2
    bx lr
}
