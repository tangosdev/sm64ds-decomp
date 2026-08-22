// NONMATCHING: register colouring + schedule only (div=24 of 42 words). Logic
// verified correct vs ROM; not byte-matchable from C at mwccarm 2004/b56 (see
// notes/matching-style.md). Counts as decompiled, not matched.
//
// THIS IS THE SIBLING'S BODY WITH FOUR CONSTANTS CHANGED, AND THE ROM SAYS SO.
// src/func_ov006_020f5b98.c is dScMgMemory2_c's card draw. Disassembled side by
// side out of extracted/overlays/overlay_0006.bin at base 0x020bfec0, the two
// ROM bodies are the SAME FORTY INSTRUCTIONS in the same order, and the only
// words that differ anywhere in either are
//
//     ldr r1,[r5,#0x314]  here   vs  #0x3d4    the state offset
//     cmp r4,#0xc         here   vs  #0x14     twelve records vs twenty
//     pool 0x0213d168     here   vs 0x0213d45c the identity/frame table
//     pool 0x0214236c     here   vs 0x02142490 the OAM pointer array
//
// The two branch encodings are identical words (0a000012 and baffffe4), so even
// the block layout is shared. That is why this file is the sibling's source
// with the four constants adjusted rather than an independent shape.
//
// AND THE RESIDUE IS THE SIBLING'S TOO, TO THE WORD. tools/match.py: 24 of 42
// words differ on all four builds that produce a 0xa8 body at all (1.2/base,
// 1.2/sp2, 1.2/sp2p3 and 2004/b56; the other twenty-one installed builds come
// out a different size). The body compiles to FORTY instructions with an
// identical mnemonic multiset to the ROM's, and the only opcode-shape
// difference anywhere in it is the bl's target, which is the wildcarded
// relocation. Every differing word differs by a register number or a schedule
// slot. The permuter's base score is 1395 here, the same figure section 15
// records for the sibling, and 12,919 iterations over two seeds (9,781 from
// seed 7 and 3,138 from seed 424242) never left the 1390 plateau. That is on
// top of the 30,452 iterations run mg7 spent on the byte-identical residue at
// the sibling's address, and the two searches agree.
//
// AND THE PERMUTER'S OWN BEST OUTPUT IS SEMANTICALLY WRONG, which is worth one
// line because a future lane will find these files. Its 1390 candidate reaches
// that score by reusing `i` -- the live loop counter -- as the scratch for the
// table index, so the loop would run once and exit. A permuter score below zero
// is a lead, never a body: nothing but a score of 0, re-verified through
// tools/match.py, may be adopted.
//
// THE RESIDUE IS ONE REGISTER ROTATION, exactly as section 15 describes it: the
// ROM colours (i, base, -1, table) into r4..r7 with the constant zero in sb,
// and every candidate colours (base, -1, table, 1) into r4..r7 with the loop
// counter pushed to the tail in sb. Nothing is inserted or deleted; the same
// eight long-lived values are held in the same eight callee-saved registers,
// rotated by one.
//
// WHAT THIS LANE TRIED, ON TOP OF WHAT SECTION 15 ALREADY PAID FOR: FORTY
// source shapes -- all six declaration orders, five loop forms (for, while,
// do/while, comma-increment, and the initialisation split across the header),
// five counter types, six spellings of the flag, two struct-typed record walks
// and an unsigned-char one, the state hoisted to a local and to a pointer,
// extra locals for the index and for the coordinates, the record address
// recomputed from the counter instead of walked, a `continue` early-out, a
// local loop bound, the mla operands both ways round, and the `register`
// keyword on each local in turn. Every shape that produces a 0xa8 body at all
// answers 24 on all four capable builds; the struct walks answer 36. The
// shipped shape was then crossed with TWENTY-FIVE optimisation settings on
// each of the four builds -- that cross was run on the shipped shape only, not
// on all forty -- and 24 does not move: every setting that changes the number
// changes it by changing the SIZE.
//
// FIVE SHAPES REACH 23 AND NONE IS SHIPPED, which is the sibling's finding
// reproduced independently at this address. Four of them -- the bare comparison
// as the fifth argument, the same comparison assigned to the flag, an if/else,
// and a ternary -- compile the flag as a movne/moveq PAIR, where the ROM emits
// an unconditional mov of a hoisted zero followed by a movne of a hoisted one,
// which is the if-statement below. The fifth, the inverted form
// `flag = 1; if (*(int *)(c + 0x5314) == 2) flag = 0;`, DOES keep the
// unconditional-then-conditional construct, and is rejected for a different
// reason: it emits moveq where the ROM emits movne, so it buys its word by
// flipping a condition code. That is an opcode-shape difference, not a
// colouring one, and it would falsify the sentence at the top of this banner.
// The 23 is recorded in nearmiss/db.jsonl as the seed for the next attempt.
//
// THE RELOCATION DESTINATIONS ARE VERIFIED, AND match.py DID NOT DO IT.
// tools/match.py runs its --strict-relocs destination check only inside
// `if ok`, so on a NONMATCHING body it never fires: a wrong callee would be
// reported as "reloc (wildcard)" exactly like the right one, and the file
// would read as verified having had nothing verified. Run directly instead --
// tools/reloc_audit.check_destinations over this file's own object -- all
// three slots answer OK: +0x80 -> Hud_RenderSprite 0x020af68c (ov004), +0xa0
// -> data_ov006_0213d168, +0xa4 -> data_ov006_0214236c, each against
// config/arm9/overlays/ov006/relocs.txt. The check is not vacuous: a control
// with the bl spelled func_ov004_020b1bc8 answers WRONG-DEST at +0x80 and OK
// at the other two.
//
// @symbol func_ov006_020f3e68
// recovered name: dScMgMemory_c_DrawCards
/* dScMgMemory_c's card draw: the sixth call vtable slot 9 (Render) makes, from
   src/func_ov006_020f5324.c.

   TWELVE records at +0x51a8, stride 0x18 -- this class's whole board.
   src/func_ov006_020f4cd8.c fills 8, 10 or 12 of them at the deal depending on
   the board byte +0x533c, each with a card identity in 1..4, 1..5 or 1..6
   dealt twice over; src/func_ov006_020f43c4.c walks them from x = 128.0,
   y = -128.0 to their slot, and src/func_ov006_020f3f10.c's per-card state
   machine drives the flip frame.

   Per record:
     +0x12  nonzero when the card is in play (this loop's whole gate)
     +0x00  screen x, 20.12, shifted down by 12 for the sprite
     +0x04  screen y, 20.12
     +0x10  card identity, the ROW of the halfword table at
            data_ov006_0213d168
     +0x15  flip frame 0..4, its COLUMN

   THE TABLE IS SEVEN ROWS OF FIVE, and the row count was READ rather than
   divided out of the 0x50 to the vtable. Rows 0..6 are 70 bytes and end at
   0x0213d1ac; then six pad bytes, then a RELOCATED pointer at 0x0213d1b4 ->
   0x0213d090 (the RTTI record), and only then the vtable at 0x0213d1b8. Row 0
   is all zero and rows 1..6 read {0, 1, 2, 2k+2, 2k+1}, so the highest
   halfword in it is 0x0e = 14 -- which is what makes the FIFTEEN words
   __sinit_ov006_021311c8 copies out of data_ov006_02133810 into
   data_ov006_0214236c the right count. THE CODE BELOW NEVER DEPENDS ON THE ROW
   COUNT: the array is declared unsized and indexed identity*5 + frame. The
   dealer caps the identity at 6 (src/func_ov006_020f4cd8.c's widest arm draws
   1..6) and THIS CLASS'S OWN flip driver clamps the frame at 4 --
   src/func_ov006_020f41b0.c ends `if (*st > 4) { *st = 4; ... }` and
   src/func_ov006_020f411c.c is the other direction, decrementing it back
   toward zero. Both are matched TUs of this class, so the bound is not
   inherited from the sibling's equivalent. The highest index the game can
   produce is therefore 6*5 + 4 = 34 -- 0x0213d1ac, the last halfword of row 6,
   one short of the pad.

   BOTH POOL WORDS ARE THIS FUNCTION'S ONLY READER. ov006's relocs.txt has
   exactly one load to 0x0213d168 in the whole overlay (from:0x020f3f08, this
   body's own pool) and exactly two to 0x0214236c (from:0x020f3f0c, the same
   pool, and from:0x02131468, the constructor that fills it). Nothing else in
   the overlay reads either table.

   AND THE DRAW IS UNIQUE IN THE CLASS, measured off the relocation set rather
   than off a reading: ov006 has thirty-two arm_call relocations to
   Hud_RenderSprite (ov004, 0x020af68c) and exactly ONE of them, 0x020f3ee8, is
   inside this class's code block 0x020f3834..0x020f5564. The other one that
   looks close, 0x020f5c18, is the sibling's own body.

   The fifth argument is the object's main state index at +0x5314 -- the one
   vtable slot 6 dispatches on and slot 18 zeroes -- as a flag: every state but
   2 passes 1.

   EVERY FIELD THIS BODY READS IS WRITTEN BY AN ALREADY-MATCHED src TU WITH THE
   SAME MEANING, so the field reading above closes against src rather than
   against a comment:

     +0x12  src/func_ov006_020f4cd8.c writes `*(u8 *)(p + 0x51ba) = 1` in each
            of its three board arms, and src/func_ov006_020f4f94.c zeroes it in
            the per-round clear.  In play, exactly as read.
     +0x10  the same dealer writes `*(u8 *)(p + 0x51b8) = slot`, where slot is
            RandomIntInternal scaled into 1..4, 1..5 or 1..6 and refused until
            that identity has been dealt fewer than twice.  Six identities is
            the table's six non-zero rows.
     +0x15  the clear zeroes +0x51bd beside the other two.  The three bytes
            this body reads are the three that file resets.
     count  func_ov006_020f3f10, the arity-1 per-card dispatcher, walks the SAME
            records with `cmp r6,#0xc` and `add r5,r5,#0x18` off the same
            +0x5000 base -- a second body in this class agreeing that there are
            twelve of them, reached without going through this one.  It gates on
            +0x13 and dispatches on +0x14, two bytes this body never touches.
     x/y    the dealer starts every card at 0x80000 / -0x80000, which is
            128.0 / -128.0 in the 20.12 this body shifts down by 12. */

extern void Hud_RenderSprite(void *a0, int a1, int a2, int a3, int a4);
extern unsigned short data_ov006_0213d168[];
extern void *data_ov006_0214236c[];

void func_ov006_020f3e68(char *c)
{
    char *p;
    int i;
    int flag;

    p = c;
    for (i = 0; i < 0xc; i++) {
        if (*(unsigned char *)(p + 0x51ba) != 0) {
            flag = 0;
            if (*(int *)(c + 0x5314) != 2) flag = 1;
            Hud_RenderSprite(data_ov006_0214236c[data_ov006_0213d168[
                                 *(unsigned char *)(p + 0x51b8) * 5 +
                                 *(unsigned char *)(p + 0x51bd)]],
                             *(int *)(p + 0x51a8) >> 12,
                             *(int *)(p + 0x51ac) >> 12,
                             -1, flag);
        }
        p += 0x18;
    }
}
