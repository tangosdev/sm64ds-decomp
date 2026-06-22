// HAND-ASM PRIMITIVE: byte-faithful asm-block match (assembly-only primitive). Per asm policy.
extern void func_020731fc(void);
extern void func_02071ba0(void);
extern void func_020717c0(void);

asm void func_0207335c(void) {
    str sp, [r11, #0x2c]
    subs r7, r7, #1
    beq done
loop:
    sub r4, r4, r6
    mov r0, r4
    blx r5
    subs r7, r7, #1
    bne loop
    b done
    bl func_020731fc
    add r0, r11, #0x18
    bl func_02071ba0
    ldr sp, [r11, #0x2c]
done:
    bl func_020717c0
    add r0, r11, #0
    bl func_02071ba0
    add sp, r11, #0x30
    ldmia sp!, {r4, r5, r6, r7, r11, lr}
    bx lr
}
