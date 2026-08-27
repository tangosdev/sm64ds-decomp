// NONMATCHING (NOT-C-EXPRESSIBLE): byte-exact hand-written asm. Nintendo shipped this as an
// assembly primitive, so there is no original C to recover and no match to chase. Counts as
// done under the asm-primitive policy - see notes/asm-policy.md.
// bhs reads the CARRY flag, which is a property of the value before the subtraction. C has
// no expression over the post-subtraction result that denotes carry, and mwccarm fuses a
// subtract with its following test only when the test is against zero (Z/N), so the HS
// predicate is unreachable at the exact 12-byte shape. Established across 20 mwccarm builds
// x 7 optimization levels x 8 pragmas - see notes/func_02059d8c-asm-origin.md.
asm void func_02059d8c(void) {
L0:
    subs r0, r0, #4;
    bhs L0;
    bx lr
}
