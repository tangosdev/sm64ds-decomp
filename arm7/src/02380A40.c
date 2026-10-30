// NONMATCHING (ASM-PRIMITIVE): byte-exact hand-written asm. There is no original C to recover
// and no match to chase. Counts as done under the asm-primitive policy (notes/asm-policy.md).
// HAND-ASM PRIMITIVE: interworking veneer: tail call through a pooled pointer with BX, no C body (a C wrapper adds a frame or BL).
// @ 0x02380A40 size 0xC (runtime 0x037F88D8, wram)
/* storage 0x02380A40 runtime 0x037F88D8 size 0xC wram. Hand-asm thunk:
   jumps through R1 into the middle of the word-2 scan loop at loc_37F89EC;
   no C call shape can target a mid-function label. Pool +0x8: loc_37F89EC. */

extern void loc_37F89EC(void);

asm void fn_02380A40(void) {
    ldr r1, =loc_37F89EC
    bx r1
}
