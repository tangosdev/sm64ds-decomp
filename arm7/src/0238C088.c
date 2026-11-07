// NONMATCHING (ASM-PRIMITIVE): byte-exact hand-written asm. There is no original C to recover
// and no match to chase. Counts as done under the asm-primitive policy (notes/asm-policy.md).
// HAND-ASM PRIMITIVE: interworking veneer: tail call through a pooled pointer with BX, no C body (a C wrapper adds a frame or BL).
// @ 0x0238C088 size 0x10 (runtime 0x03803F20, wram)
asm void fn_0238C088(void) {
    LDR R12, [PC, #4]
    ADD R12, PC, R12
    BX R12
    DCD 0xFFFFC0E1
}
