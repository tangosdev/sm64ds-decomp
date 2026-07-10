extern void func_ov029_021116c4(void);

asm void func_ov029_021116e4(void)
{
    ldr ip, [pc, #8]
    mov r0, r1
    mov r1, r2
    bx ip
    dcd func_ov029_021116c4
}
