// Argument-shifting long-branch veneer: drop r0, shift r1->r0 and r2->r1, then jump
// to the absolute target loaded from the inline literal. Hand-asm because the trailing
// .word is a relocation to func_ov066_0211a2e4 (a wildcard) and the placeholder symbol
// must stay unmangled.
extern void func_ov066_0211a2e4(void);
asm void func_ov066_0211a35c(void)
{
    ldr ip, [pc, #8]
    mov r0, r1
    mov r1, r2
    bx ip
    dcd func_ov066_0211a2e4
}
