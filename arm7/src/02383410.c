// NONMATCHING (ASM-PRIMITIVE): byte-exact hand-written asm. There is no original C to recover
// and no match to chase. Counts as done under the asm-primitive policy (notes/asm-policy.md).
// HAND-ASM PRIMITIVE: interworking veneer: tail call through a pooled pointer with BX, no C body (a C wrapper adds a frame or BL).
// @ 0x02383410 size 0x10 (runtime 0x037FB2A8, wram)
/* storage 0x02383410 runtime 0x037FB2A8 size 0x10 wram ARM.
 * ARM->Thumb interworking veneer (linker long-call stub) to Thumb
 * 0x037FFFFA: LDR IP,=0x4D47 / ADD IP,PC,IP / BX IP + pool.
 * Whole-asm so no C prologue (AM24 fn_02388CD8 precedent: no C construct
 * emits ADD IP,PC,IP; AM11 classified it not C-expressible). No BL; pool
 * word is a pc-relative offset (numeric, not ABS32). IDA-verified. */

asm void fn_02383410(void) {
    LDR IP, =0x4D47
    ADD IP, PC, IP
    BX IP
}
