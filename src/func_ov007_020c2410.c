// NONMATCHING: hand-written asm, not a C decompilation. Byte-exact via an asm hatch on a
// proven mwccarm 1.2 register-allocation/scheduling wall; does NOT count as matched. Reverts
// to a draft until someone reproduces the bytes from real C.
// Long-branch veneer: load the absolute target address from the inline literal and
// jump to it. Hand-asm because the trailing .word is a relocation to the real target
// (a wildcard) and the placeholder symbol must stay unmangled.
extern void func_ov007_020c2410_target(void);
asm void func_ov007_020c2410(void)
{
    ldr ip, [pc]
    bx ip
    dcd func_ov007_020c2410_target
}
