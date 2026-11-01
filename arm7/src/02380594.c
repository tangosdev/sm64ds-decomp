// NONMATCHING (ASM-PRIMITIVE): byte-exact hand-written asm. There is no original C to recover
// and no match to chase. Counts as done under the asm-primitive policy (notes/asm-policy.md).
// HAND-ASM PRIMITIVE: position-independent boot-ROM thunk (PC-relative).
// @ 0x02380594 size 0x10 (runtime 0x037F842C, wram)
/* storage 0x02380594 runtime 0x037F842C size 0x10 wram ARM.
 * Thunk: branch to (PC + pool), i.e. 0x438 + 0x7BAF = 0x7FE7 (Thumb boot-ROM
 * area), computed position-independently. A whole-asm function: inline asm
 * would append the compiler's BX LR epilogue. */

asm void fn_02380594(void) {
    LDR R12, =0x7BAF
    ADD R12, PC, R12
    BX R12
}
