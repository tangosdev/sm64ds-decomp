// NONMATCHING: hand-written asm, not a C decompilation. Byte-exact via an asm hatch on a
// proven mwccarm 1.2 register-allocation/scheduling wall; does NOT count as matched. Reverts
// to a draft until someone reproduces the bytes from real C.
// HAND-ASM PRIMITIVE: byte-faithful asm-block match. Veneer that tail-calls
// func_02057178 through r1 (compiled C always uses ip for tail-call veneers,
// so this register choice marks hand-written assembly).
extern void func_02057178(void);

asm void func_02057014(void) {
    ldr r1, =func_02057178
    bx r1
}
