// HAND-ASM PRIMITIVE: byte-faithful asm-block match (assembly-only primitive). Per asm policy.
//
// __ieee_status at 0x01ffb008 (ITCM) -- the ARM runtime's interface to the floating-point
// status word, part of the ARM soft-float library (fplib) whose entries at 0x01ff8000 carry
// their ARM names (_dadd, _dmul, _fflt, _deq, _ll_ufrom_d).
//
// Named from ARM's own specification of the function, which this body implements exactly.
// ARM DUI0475 5.5: `unsigned int __ieee_status(unsigned int mask, unsigned int flags)`
// "modifies the writable parts of the status word according to the parameters, and returns
// the previous value of the whole word", with "new = (old & ~mask) ^ flags". Here:
//
//     old = *__rt_fp_status_addr();  *p = (old & ~mask) ^ flags;  return old;
//
// func_0207322c is the status-word accessor (it returns &data_020aa3f4), the role ARM's
// runtime gives __rt_fp_status_addr. The same document puts the rounding mode in bits 22-23
// of this word, and src/cstd4.cpp reads exactly those bits: __ieee_status(0, 0) & 0xc00000.
// __fp_status next door repeats this read-modify-write inside an older-layout repack.
//
// Assembly in the original: the two arguments are preserved across the `bl` by pushing
// the argument registers themselves, `stmdb sp!, {r0, r1, r2, r12, lr}`. Pushing r0-r2
// and ip is not a frame any compiler builds -- mwccarm would move the arguments into
// callee-saved registers and push those instead.

extern unsigned int *func_0207322c(void);

asm unsigned int __ieee_status(unsigned int mask, unsigned int flags)
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
