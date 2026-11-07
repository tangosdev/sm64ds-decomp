// NONMATCHING (ASM-PRIMITIVE): byte-exact hand-written asm. There is no original C to recover
// and no match to chase. Counts as done under the asm-primitive policy (notes/asm-policy.md).
// HAND-ASM PRIMITIVE: MRS/MSR (CPSR I+F bits), which C cannot express.
// @ 0x02382AD8 size 0x18 (runtime 0x037FA970, wram)
/* storage 0x02382AD8 runtime 0x037FA970 size 0x18 wram ARM.
 * Restore CPSR I-bit from argument, return previous I-bit. MRS/MSR are not
 * expressible in plain C; inline asm per the ARMProcessorMode.c precedent. */

int fn_02382AD8(int a1) {
    int tmp;
    int old;
    int ret;
    asm {
        mrs old, cpsr
        bic tmp, old, #0x80
        orr tmp, tmp, a1
        msr cpsr_c, tmp
        and ret, old, #0x80
    }
    return ret;
}
