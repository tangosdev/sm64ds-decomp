// NONMATCHING (ASM-PRIMITIVE): byte-exact hand-written asm. There is no original C to recover
// and no match to chase. Counts as done under the asm-primitive policy (notes/asm-policy.md).
// HAND-ASM PRIMITIVE: SWP (atomic swap), which C cannot express.
// @ 0x02382EF0 size 0x8 (runtime 0x037FAD88, wram)
/* storage 0x02382EF0 runtime 0x037FAD88 size 0x8 wram ARM.
 * Atomic swap (SWP R0,R0,[R1]). No pool. */

unsigned int fn_02382EF0(unsigned int a1, unsigned int *a2) {
    __asm {
        SWP R0, R0, [R1]
    }
}
