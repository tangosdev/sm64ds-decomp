// NONMATCHING (ASM-PRIMITIVE): byte-exact hand-written asm. There is no original C to recover
// and no match to chase. Counts as done under the asm-primitive policy (notes/asm-policy.md).
// HAND-ASM PRIMITIVE: interworking veneer: tail call through a pooled pointer with BX, no C body (a C wrapper adds a frame or BL).
// @ 0x02383A78 size 0x10 (runtime 0x037FB910, wram)
/* storage 0x02383A78 runtime 0x037FB910 size 0x10 wram ARM.
 * HAND-ASM VENEER (not C): ARM-to-Thumb tail that jumps to the thumb
 * SVC_GetPitchTable stub (swi 27) at 0x0380003C.
 * No C shape under the pinned flags emits it: b56 -O4,s gives the 12-byte
 * `ldr ip, =abs; bx ip` tail, and -pic gives `add ip, ip, pc` (operands
 * swapped). Three passes (AM12/AR7/AR50) plus prior AH10 agree. The SDK
 * keeps these syscall stubs as hand-written assembly (cf. libsyscall.s:
 * `swi 27; bx lr`), so an `asm void` source is the faithful reconstruction.
 * Zero relocs: the pool word is the literal pc-relative offset 0x4721
 * (0x037FB91C + 0x4721 = 0x0380003D). */

asm void fn_02383A78(void) {
    ldr ip, [pc, #4]
    add ip, pc, ip
    bx ip
    DCD 0x4721
}
