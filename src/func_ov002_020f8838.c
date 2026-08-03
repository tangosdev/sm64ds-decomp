void func_ov002_020f8838_target(void);
asm void func_ov002_020f8838(void)
{
    ldr ip, [pc, #4]
    add r0, r0, ip
    b func_ov002_020f8838_target
    dcd 0xffffffb0
}
