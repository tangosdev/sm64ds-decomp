extern void func_ov073_02122730(void);

asm void func_ov073_021227d0(void) {
    ldr ip, [pc, #8]
    mov r0, r1
    mov r1, r2
    bx ip
    dcd func_ov073_02122730
}
