extern int data_ov055_02111b68;
extern void func_0201277c(int a);

asm void func_ov055_02111264(void)
{
    ldr r1, [pc, #0x10]
    mov r2, #2
    ldr ip, [pc, #0xc]
    ldr r0, [pc, #0xc]
    str r2, [r1]
    bx ip
    dcd data_ov055_02111b68
    dcd func_0201277c
    dcd 0x179
}
