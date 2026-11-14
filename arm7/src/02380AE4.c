// NONMATCHING (ASM-STANDIN): byte-exact whole-function asm for a function that was C; no matching C yet
// @ 0x02380AE4 size 0x30 (runtime 0x037F897C, wram)
// C tried by: the AC C-only redo round; no C attempt reproduced the bytes (see the notes below).
/* storage 0x02380AE4 runtime 0x037F897C size 0x2C wram. Hand-asm: the mask
   stays a separate MOVs — b56 always folds `x | c>>r` into one ORR operand.
   Pool +0x28: dword_27FFFB8. */

extern unsigned int dword_27FFFB8[];

asm int fn_02380AE4(int a1) {
    ldr r3, =dword_27FFFB8
    cmp r0, #0xA0
    addpl r3, r3, #4
    subpl r0, r0, #0xA0
    submi r0, r0, #0x80
    mov r1, #0x80000000
    mov r1, r1, lsr r0
    ldr r2, [r3]
    orr r2, r2, r1
    str r2, [r3]
    bx lr
}
