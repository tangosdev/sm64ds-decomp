//cpp
// @symbol _ZN3IRQ6EnableEv
/* IRQ::Enable(void) at 0x02059d08 -- clear the IRQ disable bit, returning whether IRQs had been
 * disabled.
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

asm unsigned int Enable(void)
{
    mrs r0, cpsr
    bic r1, r0, #0x80
    msr cpsr_c, r1
    and r0, r0, #0x80
    bx lr
}

}
