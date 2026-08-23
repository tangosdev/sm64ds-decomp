/* ONE NAMED TRAP for dScMgCard_c, actor id 0x17b (scene 379), "Picture Poker".
 * Run mg11, lane PKR.
 *
 * THE PRECEDENT IS func_ov006_020e1854, dScMgCurling_c's twenty-fifth state:
 * an address the ROM reaches, no delink block, no src TU in either extension,
 * and therefore no body this tree may write.  What goes in its place is a trap
 * that SAYS WHICH ADDRESS WAS ENTERED and does nothing else.  A plausible body
 * would be worse than a missing one, because a plausible body cannot be told
 * apart from a real one by anything downstream, and
 * port/tools/inferred_stub_guard exists to refuse exactly that.
 *
 * ---- THE HOLE IS REAL, AND IT IS NOT A NAME-SHAPED LOOKUP MISS ------------
 *
 * config/arm9/overlays/ov006/symbols.txt DOES name the address:
 *
 *     func_ov006_020da174 kind:function(arm,size=0x2ac) addr:0x020da174
 *
 * and config/arm9/overlays/ov006/delinks.txt runs
 *
 *     src/func_ov006_020da154.c   .text start:0x020da154 end:0x020da174
 *
 * and then resumes past this body.  Nothing covers the 0x2ac bytes between.
 * There is no src/func_ov006_020da174.c and no .cpp.  That is the case
 * port/tools/stategen.py's hole message was corrected to distinguish: A NAMED
 * SYMBOL IS NOT A DECOMPILED BODY.
 *
 * THIS IS WHY port/mg_fanout_costs.txt SECTION 3's nosrc COLUMN READS 0 FOR
 * 0x17b AND IS STILL RIGHT.  That column counts VTABLE SLOTS with no src, and
 * this body is not a vtable slot -- it is called from inside the class's own
 * state machine.  Run mg9 lane BOX hit the same shape (func_ov006_0211bc8c was
 * a STATE and not a slot) and run mg5 lane SMB before it (func_ov006_02114800).
 * A fan-out lane must not read a zero in that column as "no floors".
 *
 * ---- THE SIGNATURE IS READ OUT OF THE CALLER, NOT CHOSEN ------------------
 *
 * src/func_ov006_020dac34.c:16 declares it
 *
 *     extern int func_ov006_020da174(void *p);
 *
 * and calls it twice, at state 9, on c + 0x51a8 and c + 0x5298 -- the two
 * five-card hands.  This file repeats that declaration rather than inventing
 * one.  The ROM agrees: `push {r4..fp,lr}` then `mov r6, r0`, so r0 is the
 * only argument, and both of its exits set r0.
 *
 * ---- WHAT THE ROM SAYS THE BODY DOES, AND WHY THE TRAP RETURNS 0 ----------
 *
 * Disassembled from extracted/overlays/overlay_0006.bin at base 0x020bfec0 --
 * the shipped image, deliberately not a dsd export, whose relocated words would
 * make literals read as live pointers.  0x2ac = 171 words; ONE literal-pool
 * word, at 0x020da41c, so 170 instructions + 1 pool = 171 = size/4.
 *
 * AN EARLIER VERSION OF THIS LINE READ "0 literal-pool words (every constant is
 * an immediate), so 171 instructions + 0 pool = 171", AND IT WAS WRONG.  Review
 * found it.  The pool word is 0x020da41c and it holds 0x0213bcd8, which
 * config/arm9/overlays/ov006/relocs.txt confirms with
 * `from:0x020da41c kind:load to:0x0213bcd8` -- a relocation an immediate can
 * never carry.  0x0213bcd8 is the six-word run {0, 1, 2, 3, 4, 0}, the IDENTITY
 * permutation this body seeds its ordering from before it sorts, which is why
 * the wrong number was also the more interesting one to lose: the one pool word
 * in the body names the thing the body is for.  The arithmetic is mandatory and
 * it now comes from a tool rather than from a reading -- two independent splits
 * (pc-relative load targets, and load relocations sited inside the span) that
 * agree word for word.
 *
 *   0x020da180  a six-entry short HISTOGRAM is zeroed on the stack at sp+8
 *   0x020da1b4  five iterations of `ldrb r1,[r7,#0x2a]` (the card FACE) and
 *               `ldrsh/strh` at sp+8 + face*2 -- the histogram is by face
 *   0x020da1e4  a scan of the six counts picking the LARGEST and the SECOND
 *               largest, with their face indices kept in ip and lr
 *   ...         the five cards are ordered into a permutation written to
 *               sp+0x14 as five words
 *   0x020da3c4  `ldr r2,[r5,r7,lsl #2] / mov r1,r7 / mla r0,r2,r4,r6 / bl
 *               0x020d99ec` -- five calls to func_ov006_020d99ec(card, i),
 *               which is the ONLY thing that writes a card's slide target
 *               (+0x1c = (i*0x28+0x2c) << 12) and puts it into state 7
 *   0x020da3e8  the return: 1 if any perm[i] != i (a card actually moved),
 *               0 otherwise
 *
 * So it is the HAND SORTER: it groups matching faces together and slides the
 * five cards into their sorted positions.  The caller's use of the answer is
 * `if (a != 0 || b != 0) Sound::PlayBank2_2D(0x152)` -- the rearrange sound.
 *
 * RETURNING 0 IS THE CONSERVATIVE ANSWER AND IT IS A CHOICE THIS FILE OWNS.
 * The ROM's return is data-dependent, so no constant is "the measurement" the
 * way run mg5 lane SMB's single-exit `mov r0,#1` was.  0 is the value that
 * means "no card moved", which is the only reading consistent with a trap that
 * moved none.
 *
 * ---- THE TRAP IS ON THE CRITICAL PATH AND THIS FILE SAYS SO OUT LOUD ------
 *
 * A trap-shaped floor HIDES its callees from static closure, and here it also
 * hides the machine's own progress.  func_ov006_020d99ec is reached from
 * NOWHERE ELSE in the image -- a grep of src/ finds exactly one file naming it,
 * its own -- and it is the SOLE WRITER OF STATE 7, which is the half that
 * actually carries the conclusion and is measured rather than assumed: a sweep
 * of all thirty-six of this class's src bodies for a write of 7 to a card's
 * +0x2b returns exactly one hit, src/func_ov006_020d99ec.c:4.  So with this
 * trap standing:
 *
 *   - no card is given a slide target and none enters state 7;
 *   - state 8 has TWO writers, one per element class, and BOTH are inside a
 *     `state == 7` arm: src/func_ov006_020d9c5c.cpp:67 (dMgCardObj_c's tick)
 *     and src/func_ov006_020d978c.cpp:53 (dMgDilarCardObj_c's tick).  So no
 *     card reaches state 8 -- not because one promoter is missing, but because
 *     the ONE feeder of state 7 is behind this trap;
 *   - state 10 of the machine is
 *         if (func_ov006_020da860(c+0x51a8, 8) && func_ov006_020da860(c+0x5298, 8))
 *     -- ALL FIVE cards of BOTH hands at state 8 -- so the machine reaches
 *     state 10 and stays there.
 *
 * That is a stall, not a crash, and it is the honest cost of the seat.
 * hal/scene_mg_card.cpp prints the entry count and the state index on every
 * run so the stall is a measurement and not a surprise.  Retiring it is a
 * DECOMP, not a port fix: a delink block and a matched src TU for 0x020da174.
 *
 * IT IS ALSO WHY src/func_ov006_020d99ec.c IS IN port/slice_pkr.txt AND WILL
 * NOT LINK.  /OPT:REF drops a body nothing references, and the only reference
 * is the one this trap replaced.  Run mg9 lane S364's compiled-vs-linked lesson
 * applies: being in a slice means COMPILED; the linker decides LINKED.  The
 * line stays, because the day the floor is decompiled the reference comes back
 * and nothing else has to change.
 */

#include <cstdio>

static unsigned g_card_floor_hits;

extern "C" int func_ov006_020da174(void *p)
{
    if (++g_card_floor_hits <= 4)
        std::fprintf(stderr, "  [mg-card] FLOOR func_ov006_020da174(%p): no "
                     "src TU and no delink block; the hand is not sorted and "
                     "no card is given a slide target (entry %u)\n",
                     p, g_card_floor_hits);
    return 0;
}

extern "C" unsigned port_mg_card_floor_hits(void)
{
    return g_card_floor_hits;
}
