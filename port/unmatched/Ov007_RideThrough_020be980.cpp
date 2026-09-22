/* RETIRED HOST COPY of src/func_ov007_020be980.c -- SCENE 1'S ARM REGISTER
 * RIDE-THROUGH, with the two ridden arguments spelled out.
 * ov007 0x020be980, 0x2c bytes (44, 11 ARM instructions).
 *
 * ============================ WHAT THIS IS ================================
 *
 * A DISPLACEMENT, NOT AN INTERIM. src/func_ov007_020be980.c is MATCHED and
 * stays matched; it is simply not in the port's ov007 slice any more, and this
 * body links in its place. That costs the port one linked TU and it is the
 * price of the seat, not a regression. The two other host bodies in this
 * overlay's story fill holes in the decomp; this one displaces a body the
 * decomp already has, which is a heavier thing to do and is why the shape was
 * ruled before it was taken (port/ov007_seat.txt section 5b, by the PC2
 * review) rather than decided by the lane that took it.
 *
 * ============================ THE SEAM ====================================
 *
 * Three matched TUs disagree about this function's arity, on purpose, because
 * the disagreement is what makes each of them reproduce the ROM:
 *
 *   src/func_ov007_020b0a20.c   extern void func_ov007_020be980(Sub100*,int,int)
 *   src/func_ov007_020be980.c   void func_ov007_020be980(int r0)
 *                               extern int func_ov007_020bc3dc(int)
 *   src/func_ov007_020bc3dc.c   void func_ov007_020bc3dc(void *c, int i, int a2)
 *
 * On ARM that is correct at every frame. The caller sets r1 and r2 immediately
 * before the branch, the middle body never writes either before its own call,
 * and the callee reads both. The values RIDE THROUGH a frame that does not
 * know they exist. On MSVC every argument is a stack slot: the middle frame
 * pushes one, the callee reads three, and the two it did not get handed are
 * the middle frame's own saved registers. The fault that produced is derived
 * instruction by instruction in port/ov007_seat.txt section 5b, arithmetic
 * included.
 *
 * WIDENING THE SRC DECLARATION IS NOT AVAILABLE. That one-parameter shape is
 * what makes the middle TU emit the ROM's four instructions and nothing else.
 * Change it and the byte gate answers. The disagreement is right on ARM and
 * wrong only on a stack ABI, so the fix belongs on this side of the seam --
 * the same reason port/CMakeLists.txt already answers the spawn factory, gate
 * 14's three and Sound's five this way, and the reason a host-build #ifdef in
 * src/ was considered and refused: the decomp's sources answer to the byte
 * gate and to nothing else.
 *
 * WHY THIS FRAME AND NOT ANOTHER, since all three are matched and displacing
 * any one of them would compile. Of the three this is the only frame where
 * information is DESTROYED -- the values exist in the caller and are wanted by
 * the callee and this is where they stop being passed. func_ov007_020bc3dc
 * genuinely needs both and they differ per call site (i is 1 from
 * func_ov007_020b0a20 and 0 from func_ov007_020b2614), so no constant a host
 * body could supply would be right everywhere. func_ov007_020b0a20 already
 * pushes three and is doing nothing wrong.
 *
 * ============================ THE SIGNATURE ===============================
 *
 * (void *, int, int) is not chosen, it is READ OFF THE CALL SITES.
 * config/arm9/overlays/ov007/relocs.txt has exactly four arm_call rows landing
 * on 0x020be980 and no other module has one (the single `from:0x020be980` row
 * in ov000's table is that overlay's own unrelated load at an address that
 * overlaps in memory, not a call here). Re-derived in this lane, not taken on
 * the review's word:
 *
 *   0x020b0a90, 0x020b0b74   src/func_ov007_020b0a20.c
 *                            func_ov007_020be980(scene->unk100, 1, 0)
 *   0x020b26d0, 0x020b26e8   src/func_ov007_020b2614.c
 *                            func_ov007_020be980(p_100, 0, b)
 *                            func_ov007_020be980(p_104, 0, b)
 *
 * All four sites pass three arguments and both calling TUs are matched, so the
 * arity is settled by the ROM and not by this file. Forwarding all three is
 * therefore right at every site, including the three that scene 1 is not known
 * to reach -- those are the same defect waiting, not evidence of three more
 * faults.
 *
 * ============================ PROVENANCE ==================================
 *
 * The body below is the ROM's, read off extracted/overlays/overlay_0007.bin
 * with capstone at the config-aligned base 0x020ad660 (the minimum section
 * start in config/arm9/overlays/ov007/delinks.txt), NOT the dsd export, which
 * is short for this overlay. Its delink block runs start:0x020be980
 * end:0x020be9ac and symbols.txt gives size=0x2c, so the eleven instructions
 * are the whole function:
 *
 *   push {r4, lr} / mov r4, r0 / ldr r0, [r4, #4] / bl func_ov007_020bc3dc
 *   ldr r0, [r4, #4] / mov r1, #1 / str r1, [r0, #0x94]
 *   ldr r0, [r4, #4] / str r1, [r0, #0x98] / pop {r4, lr} / bx lr
 *
 * ONE CORRECTION TO THE PROSE THIS LANE INHERITED, since it is quoted in three
 * places: section 5b says r1 and r2 are "untouched across the whole body".
 * r2 is. r1 is NOT -- `mov r1, #1` at 0x020be994 writes it. That write lands
 * AFTER the branch, which is why the ride-through works and why nothing in the
 * reading changes, but the stronger sentence is not what the instructions say
 * and a later reader should not have to re-check it. THE SUFFICIENT CONDITION,
 * in the review's sharper form: r1 and r2 are LIVE AT THE BRANCH. The callee
 * reads the register file at entry, so the only instant that matters is the
 * `bl`, not any span around it.
 *
 * The behaviour below is the matched source's, unchanged: the callee runs on
 * the +4 pointer, then the two flag words at +0x94 and +0x98 of that same
 * pointer are set to 1, each after its own reload, exactly as the ROM reloads
 * them.
 */
/* ======================== RETIRED, AND WHY =================================
 *
 * run link100 wave 15, lane SEAT15E (LINK15 BATCH 4). THIS FILE CARRIES NO
 * BODY ANY MORE. The matched TU is back in port/slice_ov007.txt's own build --
 * the `continue()` that cut it out of the loop in port/CMakeLists.txt is gone
 * and so is the branch that appended this file instead -- because the defect
 * the header below derives is not in the source any anymore.
 *
 * WHAT WAS RE-READ, against src/ at 8ddff3187, before the cut was removed. The
 * header says the seam is three matched TUs disagreeing about this function's
 * arity, and quotes the middle one as `void func_ov007_020be980(int r0)` with
 * `extern int func_ov007_020bc3dc(int)` beside it. It is now
 *
 *     extern void func_ov007_020bc3dc(void* c, int i, int a2);
 *     void func_ov007_020be980(char *r0, int i, int a2){
 *       func_ov007_020bc3dc(*(void**)(r0+4), i, a2);
 *       *(int*)(*(int*)(r0+4)+0x94)=1;
 *       *(int*)(*(int*)(r0+4)+0x98)=1;
 *     }
 *
 * -- three parameters declared and three passed, and src/func_ov007_020bc3dc.c
 * defines the callee `void func_ov007_020bc3dc(void* c, int i, int a2)`. There
 * is no frame left that fails to name a value its callee reads, so there is
 * nothing for a stack ABI to lose. The statement list this file used to carry
 * was the same source's, line for line; the host copy was spelling out an
 * argument the source now spells itself.
 *
 * The four call sites the header derives are unchanged and still settle the
 * arity at three (config/arm9/overlays/ov007/relocs.txt), so the source's
 * present spelling is the ROM's and not a convenience.
 *
 * THE HEADER ABOVE IS KEPT ON PURPOSE. It carries the capstone reading at the
 * config-aligned base 0x020ad660, the call-site sweep, and the correction to
 * the prose about r1 being "untouched across the whole body" (it is not; the
 * sufficient condition is that r1 and r2 are LIVE AT THE BRANCH). Those are
 * readings of the cartridge and stay true whichever side of the seam runs.
 */
