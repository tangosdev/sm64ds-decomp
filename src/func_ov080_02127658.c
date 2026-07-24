// NONMATCHING: hand-written asm, not a C decompilation. Byte-exact via an asm hatch on a
// proven mwccarm 1.2 register-allocation/scheduling wall; does NOT count as matched. Reverts
// to a draft until someone reproduces the bytes from real C.
// Argument-shifting long-branch veneer: drop the first argument (r0), shift r1->r0
// and r2->r1, then load the absolute target address from the inline literal pool and
// tail-jump to it. Hand-asm because the trailing .word is a relocation to the real
// target and the placeholder symbol must stay unmangled with C linkage.
extern void func_ov080_0212758c(void);

asm void func_ov080_02127658(void) {
    ldr ip, [pc, #8]
    mov r0, r1
    mov r1, r2
    bx ip
    dcd func_ov080_0212758c
}
