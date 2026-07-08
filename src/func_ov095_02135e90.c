extern void func_ov095_02135cdc(void);

asm void func_ov095_02135e90(void)
{
    ldr ip, [pc, #8]
    mov r0, r1
    mov r1, r2
    bx ip
    dcd func_ov095_02135cdc
}
