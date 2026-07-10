// HAND-ASM PRIMITIVE: byte-faithful asm-block match. MSL C++ runtime partial
// array destroy (dtor applied downward from 'current' back to 'base') with an
// exception frame (fp anchor + sp spill at [fp,#0x14]) that no C under our
// flags reproduces; its landing pad follows at 0x02073584. Per asm policy,
// sibling of matched func_0207335c / func_020732e8.
asm void func_02073534(void) {
    stmdb sp!, {r4, r5, r6, r7, r11, lr}
    sub sp, sp, #0x18
    mov r11, sp
    mov r7, r0
    mov r6, r1
    mov r5, r2
    mov r4, r3
    str sp, [r11, #0x14]
    cmp r6, r7
    addls sp, r11, #0x18
    ldmlsia sp!, {r4, r5, r6, r7, r11, lr}
    bxls lr
loop:
    sub r6, r6, r5
    mov r0, r6
    blx r4
    cmp r6, r7
    bhi loop
    add sp, r11, #0x18
    ldmia sp!, {r4, r5, r6, r7, r11, lr}
    bx lr
}
