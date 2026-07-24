// NONMATCHING: hand-written asm, not a C decompilation. Byte-exact via an asm hatch on a
// proven mwccarm 1.2 register-allocation/scheduling wall; does NOT count as matched. Reverts
// to a draft until someone reproduces the bytes from real C.
// This-adjusting virtual destructor thunk (adjustment -0x50): shift `this` from the
// secondary base back to the complete object, then tail-branch to the real destructor
// (a relocation wildcard). Hand-asm because mwccarm emits the thunk only as a mangled
// compiler byproduct; the placeholder symbol must stay unmangled here.
void func_ov002_020f8848_target(void);
asm void func_ov002_020f8848(void)
{
    ldr ip, [pc, #4]
    add r0, r0, ip
    b func_ov002_020f8848_target
    dcd 0xffffffb0
}
