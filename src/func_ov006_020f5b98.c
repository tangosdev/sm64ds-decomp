// NONMATCHING: register colouring + schedule only (div=30). Logic verified
// correct vs ROM; not byte-matchable from C at mwccarm 2004/b56 (see
// notes/matching-style.md). Counts as decompiled, not matched.
//
// The divergence number reads high and is measured two ways, both recorded
// because they disagree about what a scheduling move costs. tools/match.py:
// 24 of 42 words differ. tools/nonmatching.py: 30, an edit distance over the
// disassembly text, where moving one instruction costs an insert plus a
// delete. NEITHER IS A LOGIC DIFFERENCE: the body compiles to FORTY
// instructions with an identical mnemonic multiset to the ROM's, and the only
// opcode-shape difference anywhere in it is the bl's target, which is the
// wildcarded relocation. Every differing word differs by a register number or
// a schedule slot. Only 1.2/base, 1.2/sp2, 1.2/sp2p3 and 2004/b56 produce a
// 0xa8 body at all; the other twenty-one installed builds come out a different
// size. port/mg_fanout_costs.txt section 15 has the residue, what was tried
// against it (about thirty source shapes, ten compilers x twelve optimisation
// settings, 30,452 decomp-permuter iterations over three runs from two
// seeds) and the src/func_ov006_020f6904.c precedent in this same class at
// div=29.
//
// TWO SHAPES REACH 23 AND NEITHER IS SHIPPED, for two different reasons.
// Writing the fifth argument as the bare comparison `*(int *)(c + 0x53d4) != 2`
// costs one word less, but it compiles the flag as a movne/moveq PAIR where the
// ROM emits an unconditional mov of a hoisted zero followed by a movne of a
// hoisted one -- which is the if-statement below.
//
// THE JULY NEAR-MISS ROW ALSO REACHES 23 AND DOES KEEP THAT CONSTRUCT, so the
// trade is not "one word for the construct" and an earlier version of this note
// implied it was. Its cost is the declaration instead: nearmiss/db.jsonl's row
// for this address declares an Obj carrying a twenty-element recs[] array, then
// walks the loop by striding the Obj* itself by 0x18 and reading recs[0] every
// time -- so the array it declares is never indexed and the type it advances is
// not 0x18 wide. A word closer to the bytes and a source a reader has to
// disbelieve is the wrong trade for a file that is going to be read as the
// ROM's logic. The row stays in the DB and is the right seed for the next
// attempt at this address.
//
// @symbol func_ov006_020f5b98
// recovered name: dScMgMemory2_c_DrawCards
/* dScMgMemory2_c's card draw: the sixth call vtable slot 9 (Render) makes,
   from src/func_ov006_020f73f4.c.

   TWENTY records at +0x51a8, stride 0x18. src/func_ov006_020f6c90.c fills
   16, 18 or 20 of them at state 1 depending on the difficulty byte +0x540a,
   each with a card identity in 1..8, 1..9 or 1..10 dealt twice over;
   src/func_ov006_020f6088.c walks them from x = 128.0, y = -128.0 to their
   slot, and src/func_ov006_020f5de0.c and _020f5e74.c drive the flip frame.

   Per record:
     +0x12  nonzero when the card is in play (this loop's whole gate)
     +0x00  screen x, 20.12, shifted down by 12 for the sprite
     +0x04  screen y, 20.12
     +0x10  card identity, the ROW of the halfword table at
            data_ov006_0213d45c
     +0x15  flip frame 0..4, its COLUMN

   THE TABLE IS ELEVEN ROWS OF FIVE, not twelve. 0x78 to the next symbol
   divides by ten, but the last ten bytes are not a row: rows 0..10 end at
   0x0213d4ca, then six pad bytes, then a RELOCATED pointer at 0x0213d4d0 ->
   0x0213d350 (ov006 relocs.txt, from:0x0213d4d0). Row 0 is all zero and rows
   1..10 read {0, 1, 2, 2k+2, 2k+1}, so the highest halfword in it is 0x16 = 22
   -- which is what makes data_ov006_02142490's twenty-three entries the right
   size. THE CODE BELOW NEVER DEPENDED ON THE ROW COUNT: the array is declared
   unsized and indexed identity*5 + frame, func_ov006_020f6c90 caps the
   identity at 10 and func_ov006_020f5e74 clamps the frame at 4, so the highest
   index the game can produce is 54 -- 0x0213d4c8, the last halfword of row 10,
   one short of the pad.

   The halfword the table yields indexes data_ov006_02142490, the twenty-three
   OAM pointers __sinit_ov006_021314e4 copies out of data_ov006_02133810.

   The fifth argument is the object's state index at +0x53d4, as a flag: every
   state but 2 passes 1. */

extern void Hud_RenderSprite(void *a0, int a1, int a2, int a3, int a4);
extern unsigned short data_ov006_0213d45c[];
extern void *data_ov006_02142490[];

void func_ov006_020f5b98(char *c)
{
    char *p;
    int i;
    int flag;

    p = c;
    for (i = 0; i < 0x14; i++) {
        if (*(unsigned char *)(p + 0x51ba) != 0) {
            flag = 0;
            if (*(int *)(c + 0x53d4) != 2) flag = 1;
            Hud_RenderSprite(data_ov006_02142490[data_ov006_0213d45c[
                                 *(unsigned char *)(p + 0x51b8) * 5 +
                                 *(unsigned char *)(p + 0x51bd)]],
                             *(int *)(p + 0x51a8) >> 12,
                             *(int *)(p + 0x51ac) >> 12,
                             -1, flag);
        }
        p += 0x18;
    }
}
