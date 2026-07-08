// Long-branch veneer/thunk: load the absolute target address and a fixed data
// argument from the inline literal pool, set r1, and jump to the target. Hand-asm
// because the two trailing .words are relocations (wildcards) and the placeholder
// symbols must stay unmangled with C linkage.
extern void func_ov064_021180d4_target(void);
extern void *func_ov064_021180d4_data;
asm void func_ov064_021180d4(void)
{
    ldr ip, [pc, #4]
    ldr r1, [pc, #4]
    bx ip
    dcd func_ov064_021180d4_target
    dcd func_ov064_021180d4_data
}
