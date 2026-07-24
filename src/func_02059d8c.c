// NONMATCHING: hand-written asm, not a C decompilation. Byte-exact via an asm hatch on a
// proven mwccarm 1.2 register-allocation/scheduling wall; does NOT count as matched. Reverts
// to a draft until someone reproduces the bytes from real C.
asm void func_02059d8c(void) {
L0:
    subs r0, r0, #4;
    bhs L0;
    bx lr
}
