extern void func_ov029_02111e40(void);

asm void func_ov029_02111e60(void)
{
    ldr ip, [pc, #8]
    mov r0, r1
    mov r1, r2
    bx ip
    dcd func_ov029_02111e40
}
