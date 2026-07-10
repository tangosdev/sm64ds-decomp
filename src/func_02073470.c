// HAND-ASM PRIMITIVE: byte-faithful asm-block match. MSL C++ runtime array
// construct-with-cleanup: allocates count*size+cookie, writes the array cookie,
// constructs each element via an indirect ctor, with an exception landing pad at
// 0x020734f4 (jumped over by normal flow, reached only via unwinding) that no C
// under our flags reproduces. Per asm policy, sibling of matched func_02073534 /
// func_020732e8 / func_0207335c.
extern void _ZN6Memory13operator_new2Ej(void);
extern void func_0203cbc0(void);
extern void func_020717c0(void);
extern void func_02071ba0(void);
extern void func_02073534(void);

asm void func_02073470(void) {
    stmdb sp!, {r4, r5, r6, r7, r8, r9, r10, r11, lr}
    sub sp, sp, #0x1c
    mov r11, sp
    mov r8, r0
    mov r7, r1
    mov r6, r2
    mla r0, r8, r7, r6
    mov r5, r3
    bl _ZN6Memory13operator_new2Ej
    movs r9, r0
    addeq sp, r11, #0x1c
    moveq r0, #0
    ldmeqia sp!, {r4, r5, r6, r7, r8, r9, r10, r11, lr}
    bxeq lr
    cmp r6, #0
    beq L_c0
    add r0, r9, r6
    str r8, [r0, #-4]
    cmp r6, #8
    strhs r7, [r0, #-8]
L_c0:
    cmp r5, #0
    beq L_end
    add r10, r9, r6
    mov r4, r10
    str sp, [r11, #0x14]
    cmp r8, #0
    beq L_end
L_loop:
    mov r0, r4
    blx r5
    add r4, r4, r7
    subs r8, r8, #1
    bne L_loop
    b L_end
    ldr r3, [r11, #0x40]
    cmp r3, #0
    beq L_10
    mov r0, r10
    mov r1, r4
    mov r2, r7
    bl func_02073534
L_10:
    mov r0, r9
    bl func_0203cbc0
    bl func_020717c0
    add r0, r11, #0
    bl func_02071ba0
L_end:
    add r0, r9, r6
    add sp, r11, #0x1c
    ldmia sp!, {r4, r5, r6, r7, r8, r9, r10, r11, lr}
    bx lr
}
