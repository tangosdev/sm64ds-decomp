extern void func_ov091_02132360(void);

asm void func_ov091_02132380(void)
{
    ldr ip, [pc, #8]
    mov r0, r1
    mov r1, r2
    bx ip
    dcd func_ov091_02132360
}
