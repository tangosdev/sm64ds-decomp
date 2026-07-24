// NONMATCHING: hand-written asm, not a C decompilation. Byte-exact via an asm hatch on a
// proven mwccarm 1.2 register-allocation/scheduling wall; does NOT count as matched. Reverts
// to a draft until someone reproduces the bytes from real C.
extern int data_ov055_02111b68;
extern void func_0201277c(int a);

asm void func_ov055_02111264(void)
{
    ldr r1, [pc, #0x10]
    mov r2, #2
    ldr ip, [pc, #0xc]
    ldr r0, [pc, #0xc]
    str r2, [r1]
    bx ip
    dcd data_ov055_02111b68
    dcd func_0201277c
    dcd 0x179
}
