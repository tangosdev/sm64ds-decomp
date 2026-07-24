// NONMATCHING: hand-written asm, not a C decompilation. Byte-exact via an asm hatch on a
// proven mwccarm 1.2 register-allocation/scheduling wall; does NOT count as matched. Reverts
// to a draft until someone reproduces the bytes from real C.
asm void func_020729e8(void) { add sp, sp, #0xac; ldmia sp!, {r4, r5, r6, r7, r8, r9, r10, r11, lr}; bx lr }
