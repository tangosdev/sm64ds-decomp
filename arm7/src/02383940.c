// NONMATCHING (ASM-PRIMITIVE): byte-exact hand-written asm. There is no original C to recover
// and no match to chase. Counts as done under the asm-primitive policy (notes/asm-policy.md).
// HAND-ASM PRIMITIVE: interworking veneer: tail call through a pooled pointer with BX, no C body (a C wrapper adds a frame or BL).
// @ 0x02383940 size 0x10 (runtime 0x037FB7D8, wram)
/* storage 0x02383940 runtime 0x037FB7D8 size 0x10 wram ARM.
 * ROPI ARM->Thumb veneer (linker long-call stub) to Thumb 0x03800040
 * (SVC_GetVolumeTable, libsyscall swi 0x1C): LDR IP,=0x485D /
 * ADD IP,PC,IP / BX IP + offset pool.
 * Whole-asm: b56 -interworking only emits the 12-byte absolute
 * `LDR IP,=abs; BX IP` thunk; no C construct produces ADD IP,PC,IP
 * (AM12/AR7/AR50 all classified it not C-expressible). Same shape as
 * 02383410.c / fn_02388CD8. Pool word is a numeric pc-relative offset. */

asm int fn_02383940(int v) {
    LDR IP, =0x485D
    ADD IP, PC, IP
    BX IP
}
