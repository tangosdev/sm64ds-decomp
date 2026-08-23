// PORT_HOST_ABI. dScMgMemory_c's own faces.  Its one named trap is GONE.
// Run mg9, lane MMT; the trap retired by run mg10, lane F362.
// Actor id 0x16a, scene 362, "Memory Match".
//
// ---- 1. THE TRAP THAT USED TO BE HERE WAS THE CARD DRAW -------------------
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
// permuter iterations.  RUN mg9 LANE MMT DID NOT ATTEMPT THAT and trapped it
// instead; a seat with a trap here booted, ticked, dealt and played its state
// machine, and drew NO CARDS.
//
// RUN mg10 LANE F362 DECOMPILED IT.  src/func_ov006_020f3e68.c is the body and
// port/slice_mmt.txt carries it, so this file no longer defines the symbol at
// all.  It is NONMATCHING at 24 of 42 words and the file's own banner is the
// measurement: the ROM body is the sibling's 0x020f5b98 with FOUR constants
// changed (the state offset, the record count and the two pool words), the
// residue is the sibling's one-register rotation to the word, and the base
// permuter score is the same 1395.
//
// WHY IT WAS A FLOOR AND NOT A LOOKUP MISS.  Three checks, all re-run on this
// tree by lane F362 before the body was written:
//   - config/arm9/overlays/ov006/symbols.txt names it and sizes it 0xa8.
//   - config/arm9/overlays/ov006/delinks.txt has a HOLE there: the block
//     before it ends at 0x020f3e68 and the next one starts at 0x020f3f10.
//     THAT HOLE IS STILL THERE AND MUST STAY: a NONMATCHING TU gets no delink
//     block, so the ROM build keeps using the ROM's own bytes.
//   - No src file defined it in EITHER extension, in any overlay.  It was the
//     only one of the forty-seven functions in this class's code block
//     (0x020f3834..0x020f5564) without a src TU; all forty-seven have one now.
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
//   AND BOTH POOL WORDS HAVE ONE READER, which lane F362 measured and lane MMT
//   did not: ov006's relocs.txt has exactly ONE load to 0x0213d168 in the whole
//   overlay (from:0x020f3f08, this body's own pool) and exactly TWO to
//   0x0214236c (from:0x020f3f0c, the same pool, and from:0x02131468, the
//   constructor that fills it).  Nothing else in the overlay reads either
//   table, which is the same reading section 15 records for the sibling's pair.
//
// THE TRAP COUNTED ITSELF, so "the cards did not draw" was a number rather than
// an absence: one entry per Render frame was the reading that said the seat
// reached the draw and the draw was the hole.  hal/scene_mg_memory1.cpp reports
// the card records themselves and the sub-OAM census instead now, which is what
// the count was standing in for.
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

// ---- 3. the aliases the link asked for -------------------------------------
//
// WAVE 1 PRODUCED EXACTLY ONE UNRESOLVED EXTERNAL, on all three targets, line
// for line:
//
//     func_ov006_020f3834.cpp.obj : error LNK2019: unresolved external symbol
//     "void * data_ov006_0213d1b8" (?data_ov006_0213d1b8@@3PAXA)
//
// and that is the whole of this seat's wall bill.  The reason it is one row and
// not the sibling's forty-one is that this class's ov004 closure and its shared
// model sub-object are both already seated: run mg6 lane MEM paid the forty-one
// for the same sub-object at the same +0x4f38, and run mg5 lane BASESET paid
// the framework's.
//
// THE RULING.  src/func_ov006_020f3834.cpp (slot 16, the D2) declares
// `extern void* data_ov006_0213d1b8;` OUTSIDE an extern "C" block, so MSVC
// mangles it, while the ov006 mount defines the plain C name.  Its sibling
// src/func_ov006_020f3888.cpp (slot 17, the D0) declares the same table through
// include/decl_common.h at C linkage and resolves without help, which is why
// only one of the two destructors shows up in the link -- the same one-of-a-
// pair spelling defect port/mg_fanout_costs.txt section 6 catalogues.
//
// IT IS AN ALIAS AND NOT A HOST COPY, and the rulebook corollary section 4
// states is why.  The consumer is `*(void**)c = &data_ov006_0213d1b8;`: it
// takes the ADDRESS of a vtable, so a name that resolves to the same address is
// exactly right and no stride is involved.  The member-pointer half of the
// corollary cannot apply -- this symbol is a 36-word virtual table, not a
// {code, adjustment} pair, and the fill in hal/scene_mg_memory1.cpp is the only
// thing that ever indexes it.  hal/scene_mg_memory2.cpp's seat carries the
// identical two rows for 0x0213d4d4 and 0x0213e448.
//
// facegen was NOT the classifier here and this line says so rather than
// implying a tool blessed it: the failed link had already truncated
// walk_window.map, which is both facegen's and closure.py's universe (finding 3
// of section 10), and a one-row wall does not repay rebuilding that universe
// out of the object symbol tables.  The row was ruled by hand against the
// consumer, which is the standard the corollary sets.
#pragma comment(linker, "/alternatename:?data_ov006_0213d1b8@@3PAXA=_data_ov006_0213d1b8")

// ---- 4. the card draw is a real body now -----------------------------------
//
// Nothing is defined here for 0x020f3e68 any more.  src/func_ov006_020f3e68.c
// defines it and port/slice_mmt.txt compiles it, so a tree that loses that
// slice line fails to LINK rather than quietly drawing nothing.  The trap is
// DELETED rather than emptied: an emptied trap still satisfies the symbol and
// still draws nothing, and the link is the only instrument that can tell the
// difference.
//
// THIS FILE NOW CONTAINS NO CODE AT ALL, only the one alias row section 3
// rules on.  It stays because that row is real and because sections 1 and 2
// are the derivation a future reader needs; it is not a placeholder.
