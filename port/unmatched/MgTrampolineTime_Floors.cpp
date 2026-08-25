/* PORT_HOST_ABI.  dScMgTrampoline_c's ONE undecompiled body, trapped.
 * Run mg11, lane TTI.
 *
 * ---- WHAT IS MISSING AND HOW THAT WAS ESTABLISHED ------------------------
 *
 * func_ov006_020d0c38, ROM 0x020d0c38, size 0x3ac.
 *
 * config/arm9/overlays/ov006/symbols.txt carries the symbol, so the body
 * exists in the image and its extent is known.  What does NOT exist is a
 * decompilation: config/arm9/overlays/ov006/delinks.txt ends a .text block at
 * `end:0x020d0c38` and no block resumes over it, and no file in src/ defines
 * the symbol.  It is a hole in the decomp, not a hole in this seat.
 *
 * IT IS NOT ONE OF THIS CLASS'S vtable OR STATE BODIES.  port/tools/
 * vtablerows.py --vtable 0x0213fb34 --width 36 reports nosrc 0, and all five
 * chain links reach a matched src TU (port/slice_tti.txt section 7).  This body
 * is reached one level further down: vtable slot 23 (src/func_ov006_0212101c.c)
 * calls it as the stylus HIT TEST, with two Vec2s the caller builds from the
 * object's +0x5db0..+0x5db6 block:
 *
 *     if (func_ov006_020d0c38(&v1, &v2))
 *         func_02012718(0x1af, (+0x5db4 + +0x5db0) << 11);   the hit sound
 *     else
 *         func_02012790(0xe);                                the miss sound
 *
 * so the trap's return value decides WHICH SOUND a stylus release plays, and
 * nothing else.  It cannot fault a frame and it cannot stop the machine: slot
 * 23 clears +0x5dc4 and +0x5dc5 and returns 1 on both arms.
 *
 * ---- WHAT THE TRAP DOES AND WHAT IT DOES NOT --------------------------- --
 *
 * COUNT AND RETURN ONLY, which is the run law's shape for a floor.  It returns
 * 0 -- the MISS arm -- because that is the arm with no side effect beyond a
 * sound, and because a trap that guessed HIT would fabricate a game event.  The
 * count is printed by hal/scene_mg_trampoline.cpp's census so a run says how
 * many stylus releases went unjudged rather than leaving it inferred.
 *
 * A TRAP-SHAPED FLOOR HIDES ITS CALLEES FROM STATIC CLOSURE, which is the run
 * law's warning and is recorded here rather than discovered at a later merge:
 * whatever func_ov006_020d0c38 calls is NOT in port/slice_tti.txt, because
 * nothing in this tree can see it.  Retiring this floor will cost extra slice
 * lines beyond the one this file replaces.
 *
 * ---- TWO MORE, FOUND BY THE SECOND CLOSURE WAVE -------------------------
 *
 * The first wave pulled in the ELEMENT-OBJECT machinery behind the four
 * dMgTrmpln3DMario_c records (port/slice_tti.txt section 8), and two of the
 * bodies it reaches have no decompilation either.  Both were established the
 * same way as the first: config/arm9/overlays/ov006/symbols.txt carries the
 * symbol and its size, delinks.txt has no block over it, and no file in src/
 * defines it.
 *
 *   func_ov006_020cf2fc  0x45c.  Called once, from 0x020d0a80, which is inside
 *                        func_ov006_020d09e0 (0x020d09e0..0x020d0ac0) -- the
 *                        body vtable slot 9 calls through
 *                        func_ov006_020cd270's neighbour in Render's tail.
 *   func_ov006_020d01e0  0x800.  Called THREE times, and from THREE DIFFERENT
 *                        bodies, one call each:
 *                          0x020d0ba8  in func_ov006_020d0b78
 *                          0x020d0c08  in func_ov006_020d0bd8 (this slice
 *                                      compiles it)
 *                          0x020d0f7c  in func_ov006_020d0c38 -- which is
 *                                      ITSELF one of the three floors trapped
 *                                      in this file
 *
 * THE FIRST VERSION OF THOSE TWO LINES NAMED TWO BODIES THAT DO NOT HOLD THE
 * CALLS, and one of them does not exist at all.  There is no
 * func_ov006_020d0a3c in config/arm9/overlays/ov006/symbols.txt; 0x020d0a80 is
 * mid-body of func_ov006_020d09e0.  And 0x020d0ba8 is in func_ov006_020d0b78,
 * not func_ov006_020d0bd8 -- the two calls were assumed to share a body because
 * they are 0x60 bytes apart, and a body boundary sits between them.  Both were
 * re-derived by resolving each `from:` address against the symbol table's
 * start+size spans rather than by reading the nearest symbol below it.
 *
 * ONE OF THE THREE CALLERS IS A TRAP OF THIS FILE'S OWN, which strengthens the
 * measurement below rather than weakening it: the 0x020d0f7c call can never
 * fire while func_ov006_020d0c38 returns count-and-return, so the run's
 * `020d01e0 x1` is one call from a live body, not one of three.
 *
 * 0x800 is the largest body in this class's closure and neither trap can be
 * argued away as unreachable: func_ov006_020d0bd8 is on the path chain link 0
 * takes.  What they DO is not known from this tree, so the traps return zero
 * and count, and the census prints the counts.  A run with a nonzero count
 * here is a run in which something the ROM does did not happen -- which is the
 * honest reading and is the one the picture should be judged against.
 *
 * SAME WARNING, THREE TIMES OVER: whatever these two call is not in the slice.
 *
 * ---- RUN mg12 LANE TRM, PHASE 2: BOTH REMAINING FLOORS ARE DERIVED --------
 *
 * Neither is decompiled yet and neither is guessed at any more. Both were read
 * instruction by instruction out of extracted/overlays/overlay_0006.bin at base
 * 0x020bfec0; the full listings are banked as p2_disasm_020d01e0.txt and
 * p2_disasm_020cf2fc.txt. What follows is what they DO, so the next attempt
 * starts from a map instead of a hex dump.
 *
 * func_ov006_020d01e0 (0x800 = 503 instructions + 9 pool words) IS THE
 * INSTALLER, and it is what writes the render gate at +0x328 that
 * hal/trampoline_records.cpp reports on. Given the record and the two stroke
 * endpoints it: stores the raw endpoints at +0x50/+0x54; shifts both by -0x80
 * in x and negates y; swaps them if a.x > b.x; builds the midpoint into +0x08
 * and the along-stroke vector into +0x20; takes the cross product with world up
 * into +0x14 and flips it if its y is negative; writes the two draw vectors at
 * +0x38 and +0x44; measures the stroke with Vec3_Dist, keeps half of it at
 * +0x58 and a third of it as the row pitch; then fills a 4x4 grid of Fix12
 * vertices at +0x5c and copies it to +0x11c, with the row/column weights
 * running -0x180..+0x180 in steps of 0x100 and the along-stroke parameter
 * 0..0xfff in steps of 0x555. It then walks the five element records in
 * data_ov006_0214097c, asks each one three virtuals, and keeps the one whose
 * projections fall inside the stroke's own extents; on a hit it writes the
 * record's fields, plays 0x1b1 or 0x1ae through func_ov006_020e6db4 with a
 * blend of data_ov006_0213b2f8/2fc/300/304, and SETS +0x328 = 3. With no hit it
 * writes 0x258 at +0x31c and sets +0x328 = 1. Either way the gate goes nonzero,
 * which is what lets src/func_ov006_020d09e0.c and src/func_ov006_020d0ac0.c
 * look at the record at all.
 *
 * THE READING IS CORROBORATED, NOT JUST ASSERTED. nearmiss/db.jsonl already
 * carried an independent attempt at this body (source fanout-fable) and it
 * agrees on every adjudicated constant -- +0x329 = 0x1f, the three accept tests
 * (d3 < 0x100, d2 <= best, d2 > -0x24000), the 0x400000 branch, +0x328 = 3 on a
 * hit and 1 without one, and the 0x180/0x555 grid steps. Two separate readings,
 * no disagreement.
 *
 * IT IS NOT SEATED AND THE WALL IS CODEGEN, NOT SEMANTICS. Two candidates were
 * measured against the ROM with tools/match.py's own compile at 2004/b56, and
 * NEITHER MATCHES THE SIZE, so a plain word-diff count is not defined for
 * either: this lane's draft comes out 0x810, four words LONG, and the banked
 * fanout-fable row 0x81c, seven words long. Compared over the common 512-word
 * prefix with reloc slots wildcarded -- which is the only figure the sizes
 * allow, and the instrument is stated because the number means nothing without
 * it -- 457 words differ for the draft and 393 for the banked row.
 * nearmiss/db.jsonl scores that same banked row 277 by ITS own scorer, which is
 * a different instrument again and not comparable with either figure above.
 * (An earlier version of this paragraph said 459 and 395. Those came from a
 * side-by-side that padded the shorter side and counted the pad rows, so each
 * was overstated by exactly the length difference.) Neither is close enough to seat
 * honestly -- the residual is structural, not register naming, so no banner
 * could truthfully call it scheduling. The draft is banked beside the listing
 * as p2_draft_020d01e0.c rather than put in src/, because src/ is for matches
 * and for NONMATCHING seats whose divergences can be stated, and this one's
 * cannot be yet.
 *
 * A NEGATIVE LEVER WORTH KNOWING: reordering the twelve Vec3 locals into the
 * ROM's own stack order (v0, v1, bestA, bestB, tmpA, tmpB, sum, mid, dv, nrm,
 * up, crs) moved the count by two words. Declaration order does not decide
 * frame layout in this body the way pret's idiom predicts; the scalar block
 * below the vectors is 0x14 bytes wider than the ROM's and that is what shifts
 * every stack reference in the function.
 *
 * ONE OF ITS CALLS IS DEAD IN THE ROM. The fourth DotVec3 in the element loop
 * (0x020d0798, rec+0x20 against the second point) has its result read by
 * nothing. It is in the listing and any candidate has to emit it.
 *
 * func_ov006_020cf2fc (0x45c = 258 instructions + 21 pool words) IS THE MESH
 * DRAW for the grid the installer builds, and it is the reason the two loops in
 * src/func_ov006_020d09e0.c gate on +0x328. It loads the record's translation
 * from +0x08 and its scale from +0x2c into data_020a0e68, multiplies through
 * data_0209b3ec twice and pushes both matrices with func_020553a4, then writes
 * the geometry engine directly: 0x04000440 (matrix mode), 0x0400046c, 0x04000454,
 * 0x040004a8 and 0x040004ac (texture image and palette base, from
 * data_ov006_02140844 and data_ov006_02140814), 0x040004a4 (the polygon
 * attribute, built from the record's +0x31e and +0x329), 0x040004c0 (the two
 * halves of data_ov006_0212e060 and data_ov006_0212e068 indexed by +0x326),
 * 0x040004c4, and then a pair of double loops that each open a strip with
 * 0x04000500 = 2 and emit, per vertex, a normal word from
 * data_ov006_0212e0b0, a texture coordinate from the record's +0x2dc array
 * (negated and masked with 0xc0000000), and the position as two 0x0400048c
 * writes packing three (v << 8) >> 16 halfwords. Rows run 0..2 and columns
 * 0..3 over the 4x4 grid, twice: once from +0x5c and once from the +0x11c copy.
 *
 * THAT ONE ALSO NEEDS HOSTGEN ON THE PORT SIDE, and the pattern is already in
 * this tree: every one of those stores is a raw MMIO write, which is exactly
 * the class run mg11's R373 lane found latching silently in memory and fixed
 * CMakeLists-only through the FLW_HOSTGEN/TT_SHARED_HOSTGEN blocks. Seating it
 * without that would draw nothing and look like a decompilation bug.
 */


/* ---- THE MERGE MADE THIS FILE THE ONE DEFINER OF ALL THREE -----------------
 *
 * Lane TTE trapped the SAME THREE BODIES in
 * unmatched/MgTrampolineTerror_Faces.cpp, because 0x180 and 0x181 share this
 * machinery.  Different filenames, so git saw no add/add and the mg11 merge
 * found them at link (LNK2005 x3).  ONE definition each survives, here, and
 * BOTH lanes' censuses read it.
 *
 * THE TWO LANES AGREED ON EVERYTHING THAT IS A RULING.  Both established all
 * three the same way -- the symbol and size are in
 * config/arm9/overlays/ov006/symbols.txt, delinks.txt has no block over the
 * body, no file in src/ defines it -- and both recorded the same sizes
 * (0x45c, 0x800, 0x3ac) and the same verdict, UNDECOMPILED.  Nothing was
 * adjudicated twice with two answers.
 *
 * THE SIGNATURES BELOW ARE TTE'S, AND THAT IS THE ONE THING THAT CHANGED.
 * This lane spelled the first two `int f(void)` -- placeholders, because a
 * count-and-return trap does not read its arguments and the arity was never
 * derived.  TTE derived all three from the ROM CALL SITES that name them
 * (src/func_ov006_0212101c.c and src/func_ov006_02122f24.c for 020d0c38,
 * which agree with each other), so its spellings are evidenced where this
 * lane's were merely sufficient.  On a cdecl host the two behave identically
 * -- the caller cleans the stack and the callee ignores what it was passed --
 * so this is not a behaviour change; it is a record that stops claiming an
 * arity the ROM contradicts.  The return of 020d0c38 stays int and stays 0,
 * the MISS arm, for the reason the header gives.
 *
 * ONE COUNTER SET, TWO ACCESSORS.  Both lanes' entry points survive with their
 * exact names: port_mg_tti_hittest_calls and port_mg_tti_floor_counts are read
 * by hal/scene_mg_trampoline.cpp, and port_mg_shared_trap_counts is what
 * unmatched/MgTrampolineTerror_Faces.cpp's port_mg_tte_trap_counts now reads
 * for its first three of four.  Neither seat file was touched.  Separate
 * counter sets would have been equivalent and are not worth the duplication:
 * one scene runs per process, so scene 384 reads its accessor and scene 385
 * reads its own, and neither can see the other's ticks.
 *
 * TTE's FOURTH trap, func_ov006_020cfc74, is NOT here -- it is behind one of
 * that class's 3D-Mario states and only 0x181 reaches it, so it stays in
 * TTE's file with its own counter.
 */

extern "C" {

/* g_hittest_020d0c38 is GONE with its trap: run mg12 lane TRM decompiled
   func_ov006_020d0c38 and src/func_ov006_020d0c38.c is a slice line in BOTH
   slice_tti.txt and slice_tte.txt now, so a second definition here would be
   the LNK2005 the mg11 merge already paid for once. The accessors keep their
   exact shapes and report the hit-test slot as 0 -- see the note above them
   before reading that zero as "never asked".

   g_floor_020d01e0 IS GONE THE SAME WAY, and it is the bigger of the two: run
   mg12 lane INST decompiled func_ov006_020d01e0, the 0x800 INSTALLER, and
   src/func_ov006_020d01e0.c is a slice line in both slices now. It is an honest
   NONMATCHING seat (49 of 503 code words, frame exact, every call present with
   the ROM's destination) and its divergences are stated in its own banner.
   THE CONSEQUENCE FOR EVERY READER OF THIS FILE: the render gate at +0x328 is
   written inside that body, so "gated == 0 is the expected shape of this build"
   -- which the record readout and both censuses said, correctly, while the body
   was trapped -- is NO LONGER TRUE. A zero gate after a stroke is now a
   finding, not the floor.

   g_floor_020cf2fc IS GONE TOO, AND WITH IT THIS FILE'S LAST TRAP. Run mg13
   lane TRAMP decompiled func_ov006_020cf2fc, THE MESH DRAW, and
   src/func_ov006_020cf2fc.c is a line in both trampoline slices now -- as a
   TT_SHARED_HOSTGEN symbol, because all thirty-two of its geometry-engine
   stores are raw MMIO and the src/ copy would latch every one of them into
   mapped memory. It is an honest NONMATCHING seat (37 divergences against 258
   ROM code words; the 0x7c frame and all six call offsets are the ROM's) and
   its banner states every divergence.

   WHAT THAT CHANGES FOR A READER OF THE CENSUS: with the installer AND the mesh
   draw both real, a stroke that reaches the gate now has a body to draw it.
   "The trampoline is built but nothing appears" is no longer explainable by a
   floor in this file -- there are none left in it. */

/* ALL THREE SLOTS ARE RETIRED NOW, NOT SILENT.  Every accessor below still takes and
   fills the same arguments it did in run mg11, because two seat files and two
   censuses read them and neither was touched for this; what changed is that the
   020d0c38 slot and the 020d01e0 slot can no longer be anything but 0, since
   both bodies are real code now.  A reader who sees 0 in either must read it as
   RETIRED -- it is not "the stroke was never judged" or "the installer was
   never reached" any more, and the evidence to read instead is the
   trampoline-record readout both censuses print, whose RENDER GATE is now a
   live measurement rather than a known floor. */
unsigned port_mg_tti_hittest_calls(void) { return 0; }

void port_mg_tti_floor_counts(unsigned *hit, unsigned *f2fc, unsigned *f1e0)
{
    *hit  = 0;                  /* RETIRED, see above */
    *f2fc = 0;                  /* RETIRED, see above */
    *f1e0 = 0;                  /* RETIRED, see above */
}

/* lane TTE's census reads its first three of four through this
   (unmatched/MgTrampolineTerror_Faces.cpp) */
void port_mg_shared_trap_counts(unsigned *f2fc, unsigned *f1e0, unsigned *fc38)
{
    if (f2fc) *f2fc = 0;        /* RETIRED, see above */
    if (f1e0) *f1e0 = 0;        /* RETIRED, see above */
    if (fc38) *fc38 = 0;        /* RETIRED, see above */
}

}  /* extern "C" */
