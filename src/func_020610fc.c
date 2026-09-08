// NONMATCHING: hand-written asm, not a C decompilation. Byte-exact via an asm hatch on a
// proven mwccarm 1.2 register-allocation/scheduling wall; does NOT count as matched. Reverts
// to a draft until someone reproduces the bytes from real C.
// HAND-ASM: IPC send loop (cmd 0xd, arg 2) followed by an intentional hang
// (b self). The routine never returns, so mwccarm will not emit a function
// that ends on a backward branch to itself with no epilogue at all.
extern int IPCSend(unsigned int a, unsigned int c, unsigned int b);

asm void func_020610fc(void) {
    stmdb sp!, {r4, r5, r6, lr}
    mov r6, #0xd
    mov r5, #2
    mov r4, #0
L10:
    mov r0, r6
    mov r1, r5
    mov r2, r4
    bl IPCSend
    cmp r0, #0
    bne L10
L24:
    b L24
}
