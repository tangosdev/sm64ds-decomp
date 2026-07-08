// This-adjusting virtual destructor thunk (adjustment -0x50): shift `this` from the
// secondary base back to the complete object, then tail-branch to the real destructor
// (a relocation wildcard). Hand-asm because mwccarm emits the thunk only as a mangled
// compiler byproduct; the placeholder symbol must stay unmangled here.
void func_ov002_020f8838_target(void);
asm void func_ov002_020f8838(void)
{
    ldr ip, [pc, #4]
    add r0, r0, ip
    b func_ov002_020f8838_target
    dcd 0xffffffb0
}
