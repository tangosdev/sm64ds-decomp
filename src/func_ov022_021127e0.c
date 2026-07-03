asm int func_ov022_021127e0(void) {
    ldr r0, [r0, #0x10c]
    cmp r0, #0
    addne r1, r0, #0x324
    ldrneh r0, [r1]
    subne r0, r0, #1
    strneh r0, [r1]
    mov r0, #1
    bx lr
}