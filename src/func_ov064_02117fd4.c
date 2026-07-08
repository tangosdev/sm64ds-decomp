// Argument-shifting long-branch veneer: drop the first argument (r0), shift r1->r0
// and r2->r1, then load the absolute target address from the inline literal pool and
// tail-jump to it. Hand-asm because the trailing .word is a relocation to the real
// target (a wildcard) and the placeholder symbol must stay unmangled with C linkage.
extern void func_ov064_02117fb4(void);
asm void func_ov064_02117fd4(void)
{
    ldr ip, [pc, #8]
    mov r0, r1
    mov r1, r2
    bx ip
    dcd func_ov064_02117fb4
}
