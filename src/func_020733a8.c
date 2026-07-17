// HAND-ASM PRIMITIVE: byte-faithful asm-block match. MSL C++ runtime array
// construction (ctor applied forward across n elements) with an exception
// frame (fp anchor + sp spill at [fp,#0x14]) and a landing pad that calls the
// partial array destroyer; no C under our flags reproduces it. Per asm
// policy, sibling of matched func_02073534 / func_0207335c.
extern void func_02073534(void);
extern void func_020717c0(void);
extern void func_02071ba0(void);

asm void func_020733a8(void) {
    stmdb sp!, {r4, r5, r6, r7, r8, r11, lr}
    sub sp, sp, #0x1c
    mov r11, sp
    movs r4, r3
    mov r7, r0
    mov r6, r1
    mov r5, r2
    addeq sp, r11, #0x1c
    ldmeqia sp!, {r4, r5, r6, r7, r8, r11, lr}
    bxeq lr
    ldr r0, [r11, #0x38]
    cmp r0, #0
    beq L440
    mov r8, r7
    str sp, [r11, #0x14]
    cmp r6, #0
    addeq sp, r11, #0x1c
    ldmeqia sp!, {r4, r5, r6, r7, r8, r11, lr}
    bxeq lr
L3f4:
    mov r0, r8
    blx r4
    add r8, r8, r5
    subs r6, r6, #1
    bne L3f4
    add sp, r11, #0x1c
    ldmia sp!, {r4, r5, r6, r7, r8, r11, lr}
    bx lr
    ldr r3, [r11, #0x38]
    mov r0, r7
    mov r1, r8
    mov r2, r5
    bl func_02073534
    bl func_020717c0
    add r0, r11, #0
    bl func_02071ba0
    add sp, r11, #0x1c
    ldmia sp!, {r4, r5, r6, r7, r8, r11, lr}
    bx lr
L440:
    cmp r6, #0
    addeq sp, r11, #0x1c
    ldmeqia sp!, {r4, r5, r6, r7, r8, r11, lr}
    bxeq lr
L450:
    mov r0, r7
    blx r4
    add r7, r7, r5
    subs r6, r6, #1
    bne L450
    add sp, r11, #0x1c
    ldmia sp!, {r4, r5, r6, r7, r8, r11, lr}
    bx lr
}
