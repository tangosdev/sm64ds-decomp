// NONMATCHING (ASM-PRIMITIVE): byte-exact hand-written asm. There is no original C to recover
// and no match to chase. Counts as done under the asm-primitive policy (notes/asm-policy.md).
// HAND-ASM PRIMITIVE: interworking veneer: tail call through a pooled pointer with BX, no C body (a C wrapper adds a frame or BL).
// @ 0x0238C0A8 size 0xC (runtime 0x03803F40, wram)
/* storage 0x0238C0A8 runtime 0x03803F40 size 0xC wram ARM.
 * Shared epilogue for fn_0238C098: drops the stack word and returns.
 * Whole-asm: only 3 words; any C spelling costs at least a frame word. */

asm void fn_0238C0A8(void) {
    ADD SP, SP, #4
    LDMIA SP!, {LR}
    BX LR
}
