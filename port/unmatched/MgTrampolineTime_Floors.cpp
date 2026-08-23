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
 */

#include <cstdio>

extern "C" {

static unsigned g_tti_hittest_calls;

/* ROM 0x020d0c38, 0x3ac, UNDECOMPILED.  Two Vec2s in, a hit/miss predicate
   out.  Returns the MISS arm; see the header. */
int func_ov006_020d0c38(void * /*a*/, void * /*b*/)
{
    ++g_tti_hittest_calls;
    return 0;
}

unsigned port_mg_tti_hittest_calls(void) { return g_tti_hittest_calls; }

}  /* extern "C" */
