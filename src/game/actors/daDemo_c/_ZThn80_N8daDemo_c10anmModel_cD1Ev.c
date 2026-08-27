// NONMATCHING: hand-written asm, not a C decompilation. Byte-exact via an asm hatch on a
// proven mwccarm 1.2 register-allocation/scheduling wall; does NOT count as matched. Reverts
// to a draft until someone reproduces the bytes from real C.
// This-adjusting virtual destructor thunk (adjustment -0x50 = -80): shift `this` from
// the dExtFrameCtrl_c-in-dExtAnmModel_c secondary base at +80 back to the complete
// daDemo_c::anmModel_c object, then tail-branch to the real destructor _ZN8daDemo_c10anmModel_cD1Ev
// at 0x020f6870 (a relocation wildcard). Hand-asm because mwccarm emits the thunk only
// as a compiler byproduct of the key-function TU; it gets folded into the class's own
// TU when daDemo_c::anmModel_c is migrated whole.
void _ZN8daDemo_c10anmModel_cD1Ev(void);
asm void _ZThn80_N8daDemo_c10anmModel_cD1Ev(void)
{
    ldr ip, [pc, #4]
    add r0, r0, ip
    b _ZN8daDemo_c10anmModel_cD1Ev
    dcd 0xffffffb0
}
