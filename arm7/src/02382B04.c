// NONMATCHING (ASM-PRIMITIVE): byte-exact hand-written asm. There is no original C to recover
// and no match to chase. Counts as done under the asm-primitive policy (notes/asm-policy.md).
// HAND-ASM PRIMITIVE: MRS/MSR (CPSR), which C cannot express.
// @ 0x02382B04 size 0x18 (runtime 0x037FA99C, wram)
/* storage 0x02382B04 runtime 0x037FA99C size 0x18 wram ARM. No pool, no calls. */

int fn_02382B04(int a1) {
    int tmp, old, ret;
    asm {
        MRS old, CPSR
        BIC tmp, old, #0xC0
        ORR tmp, tmp, a1
        MSR CPSR_c, tmp
        AND ret, old, #0xC0
    }
    return ret;
}
