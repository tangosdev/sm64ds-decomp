// NONMATCHING: hand-written asm, not a C decompilation. Byte-exact via an asm hatch on a
// proven mwccarm 1.2 register-allocation/scheduling wall; does NOT count as matched. Reverts
// to a draft until someone reproduces the bytes from real C.
extern void func_0207328c(void);
extern char data_ov006_02140990;
extern void func_ov006_020d1008(void);

asm void func_ov006_020d0fe4(void)
{
    ldr ip, [pc, #0x10]
    ldr r0, [pc, #0x10]
    ldr r3, [pc, #0x10]
    mov r1, #4
    mov r2, #0x32c
    bx ip
    dcd func_0207328c
    dcd data_ov006_02140990
    dcd func_ov006_020d1008
}
