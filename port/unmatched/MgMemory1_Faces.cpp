// PORT_HOST_ABI. dScMgMemory_c's own faces and its one named trap.
// Run mg9, lane MMT.  Actor id 0x16a, scene 362, "Memory Match".
//
// ---- 1. THE TRAP IS THE CARD DRAW, AND IT IS THIS CLASS'S ONLY FLOOR ------
//
// func_ov006_020f3e68, ov006, size 0xa8, forty instructions plus two pool
// words.  It is the SIXTH call vtable slot 9 (Render) makes -- see
// src/func_ov006_020f5324.c, whose call list is
//
//     func_ov006_020c0aa8(c + 0x4660)
//     func_ov004_020b1bc8(c, 0xc, 0xc, 0)
//     func_ov004_020b6430()
//     func_ov006_020f38f0(c)
//     func_ov006_020f392c(c)
//     func_ov006_020f3e68(c)      <-- this one
//     func_ov006_020c1804(c + 0x4f38)
//
// and it is the ONLY code in dScMgMemory_c that puts a card pixel anywhere.
//
// THE SIBLING HAD THE IDENTICAL FLOOR AND IT COST A WHOLE LANE.
// port/mg_fanout_costs.txt section 15 is dScMgMemory2_c's func_ov006_020f5b98:
// same 0xa8, same sixth-call position under Render, same "the hand and the
// table are the same records and the same loop", and run mg7 lane MEMCARDS
// decompiled it as a NONMATCHING body after about thirty source shapes, ten
// compiler builds crossed with twelve optimisation settings and 30,452
// permuter iterations.  THIS LANE DOES NOT ATTEMPT THAT.  A seat with a trap
// here boots, ticks, deals and plays its state machine, and draws NO CARDS,
// and hal/scene_mg_memory1.cpp says so on every run rather than leaving the
// absence to be inferred from a green census.
//
// WHY IT IS A FLOOR AND NOT A LOOKUP MISS.  Three checks, all on this tree:
//   - config/arm9/overlays/ov006/symbols.txt names it and sizes it 0xa8.
//   - config/arm9/overlays/ov006/delinks.txt has a HOLE there: the block
//     before it ends at 0x020f3e68 and the next one starts at 0x020f3f10.
//   - No src file defines it in EITHER extension, in any overlay.  It is the
//     only one of the forty-seven functions in this class's code block
//     (0x020f3834..0x020f5564) without a src TU; the other forty-six all have
//     one and all forty-six are in port/slice_mmt.txt.
// The module-residency trap section 13 warns about does not fire here -- there
// is no src/func_ovNNN_020f3e68.* at any other overlay either -- but the check
// was run rather than assumed, because it is exactly what almost closed the
// sibling's floor twice.
//
// WHAT THE BODY DOES, read out of extracted/overlays/overlay_0006.bin at base
// 0x020bfec0 (alignment checked first on func_ov006_020f5324, whose
// disassembly reproduces its src call for call).  Recorded here so a later
// decomp lane starts from the ROM rather than from this paragraph's absence:
//
//     p = c; for (i = 0; i < 0xc; i++, p += 0x18)
//         if (p[+0x51ba])
//             Hud_RenderSprite(
//                 data_ov006_0214236c[ data_ov006_0213d168[ p[+0x51b8]*5
//                                                         + p[+0x51bd] ] ],
//                 *(int*)(p+0x51a8) >> 12, *(int*)(p+0x51ac) >> 12,
//                 -1, *(int*)(c+0x5314) != 2);
//
//   TWELVE records at +0x51a8, stride 0x18, which is this class's whole board.
//   data_ov006_0213d168 is SEVEN ROWS OF FIVE halfwords: row = card identity,
//   column = flip frame.  Row 0 is all zero and rows 1..6 read
//   {0, 1, 2, 2k+2, 2k+1}, so SIX card types over sprite indices 0..0x0e,
//   which is exactly the fifteen words __sinit_ov006_021311c8 copies from
//   data_ov006_02133810 into data_ov006_0214236c.  Six types dealt twice is
//   twelve cards, and src/func_ov006_020f4cd8.c deals 8, 10 or 12 of them on
//   the board byte at +0x533c.
//   The row count was READ, not divided out of a span: rows 0..6 are 70 bytes
//   and end at 0x0213d1ae, then six pad bytes, then a RELOCATED POINTER at
//   0x0213d1b4 -> 0x0213d090 (the RTTI record), and only then the vtable at
//   0x0213d1b8.  That is the same layout section 15 records for the sibling's
//   table, one row count smaller, and it is the trap that section names: a
//   span check over trailing padding over-reads the row count.
//   The highest index the game can produce is identity 6 * 5 + frame 4 = 34,
//   the last halfword of row 6, one short of the pad.
//   0x020af68c is Hud_RenderSprite in ov004's symbols.txt, and the fifth
//   argument is the main state index at +0x5314 as a flag: every state but 2
//   passes 1.
//
//   THE THIRD WITNESS, the cheapest of the three and the one to reach for
//   next time: ov006's relocs.txt has THIRTY-TWO arm_call relocations to
//   0x020af68c and exactly ONE of them is inside this class's code block --
//   0x020f3ee8, which is this function.  So the count of card draws in
//   dScMgMemory_c is one, measured off the relocation set rather than off a
//   reading of the class.
//
// THE TRAP COUNTS ITSELF, so "the cards did not draw" is a number rather than
// an absence: one entry per Render frame is the reading that says the seat
// reached the draw and the draw is the hole.
//
// ---- 2. THE FACES BELOW WERE NAMED BY A LINK ------------------------------
//
// Nothing in section 3 was predicted.  Each wave of this seat's link named its
// own unresolved externals, port/tools/facegen.py classified them (with an
// ABSOLUTE --out, which is finding 2 of port/mg_fanout_costs.txt section 10 --
// a relative one FALSE-FAILS on a correct file), and the rows below are what
// the classification and the hand rulings landed on.  The rulebook corollary
// section 4 states is the test applied to every struct-typed row:
//
//     A PAIR WHOSE CONSUMER SPELLS IT AS TWO INTS IS SAFE AS AN ALIAS.
//     A PAIR WHOSE CONSUMER NAMES A MEMBER-POINTER TYPE NEEDS A HOST COPY.

#include <cstdio>

extern "C" {

// ---- 1. the named trap -----------------------------------------------------

static unsigned g_mem1_carddraw_hits;

/* THE CARD DRAW.  Counted, reported once, and it draws nothing.  A plausible
   body here is exactly the guess port/tools/inferred_stub_guard exists to
   refuse, and a plausible CARD LAYOUT would be worse than no cards: it would
   put a picture on the screen that nobody could tell from the ROM's without a
   DS beside it. */
void func_ov006_020f3e68(void *self)
{
    static int said;
    (void)self;
    ++g_mem1_carddraw_hits;
    if (!said) {
        said = 1;
        std::fprintf(stderr, "  [scene] dScMgMemory_c CARD DRAW IS A FLOOR: "
                     "func_ov006_020f3e68 has no src TU and no delink block, "
                     "so no card is drawn on any frame. The deal, the board "
                     "and the per-card state machine all run; only the sprite "
                     "call is missing. See unmatched/MgMemory1_Faces.cpp "
                     "section 1.\n");
        std::fflush(stderr);
    }
}

unsigned port_mg_memory1_carddraw_hits(void) { return g_mem1_carddraw_hits; }

}  /* extern "C" */

// ---- 3. the aliases the link asked for -------------------------------------
//
// Filled in from this seat's own link waves; see section 2 for the rule each
// row was ruled by.
