// NONMATCHING (ASM-PRIMITIVE): byte-exact hand-written asm. There is no original C to recover
// and no match to chase. Counts as done under the asm-primitive policy (notes/asm-policy.md).
// HAND-ASM PRIMITIVE: MRS (CPSR read), which C cannot express.
// @ 0x02382B1C size 0xC (runtime 0x037FA9B4, wram)
/* storage 0x02382B1C runtime 0x037FA9B4 size 0xC wram ARM. No pool, no calls. */

unsigned int fn_02382B1C(void) {
    unsigned int psr;
    asm {
        MRS psr, CPSR
    }
    return psr & 0x1F;
}
