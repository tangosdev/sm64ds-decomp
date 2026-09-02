/* PORT_HOST_ABI. HOST COPY of dScMgSmartball_c vtable slot 18,
 * func_ov006_02118a8c. Run mg5, lane SMB.
 *
 * WHY THIS FILE EXISTS. src/func_ov006_02118a8c.cpp is MATCHED and correct for
 * the ROM, and it is wrong on the host for one reason: it drops an argument
 * that ARM rides through in r0. This is the same defect family
 * port/mg_fanout_costs.txt section 6 records as (b) and (c), found here in
 * ov006 rather than in ov004 and in this class's own code rather than the
 * framework's.
 *
 * THE ROM, read out of extracted/overlays/overlay_0006.bin at base 0x020bfec0:
 *
 *     02118a8c  stmdb sp!, {lr}
 *     02118a90  sub   sp, sp, #4
 *     02118a94  bl    #0x2115b0c          <- r0 is still the caller's self
 *     02118a98  mov   ip, #0xa
 *     02118a9c  ldr   r0, [pc, #0x38]     ; =0x04000050
 *     02118aa0  mov   r1, #0
 *     02118aa4  mov   r2, #0x18
 *     02118aa8  mov   r3, #4
 *     02118aac  str   ip, [sp]
 *     02118ab0  bl    #0x205525c          _ZN3G2x13SetBlendAlphaEPVttttt
 *     02118ab4  mov   ip, #0xa
 *     02118ab8  ldr   r0, [pc, #0x20]     ; =0x04001050
 *     02118abc  mov   r1, #0
 *     02118ac0  mov   r2, #0x18
 *     02118ac4  mov   r3, #4
 *     02118ac8  str   ip, [sp]
 *     02118acc  bl    #0x205525c
 *     02118ad0  add   sp, sp, #4
 *     02118ad4  ldm   sp!, {lr}
 *     02118ad8  bx    lr
 *
 * Nothing between the entry and the first bl writes r0, and 0x02115b0c's own
 * first instruction is `mov r8, r0`, so the callee receives the object.
 *
 * THE SOURCE PAIR, verbatim from the tree:
 *
 *     src/func_ov006_02115b0c.c:113   void func_ov006_02115b0c(char *c)
 *     src/func_ov006_02118a8c.cpp:8   void func_ov006_02115b0c(void);
 *     src/func_ov006_02118a8c.cpp:11  func_ov006_02115b0c();
 *
 * port/tools/aritycheck.py finds the row without being pointed at it:
 *   DROPS func_ov006_02115b0c -- declared 0 param(s) at
 *   src/func_ov006_02118a8c.cpp:8, DEFINED 1 param at
 *   src/func_ov006_02115b0c.c:67.
 *
 * WHY IT IS WORSE THAN A BAD READ, which is the reason it is fixed before the
 * first boot rather than after one: func_ov006_02115b0c is the board
 * teardown-and-rebuild. Its first act is seven inlined-destructor loops that
 * store two vtable words into each live slot and hand the slot to
 * Memory::operator_delete2. Off an uninitialised `this` those are WILD WRITES.
 * A fault is not guaranteed and whatever it lands on is corrupted silently,
 * which is exactly the shape lane MAR1 established for section 12's
 * displacement case.
 *
 * WHAT IS CHANGED FROM src/func_ov006_02118a8c.cpp, stated line by line so the
 * diff is checkable:
 *   1. the declaration of func_ov006_02115b0c gains its `char *` parameter,
 *      which is what src/func_ov006_02115b0c.c:113 already defines;
 *   2. func_ov006_02118a8c gains the `void *self` parameter the vtable slot
 *      is dispatched with, and passes it;
 *   3. nothing else. The two SetBlendAlpha calls, their five arguments and
 *      their two register addresses are byte-for-byte the src TU's.
 *
 * THE DECOMP-SIDE FIX IS ROUTED, NOT TAKEN. src/func_ov006_02118a8c.cpp should
 * declare and call func_ov006_02115b0c with the object; whether that still
 * builds byte-identically under mwccarm 1.2/sp2p3 is a byte-gated-tree
 * question and this tree is not the place to answer it. src/ is untouched;
 * the src TU is off port/slice_smb.txt so the link has one definition.
 */

extern "C" {

/* the real arity, from the definition */
void func_ov006_02115b0c(char *c);

/* five parameters, which is what the mangled name says and what the ROM
   passes: r0-r3 plus one stack word. The definition in
   src/_ZN3G2x13SetBlendAlphaEPVttttt.cpp carries a sixth its body never
   reads; dScMgCurling_c's slot 18 declares five as well. */
void _ZN3G2x13SetBlendAlphaEPVttttt(volatile void *p, unsigned short a,
                                    unsigned short b, unsigned short c,
                                    unsigned short d);

// PORT_HOST_ABI: src drops the self argument to func_ov006_02115b0c that the ROM rides through r0; MSVC would run board teardown off an uninitialised this, so the host copy passes self
void func_ov006_02118a8c(void *self)
{
    func_ov006_02115b0c((char *)self);
    _ZN3G2x13SetBlendAlphaEPVttttt((volatile void *)0x4000050, 0, 0x18, 4, 0xa);
    _ZN3G2x13SetBlendAlphaEPVttttt((volatile void *)0x4001050, 0, 0x18, 4, 0xa);
}

}  /* extern "C" */
