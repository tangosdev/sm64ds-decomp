// NONMATCHING (ASM-STANDIN): byte-exact whole-function asm for a function that was C; no matching C yet
// @ 0x02380A4C size 0x98 (runtime 0x037F88E4, wram)
// C tried by: the AC C-only redo round; no C attempt reproduced the bytes (see the notes below).
/* storage 0x02380A4C runtime 0x037F88E4 size 0x90 wram. Hand-asm: the bit
   tests are TST (mwccarm emits ANDS for every flag-only AND) and the mask
   stays a separate MOVs (b56 folds `x | c>>r`/`x & ~(c>>r)` into the
   operand). Pool +0x90: dword_27FFFB8, 0xFFFFFFFD. */

extern unsigned int dword_27FFFB8[];

asm int fn_02380A4C(void) {
    ldr r3, =dword_27FFFB8
    ldr r1, [r3]
    mov r2, #0
    mov r0, #0x80000000
loop1:
    tst r1, r0
    bne found1
    add r2, r2, #1
    cmp r2, #32
    beq found1
    mov r0, r0, lsr #1
    b loop1
found1:
    cmp r2, #32
    movne r0, #0x80
    bne join
    add r3, r3, #4
    ldr r1, [r3]
    mov r2, #0
    mov r0, #0x80000000
loop2:
    tst r1, r0
    bne found2
    add r2, r2, #1
    cmp r2, #32
    beq found2
    mov r0, r0, lsr #1
    b loop2
found2:
    cmp r2, #32
    ldr r0, =0xFFFFFFFD
    bxeq lr
    mov r0, #0xA0
join:
    add r0, r0, r2
    mov r1, #0x80000000
    mov r1, r1, lsr r2
    ldr r2, [r3]
    bic r2, r2, r1
    str r2, [r3]
    bx lr
}
