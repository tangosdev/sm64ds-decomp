// HAND-ASM PRIMITIVE: byte-faithful asm-block match. MSL C++ runtime array
// destroy helper (reverse dtor loop, 0x30-byte exception frame with sp spill
// at [fp,#0x14]); its out-of-line landing pad is the already-matched
// func_0207335c. No C under our flags reproduces the exception frame. Per asm
// policy, sibling of matched func_0207335c / func_020732e8.
asm void func_02073300(void) {
    stmdb sp!, {r4, r5, r6, r7, r11, lr}
    sub sp, sp, #0x30
    mov r11, sp
    movs r5, r3
    mov r7, r1
    mov r6, r2
    addeq sp, r11, #0x30
    ldmeqia sp!, {r4, r5, r6, r7, r11, lr}
    bxeq lr
    mla r4, r7, r6, r0
    str sp, [r11, #0x14]
    cmp r7, #0
    addeq sp, r11, #0x30
    ldmeqia sp!, {r4, r5, r6, r7, r11, lr}
    bxeq lr
loop:
    sub r4, r4, r6
    mov r0, r4
    blx r5
    subs r7, r7, #1
    bne loop
    add sp, r11, #0x30
    ldmia sp!, {r4, r5, r6, r7, r11, lr}
    bx lr
}
