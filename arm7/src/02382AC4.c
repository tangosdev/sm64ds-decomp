// NONMATCHING (ASM-PRIMITIVE): byte-exact hand-written asm. There is no original C to recover
// and no match to chase. Counts as done under the asm-primitive policy (notes/asm-policy.md).
// HAND-ASM PRIMITIVE: MRS/MSR (CPSR F-bit), which C cannot express.
// @ 0x02382AC4 size 0x14 (runtime 0x037FA95C, wram)
/* storage 0x02382AC4 runtime 0x037FA95C size 0x14 wram ARM.
 * Set CPSR I-bit (disable IRQ), return previous I-bit. MRS/MSR are not
 * expressible in plain C; inline asm per the ARMProcessorMode.c precedent. */

int fn_02382AC4(void) {
    int old;
    int tmp;
    asm {
        mrs old, cpsr
        orr tmp, old, #0x80
        msr cpsr_c, tmp
        and old, old, #0x80
    }
    return old;
}
