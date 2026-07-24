// NONMATCHING: hand-written asm, not a C decompilation. Byte-exact via an asm hatch on a
// proven mwccarm 1.2 register-allocation/scheduling wall; does NOT count as matched. Reverts
// to a draft until someone reproduces the bytes from real C.
// HAND-ASM PRIMITIVE: byte-faithful asm-block match. MSL C++ runtime array
// destroy helper (dtor applied to elements in reverse) with an exception-frame
// (fp anchor + sp spill at [fp,#0x14]) that no C under our flags reproduces;
// its out-of-line landing pad is the already-matched func_020732e8. Per asm
// policy, sibling of matched func_0207335c.
asm void func_0207328c(void) {
    stmdb sp!, {r4, r5, r6, r7, r11, lr}
    sub sp, sp, #0x18
    mov r11, sp
    movs r5, r3
    mov r7, r1
    mov r6, r2
    addeq sp, r11, #0x18
    ldmeqia sp!, {r4, r5, r6, r7, r11, lr}
    bxeq lr
    mla r4, r7, r6, r0
    str sp, [r11, #0x14]
    cmp r7, #0
    addeq sp, r11, #0x18
    ldmeqia sp!, {r4, r5, r6, r7, r11, lr}
    bxeq lr
loop:
    sub r4, r4, r6
    mov r0, r4
    blx r5
    subs r7, r7, #1
    bne loop
    add sp, r11, #0x18
    ldmia sp!, {r4, r5, r6, r7, r11, lr}
    bx lr
}
