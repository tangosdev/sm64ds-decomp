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
 */

#include <cstdio>

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
   before reading that zero as "never asked". */
static unsigned g_floor_020cf2fc, g_floor_020d01e0;

/* ROM 0x020cf2fc, 0x45c, UNDECOMPILED.  Arity from the call site. */
void func_ov006_020cf2fc(char *)
{
    if (!g_floor_020cf2fc)
        std::fprintf(stderr, "  [mg384/385] FLOOR func_ov006_020cf2fc (0x45c, "
                     "no src, no delinks block) wanted\n");
    ++g_floor_020cf2fc;
}

/* ROM 0x020d01e0, 0x800, UNDECOMPILED.  Arity from the call sites. */
void func_ov006_020d01e0(short *, short *, short *)
{
    if (!g_floor_020d01e0)
        std::fprintf(stderr, "  [mg384/385] FLOOR func_ov006_020d01e0 (0x800, "
                     "no src, no delinks block) wanted\n");
    ++g_floor_020d01e0;
}

/* THE HIT-TEST SLOT IS RETIRED, NOT SILENT.  Every accessor below still takes
   and fills the same arguments it did in run mg11, because two seat files and
   two censuses read them and neither was touched for this; what changed is that
   the 020d0c38 slot can no longer be anything but 0, since the body it counted
   is real code now.  A reader who sees 0 there must read it as RETIRED -- it is
   not "the stroke was never judged" any more, and the evidence to read instead
   is the trampoline-record readout both censuses print. */
unsigned port_mg_tti_hittest_calls(void) { return 0; }

void port_mg_tti_floor_counts(unsigned *hit, unsigned *f2fc, unsigned *f1e0)
{
    *hit  = 0;                  /* RETIRED, see above */
    *f2fc = g_floor_020cf2fc;
    *f1e0 = g_floor_020d01e0;
}

/* lane TTE's census reads its first three of four through this
   (unmatched/MgTrampolineTerror_Faces.cpp) */
void port_mg_shared_trap_counts(unsigned *f2fc, unsigned *f1e0, unsigned *fc38)
{
    if (f2fc) *f2fc = g_floor_020cf2fc;
    if (f1e0) *f1e0 = g_floor_020d01e0;
    if (fc38) *fc38 = 0;        /* RETIRED, see above */
}

}  /* extern "C" */
