//cpp
// @symbol _ZN3IRQ7RestoreEj
/* IRQ::Restore(unsigned int state) at 0x02059d30 -- put the IRQ bit back to a state a previous Disable returned.
 *
 * HAND-ASM PRIMITIVE. `mrs`/`msr` read and write the CPSR and no C construct
 * compiles to them, so the asm block is the faithful source rather than a
 * transcription of something lost. The asm-primitive policy is unchanged; what
 * changes is that the compiler mangles the symbol instead of the file spelling
 * it by hand.
 *
 * The pattern in all five: read CPSR, set or clear the mask bits, write it
 * back, and return the OLD state of those bits so the caller can restore it.
 * Bit 0x80 is IRQ, 0x40 is FIQ -- which is the whole difference between the
 * plain and the `All` variants.
 *
 * Verified layout-free against the ROM, not just against the audit: the address
 * of this function is never stored as a word anywhere in arm9_dec.bin, so it is
 * in no vtable and no dispatch table. */
namespace IRQ {

asm unsigned int Restore(unsigned int state)
{
    mrs r1, cpsr
    bic r2, r1, #0x80
    orr r2, r2, r0
    msr cpsr_c, r2
    and r0, r1, #0x80
    bx lr
}

}
