// NONMATCHING: hand-written asm, not a C decompilation. Byte-exact via an asm hatch on a
// proven mwccarm 1.2 register-allocation/scheduling wall; does NOT count as matched. Reverts
// to a draft until someone reproduces the bytes from real C.
// Long-branch veneer/thunk: load the absolute target address and a fixed data
// argument from the inline literal pool, set r1, and jump to the target. Hand-asm
// because the two trailing .words are relocations (wildcards) and the placeholder
// symbols must stay unmangled with C linkage.
extern void func_ov064_021180d4_target(void);
extern void *func_ov064_021180d4_data;
asm void _ZN12MetalNetLift16CleanupResourcesEv(void)
{
    ldr ip, [pc, #4]
    ldr r1, [pc, #4]
    bx ip
    dcd func_ov064_021180d4_target
    dcd func_ov064_021180d4_data
}
