// NONMATCHING: hand-written asm, not a C decompilation. Byte-exact via an asm hatch on a
// proven mwccarm 1.2 register-allocation/scheduling wall; does NOT count as matched. Reverts
// to a draft until someone reproduces the bytes from real C.
// Long-branch veneer: load absolute target into ip and data arg into r1, then bx ip.
extern void func_ov080_0212714c(void);
extern void *data_ov030_021159f4;
asm void _ZN9UkikiCage8BehaviorEv(void)
{
    ldr ip, [pc, #4]
    ldr r1, [pc, #4]
    bx ip
    dcd func_ov080_0212714c
    dcd data_ov030_021159f4
}
