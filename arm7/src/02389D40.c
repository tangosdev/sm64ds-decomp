// NONMATCHING (ASM-PRIMITIVE): byte-exact hand-written asm. There is no original C to recover
// and no match to chase. Counts as done under the asm-primitive policy (notes/asm-policy.md).
// HAND-ASM PRIMITIVE: interworking veneer: tail call through a pooled pointer with BX, no C body (a C wrapper adds a frame or BL).
// @ 0x02389D40 size 0x10 (runtime 0x03801BD8, wram)
/* storage 0x02389D40 runtime 0x03801BD8 size 0x10 wram ARM.
 * Thunk: branch to (PC + pool) = 0x03801BE4 + 0xFFFFE407 = 0x037FFFEB,
 * i.e. Thumb entry 0x037FFFEA, computed position-independently (capstone:
 * middle word E08FC00C is `add ip, pc, ip`). Plain C cannot emit the
 * LDR/ADD-PC/BX sequence, so whole-asm like AM1 fn_02380594. */

asm void fn_02389D40(void) {
    LDR R12, =0xFFFFE407
    ADD R12, PC, R12
    BX R12
}
