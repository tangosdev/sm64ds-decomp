// NONMATCHING: hand-written asm, not a C decompilation. Byte-exact via an asm hatch on a
// proven mwccarm 1.2 register-allocation/scheduling wall; does NOT count as matched. Reverts
// to a draft until someone reproduces the bytes from real C.
asm int _ZN13RollingLogLll16CleanupResourcesEv(void) {
    ldr r0, [r0, #0x10c]
    cmp r0, #0
    addne r1, r0, #0x324
    ldrneh r0, [r1]
    subne r0, r0, #1
    strneh r0, [r1]
    mov r0, #1
    bx lr
}