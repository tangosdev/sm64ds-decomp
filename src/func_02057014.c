// NONMATCHING (ASM-PRIMITIVE): byte-exact hand-written asm. Nintendo shipped this as an
// assembly primitive, so there is no original C to recover and no match to chase. Counts as
// done under the asm-primitive policy - see notes/arm9-endgame.md.
// HAND-ASM PRIMITIVE: byte-faithful asm-block match. This is NitroSDK OS_UnLockCartridge,
// the misspelled-name compatibility thunk for OS_UnlockCartridge (= func_02057178, matched
// at src/func_02057178.c). Nintendo's own SDK source writes it as assembly:
// OS_spinLock.c carries `asm s32 OS_UnLockCartridge(u16 lockID) { ldr r1, =OS_UnlockCartridge;
// bx r1 }`, which is this function instruction for instruction. The surrounding cluster is the
// same TU: func_02057128 = OSi_AllocateCartridgeBus, func_02057228 = OSi_DoLockByWord,
// 0x027fffe8 = HW_CTRDG_LOCK_BUF, and func_02057020/func_02057078 are OS_ReleaseLockID, also
// `asm` there. That is direct evidence for the first row of notes/asm-policy.md's table
// (original was hand-written asm), not an inference from the instruction set.
// Measured, so the claim is not just provenance: a compiled tail-call veneer always routes
// through ip, never r1. Ten source spellings (plain forwarder, void forwarder, dead extra
// argument, unprototyped and variadic callee, function-pointer local, typedef'd pointer,
// cast-through-pointer call, 5-argument forwarder, `#pragma interworking on`) were compiled
// against all 25 installed mwccarm builds; every one emits `ldr ip,[pc]; bx ip`. The register
// is the whole residual, and no C expression names a register.
extern void func_02057178(void);

asm void func_02057014(void) {
    ldr r1, =func_02057178
    bx r1
}
