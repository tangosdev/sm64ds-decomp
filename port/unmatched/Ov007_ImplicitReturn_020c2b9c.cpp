/* HOST COPY of src/func_ov007_020c2b9c.c -- THE TITLE'S IMPLICIT r0 RETURN
 * VALUE, with the one thing the ROM leaves implicit spelled out: the return.
 * ov007 0x020c2b9c, 0x5c bytes (92, 23 ARM instructions).
 *
 * ============================ WHAT THIS IS ================================
 *
 * A DISPLACEMENT, NOT AN INTERIM. src/func_ov007_020c2b9c.c is MATCHED and
 * stays matched, byte for byte and untouched by this lane; it is simply not in
 * the port's ov007 slice any more, and this body links in its place. That costs
 * the port one linked TU and it is the price of the seat, not a regression.
 *
 * THE FOURTH DISPLACEMENT IN THIS OVERLAY, AND THE FIRST ON THE RETURN SIDE.
 * The other three all lose a value on the way IN:
 *
 *   Ov007_RideThrough_020be980.cpp   a MIDDLE frame handed r1/r2, naming neither
 *   Ov007_ImplicitR0_020add3c.cpp    a frame that OWNS its argument and does not
 *                                    pass it
 *   Ov007_BdPending_020b6eb4.cpp     a guard that loads the word and lets the
 *                                    callee re-read it out of r0
 *
 * This one loses a value on the way OUT. Same family, opposite direction, and
 * the direction is why it gets its own derivation rather than the same fix
 * again: on the argument side a host copy has to decide WHICH of two frames
 * owns the value, and on the return side there is only one frame that can
 * possibly supply it.
 *
 * ============================ THE SEAM ====================================
 *
 * THREE matched TUs, and all three are right on ARM:
 *
 *   src/func_ov007_020c2b9c.c   void func_ov007_020c2b9c(int count, int *src)
 *                                    <- the DEFINITION returns nothing
 *   src/func_ov007_020c2acc.c   extern void *func_ov007_020c2b9c(int, int);
 *                                    struct O *o = (struct O *)func_...(count, x);
 *   src/func_ov007_020c2b38.c   extern void *func_ov007_020c2b9c(int, int);
 *                                    void *obj = func_...(count, arg3);
 *
 * On ARM there is nothing for the definition to emit. Its own call to
 * func_ov007_020c2c68 leaves the new object in r0, and NOTHING BETWEEN THAT
 * CALL AND THE RETURN WRITES r0 -- so the object is still sitting in r0 when
 * the callee falls out, and both callers simply take it from there. mwccarm is
 * correct to emit no move, and a matched body written `void` reproduces that
 * exactly. On MSVC a `void` function never sets EAX, so both callers read
 * whatever the last call left there.
 *
 * ============================ THE ROM ======================================
 *
 * Read off extracted/overlays/overlay_0007.bin at the config-aligned base
 * 0x020ad660 -- derived in this lane as the lowest ov007 symbol address in
 * config/arm9/overlays/ov007/symbols.txt, NOT taken from the dsd export, whose
 * overlay-table ramAddress is a different space. The alignment control was run
 * first and on this lane's own tree: tools/match.py --strict-relocs at 2004/b56
 * reports MATCH for func_ov007_020c2acc (0x6c), func_ov007_020c2b38 (0x64) and
 * func_ov007_020c2b9c (0x5c) out of that window, so the bytes disassembled
 * below are this function's and not a neighbour's tail.
 *
 * THE CALLEE, and the claim that matters is about EVERY path, not the happy one:
 *
 *   020C2B9C  push  {r4, r5, r6, lr}
 *   020C2BA0  mov   r4, #0
 *   020C2BA4  movs  r5, r1              <- src, and the flags it sets
 *   020C2BA8  mov   r6, r0              <- count
 *   020C2BAC  orrne r4, r4, #1          <- flag |= 1 when src != 0
 *   020C2BB0  mov   r1, r6
 *   020C2BB4  mov   r2, r4
 *   020C2BB8  mov   r0, #0
 *   020C2BBC  bl    #0x20c2c68          <- THE OBJECT ARRIVES IN r0
 *   020C2BC0  ands  r1, r4, #1
 *   020C2BC4  beq   #0x20c2bec          <- path A: no copy
 *   020C2BC8  cmp   r6, #0
 *   020C2BCC  mov   r3, #0
 *   020C2BD0  ble   #0x20c2bec          <- path B: count <= 0
 *   020C2BD4  ldr   r2, [r5, r3, lsl #2]
 *   020C2BD8  ldr   r1, [r0, #0x20]     <- READS r0, does not write it
 *   020C2BDC  str   r2, [r1, r3, lsl #2]
 *   020C2BE0  add   r3, r3, #1
 *   020C2BE4  cmp   r3, r6
 *   020C2BE8  blt   #0x20c2bd4          <- path C: the loop
 *   020C2BEC  strh  r6, [r0, #8]        <- READS r0, does not write it
 *   020C2BF0  pop   {r4, r5, r6, lr}
 *   020C2BF4  bx    lr                  <- THE ONLY EXIT
 *
 * Between the `bl` at 020C2BBC and the `bx lr` at 020C2BF4 there are eleven
 * instructions and NOT ONE OF THEM WRITES r0. The two reads at 020C2BD8 and
 * 020C2BEC are reads. The three paths (the `beq`, the `ble`, the loop) all
 * converge at 020C2BEC and there is a single `pop`/`bx lr`, so "r0 still holds
 * the object at the return" is a statement about the whole function and not
 * about one route through it. THAT is the ROM-side confirmation this seat rests
 * on; the dumpbin reading below is the host half of the same fact, and neither
 * alone would be enough.
 *
 * BOTH CALLERS TAKE IT STRAIGHT OUT OF r0, one instruction after the branch:
 *
 *   020C2AE0  bl  #0x20c2b9c     020C2B4C  bl  #0x20c2b9c
 *   020C2AE4  mov r5, r0         020C2B50  mov r5, r0
 *   ...                          ...
 *   020C2AF8  ldr r1, [r5, #0x24]  <- and this is where the port faults, via
 *                                     the host caller's `mov eax, [edi+24h]`
 *   020C2B2C  mov r0, r5         020C2B90  mov r0, r5
 *                                             <- and both then RETURN it, which
 *                                                is declared and emitted
 *
 * ============================ THE CALLER SWEEP =============================
 *
 * TWO call sites in the whole config, both in ov007, and BOTH expect the value.
 * Out of config/arm9/overlays/ov007/relocs.txt, re-derived here rather than
 * taken on a review's word:
 *
 *   from:0x020c2ae0 kind:arm_call to:0x020c2b9c module:overlay(7)
 *   from:0x020c2b4c kind:arm_call to:0x020c2b9c module:overlay(7)
 *
 * 0x020c2ae0 falls inside func_ov007_020c2acc's delink span (start:0x020c2acc
 * end:0x020c2b38) and 0x020c2b4c inside func_ov007_020c2b38's (start:0x020c2b38
 * end:0x020c2b9c). The two callers are the same routine twice over: 020c2acc
 * fills the object from two BYTE arrays (`ldrb` + `lsl #0xc`), 020c2b38 from two
 * INT arrays. Both do `mov r5, r0` after the branch, both index r5+0x24 and
 * r5+0x28, both return r5.
 *
 * A grep for the bare address over the whole of config/ finds NO row in any
 * other overlay's reloc table -- unlike the add3c seam, where ov002 and ov003
 * each carry an unrelated call written FROM an address that overlaps in the
 * shared scene slot. Worth stating as a negative rather than leaving unsaid,
 * because that trap has now cost two lanes a paragraph.
 *
 * SO ONE DISPLACEMENT ANSWERS BOTH CALLERS AND NEITHER CALLER IS TOUCHED. That
 * is not a convenience, it is the reason the callee is the right frame. Both
 * callers' matched sources ALREADY declare the return, so MSVC already emits
 * the `mov <reg>, eax` at both sites; the only missing half is the callee
 * setting EAX. Displacing a caller instead would fix one of two sites, would
 * have to invent the object rather than forward it (the value is the callee's
 * own local, allocated inside func_ov007_020c2c68 -- there is nothing in either
 * caller's frame to forward), and would leave the other site faulting.
 *
 * ============================ THE PORT'S HALF ==============================
 *
 * MEASURED ON THIS LANE'S OWN BINARY with `dumpbin /disasm`, not argued from
 * what MSVC ought to emit. THE CALLEE, in the pre-fix build:
 *
 *   func_ov007_020c2b9c.c.obj:  the object lives in EDI for the whole body,
 *   both return paths are `mov word ptr [edi+8], si` / pop / `ret`, and EAX IS
 *   NEVER WRITTEN after the call to func_ov007_020c2c68 returns into it -- the
 *   `movzx`/`mov` traffic in between clobbers it. Nothing declares a return, so
 *   nothing preserves one.
 *
 *   THE CALLER, func_ov007_020c2acc.c.obj: `call _func_ov007_020c2b9c` ->
 *   `mov edi, eax` -> `mov eax, [edi+24h]` -> `mov dword ptr [eax+esi*4], ecx`
 *   at +0x3f, which is the faulting offset in the fault report, with eax=0x62
 *   in the register dump. 0x62 is `count` -- the low half of the object's own
 *   +8 field spelled into a pointer -- so the crash address is not garbage in
 *   the usual sense, it is the last thing the callee's own arithmetic left in
 *   EAX read as if it were the object.
 *
 * The exact offsets belong to one binary; the map addresses and the git sha are
 * the currency. The measured pair is banked with this lane's evidence.
 *
 * ============================ WHY NOT FIX src/ =============================
 *
 * AND THIS IS RECORDED BECAUSE IT WAS MEASURED, NOT BECAUSE IT WAS RULED OUT.
 * Spelling the return in the DECOMP -- changing src/func_ov007_020c2b9c.c to
 * `void *` and adding `return r;` -- STILL MATCHES: 0 divergent words of 23 at
 * 2004/b56 with strict relocs, because r0 already holds the value and mwccarm
 * emits nothing extra to say so. That is the MP1 shape (run mg15 lane MP1
 * retired two host copies by fixing include/decl_common.h once the byte gate
 * proved the fix free), it would cost the port no linked TU, and it would make
 * the definition agree with the two callers that already declare `void *`.
 *
 * IT IS NOT TAKEN HERE. This lane's brief scopes the fix to a displacement and
 * leaves src/ untouched, and changing which of the decomp's matched bodies is
 * the decomp is a review's ruling, not a lane's -- the same precedent
 * Ov007_ImplicitR0_020add3c.cpp records for the opposite direction. The
 * measurement is in this lane's evidence (V1_src_return_variant.c and its match
 * log) so that the ruling can be made on a number instead of an argument. If it
 * goes that way this file is retired by deleting its CMake block and this file,
 * exactly as MP1 retired Comms_SlotIndex_*.cpp.
 *
 * ============================ THE SIGNATURE ================================
 *
 * `(int count, int *src)` is what the matched source declares and it is
 * unchanged here. The two callers spell the second parameter `int`, which is
 * the same 32-bit slot, and neither is touched. The ONLY edit to the body is
 * the return type and the `return r;`.
 */
extern "C" {

extern void *func_ov007_020c2c68(int a, int b, int c);

/* PORT_HOST_ABI: implicit r0 return value. The matched src declares this body
 * `void` and its two matched callers declare it `void *`, because on ARM the
 * object its own callee returned is still in r0 when this frame falls out. See
 * the header. */
void *func_ov007_020c2b9c(int count, int *src)
{
    int flag = 0;
    void *r;
    if (src != 0) flag |= 1;
    r = func_ov007_020c2c68(0, count, flag);
    if ((flag & 1) != 0) {
        int i;
        for (i = 0; i < count; i++) {
            ((int *)(*(int **)((char *)r + 0x20)))[i] = src[i];
        }
    }
    *(unsigned short *)((char *)r + 8) = (unsigned short)count;
    return r;
}

}
