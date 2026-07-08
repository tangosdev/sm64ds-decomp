// Long-branch veneer: load the absolute target address from the inline literal and
// jump to it. Hand-asm because the trailing .word is a relocation to the real target
// (a wildcard) and the placeholder symbol must stay unmangled.
extern void func_ov007_020c8098_target(void);
asm void func_ov007_020c8098(void)
{
    ldr ip, [pc]
    bx ip
    dcd func_ov007_020c8098_target
}
