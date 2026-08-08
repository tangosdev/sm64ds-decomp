//cpp
// @symbol _ZN3IRQ10DisableAllEv
/* IRQ::DisableAll(void) at 0x02059d48 -- set both the IRQ and FIQ disable bits (0xc0).
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

asm unsigned int DisableAll(void)
{
    mrs r0, cpsr
    orr r1, r0, #0xc0
    msr cpsr_c, r1
    and r0, r0, #0xc0
    bx lr
}

}
