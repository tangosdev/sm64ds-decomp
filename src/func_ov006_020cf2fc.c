// NONMATCHING: one extra code word and the vertex-pack instruction schedule in
// loop one (25 divergences against 258 ROM code words). Run mg13, lane TRAMP.
//
// WHAT IS EXACT. The push list and the 0x7c stack frame. All SIX external
// calls -- Matrix4x3_FromTranslation, MulMat4x3Mat4x3, Matrix4x3_Apply-
// InPlaceToScale, MulMat4x3Mat4x3, func_020553a4, func_020553a4 -- are
// present, in the ROM's order, at the ROM's own byte offsets +0x1c, +0x2c,
// +0x44, +0x54, +0x68 and +0x7c. The literal pool is 21 words in both and
// carries the same seven relocated symbols and the same fourteen constants
// (0x04000440, 0x0400046c, 0x04000454, 0x8da70000, 0x040004a8, 0x040004ac,
// 0x04000484, 0x040004a4, 0x040004c0, 0x040004c4, 0x04000488, 0x0400048c,
// 0x04000500, 0x04000504); four of the constant slots sit one place apart
// from the ROM's, which is a consequence of the extra word below and not a
// value difference. BOTH of loop two's vertex packs (+0x31c..+0x360 and
// +0x380..+0x3c4) match the ROM instruction for instruction. Every adjudicated
// constant is the ROM's: the three MTX_SCALE writes of 0x100000, the mode
// sequence 2 / 1 / 3, the identity write, the
// 0x8da70000 | (data_ov006_02140844 >> 3) texture image parameter, the
// data_ov006_02140814 >> 4 palette base, the 0x82 polygon-attribute base with
// the record's +0x31e (plus one, shifted 24) and +0x329 (shifted 16), the
// 0x8000 specular/emission word, the 0xc0000000 normal mask and its negation
// in loop one only, the (v << 8) >> 16 packing, the strip type 2, and both
// loop bounds (rows 0..2, columns 0..3).
//
// THE DIVERGENCES, ACCOUNTED FOR IN FULL. 234 of the ROM's 258 code words
// align exactly; the 24 that do not, plus this body's one extra word, are the
// 25, and they fall into six classes:
//
//  1. LOOP ONE'S VERTEX-PACK SCHEDULE (12 of the 24 ROM words, six per element:
//     +0x1c8, +0x1d4..+0x1d8, +0x1e4..+0x1e8 and +0x200 for the first;
//     +0x230, +0x23c..+0x240, +0x24c..+0x250 and +0x268 for the second). Same
//     instructions, same count, same values, same store order. The ROM loads
//     x, y and z up front and runs the x and y shift chains in lockstep with z
//     lagging two slots; loop one here defers the y load to sit beside the
//     pointer bump. LOOP TWO USED TO CARRY THE SAME SHAPE AND NO LONGER DOES:
//     splitting the x conversion into a statement of its own ahead of the pack,
//     at all four sites, closed both of loop two's packs and took the figure
//     37 -> 31 -> 25. That lever is the ONE thing that moved this. Ninety-odd
//     spellings were compiled and measured against it first -- seven emit forms
//     (raw locals packed into two words, shifted locals, u16 locals, an s32
//     helper, an s16 helper, a bare two-store expression, a whole-struct read),
//     four declaration orders, both scopes for the temporaries and for the
//     packed pair independently, all six orders of the three component reads,
//     and four loop-one-only read orders -- and every one of them landed on the
//     same schedule. Splitting y or z the same way costs 24 bytes (mwcc folds a
//     pack it can see through) or 15 divergences. The lever came from the
//     permuter, not from hand search.
//
//  2. THE POINTER AND COUNTER BUMPS (5 words: +0x210, +0x21c, +0x270, +0x368,
//     +0x378). va++, vb++ and n++ land one to three slots from the ROM's
//     position inside the same loop body. Downstream of class 1.
//
//  3. THE INNER-LOOP COMPARE (2 words: +0x1b4, +0x3cc). The ROM issues
//     cmp k, #4 one slot after the normal-array load; this body issues it one
//     slot before. Same instruction, same operand, flags consumed by the same
//     branch at the bottom of the body in both.
//
//  4. LOOP ONE'S SETUP (2 words: +0x174, +0x19c). The (i + 1) * 4 shift and
//     the column-counter seed swap places with two neighbours.
//
//  5. LOOP TWO'S SETUP (2 words: +0x2d0, +0x2d8). The same pair, one loop on.
//
//  6. THE SPECULAR/EMISSION STORE (1 word: +0x138). It lands three words later
//     here than in the ROM. The value is identical and nothing reads the
//     register between the two positions.
//
//  AND THE ONE EXTRA WORD, which is this body's, not the ROM's: a mov at
//  +0x2bc that copies loop one's induction register into loop two's. The ROM
//  reuses the register in place because the zero it wrote to END_VTXS through
//  loop one is still sitting there. Spelling the two loop counters and the two
//  emitted zeroes as distinct source variables closes the word -- both such
//  forms compile to exactly 0x45c -- but costs 32 more schedule divergences,
//  so it is not taken.
//
// HOW THE 25 WAS MEASURED, BECAUSE THE NUMBER MEANS NOTHING WITHOUT IT. The
// sizes differ (0x460 against 0x45c), so match.py's compare() returns its
// sentinel and a raw prefix compare is not a usable instrument here -- the one
// extra word walks every later pc-relative pool displacement, which is why the
// byte-identical prefix reads as six words when 234 of 258 actually line up.
// 25 is a difflib alignment over the decoded instruction stream with register
// tokens holed out, call destinations holed (the object's bl slots are
// unlinked relocs, so their printed target is link state rather than codegen
// -- the byte gate wildcards them for the same reason), pc-relative pool
// displacements holed, intra-function branches compared by DISTANCE, and the
// literal pool excluded. The frame is NOT normalised out of the figure; it
// happens to be exact. Re-scored under that one instrument on one clean tree
// at 2004/b56, the near-miss row banked in nearmiss/db.jsonl before this run
// is 61 (76.4%) and this body is 25 (90.7%).
//
// THE NEAR-MISS DB STILL HOLDS THE OTHER ROW ON PURPOSE. Its own metric keeps
// register names, and by that measure the older row is 190 and this body 194,
// so nearmiss_db.py ingest declined the swap. The two instruments disagree
// because the older row is nearer in raw instruction text and this one is
// nearer in structure; both readings are recorded rather than one being
// quietly preferred. (The ingest also tripped the known duplicate-key bug --
// an int addr against a string addr -- and nearmiss_db.py dedupe put the DB
// back to 218 unique entries.)
//
// NOT REGISTER NAMING, AND THE BANNER DOES NOT CLAIM IT IS. The instrument
// holes register names out, so none of the 25 is a colouring difference; they
// are all position or count. Register naming does differ widely and is not
// counted here.
//
// A PERMUTER OUTPUT SCORING 19 WAS REJECTED AS WRONG, and that is worth
// recording because it compiles and it scores better. It reaches 19 by masking
// the first VTX_16 word with 0xFFFF, which drops the packed Y coordinate --
// the ROM assembles that word with orr sl, sl, ip, lsl #16 and stores both
// halves. A permuter transformation is not a semantics proof; only the
// x-conversion split above was taken from that output.
//
// THE HOST BUILD REWRITES EVERY STORE BELOW. All thirty-two geometry-engine
// writes are raw MMIO, which on the host latches into mapped memory and draws
// nothing, so func_ov006_020cf2fc is a TT_SHARED_HOSTGEN symbol in
// port/CMakeLists.txt and both trampoline slices take the generated copy.
// Seating it without that wiring would compile, link, run, and render an empty
// screen -- which is exactly what the build before this seat did.

typedef unsigned int u32;
typedef unsigned short u16;
typedef short s16;
typedef int s32;

struct Vec { s32 x, y, z; };

extern s32 data_020a0e68[12];
extern s32 data_0209b3ec[12];
extern u16 data_ov006_0212e060[];
extern u16 data_ov006_0212e068[];
extern s32 data_ov006_0212e0b0[];
extern u32 data_ov006_02140844;
extern u32 data_ov006_02140814;
extern void Matrix4x3_FromTranslation(s32 *m, s32 x, s32 y, s32 z);
extern void MulMat4x3Mat4x3(s32 *a, s32 *b, s32 *dst);
extern void Matrix4x3_ApplyInPlaceToScale(s32 *m, s32 x, s32 y, s32 z);
extern void func_020553a4(s32 *m);

void func_ov006_020cf2fc(char *obj)
{
    s32 i;
    s32 k;
    s32 n;
    struct Vec *va;
    struct Vec *vb;
    s32 *na;
    s32 *nb;
    s32 m2[12];
    s32 m1[12];

    Matrix4x3_FromTranslation(data_020a0e68, *((s32 *)(obj + 8)), *((s32 *)(obj + 0xc)), *((s32 *)(obj + 0x10)));
    MulMat4x3Mat4x3(data_020a0e68, data_0209b3ec, m1);
    Matrix4x3_ApplyInPlaceToScale(data_020a0e68, *((s32 *)(obj + 0x2c)), *((s32 *)(obj + 0x30)), *((s32 *)(obj + 0x34)));
    MulMat4x3Mat4x3(data_020a0e68, data_0209b3ec, m2);
    *(volatile unsigned int *)0x4000440 = 2;
    func_020553a4(m1);
    *(volatile unsigned int *)0x4000440 = 1;
    func_020553a4(m2);
    *(volatile unsigned int *)0x400046c = 0x100000;
    *(volatile unsigned int *)0x400046c = 0x100000;
    *(volatile unsigned int *)0x400046c = 0x100000;
    *(volatile unsigned int *)0x4000440 = 3;
    *(volatile unsigned int *)0x4000454 = 0;
    *(volatile unsigned int *)0x40004a8 = 0x8da70000 | (data_ov006_02140844 >> 3);
    *(volatile unsigned int *)0x40004ac = data_ov006_02140814 >> 4;
    *(volatile unsigned int *)0x40004a4 =
        (((*((s16 *)(obj + 0x31e)) + 1) << 24) | 0x82) | (*((unsigned char *)(obj + 0x329)) << 16);
    *(volatile unsigned int *)0x40004c0 =
        data_ov006_0212e060[*((u16 *)(obj + 0x326))] | (data_ov006_0212e068[*((u16 *)(obj + 0x326))] << 16);
    *(volatile unsigned int *)0x40004c4 = 0x8000;
    for (i = 0; i < 3; i++) {
        *(volatile unsigned int *)0x4000500 = 2;
        n = (i + 1) * 4;
        va = ((struct Vec *)(obj + 0x5c)) + i * 4;
        vb = ((struct Vec *)(obj + 0x5c)) + n;
        na = ((s32 *)(obj + 0x2dc)) + i * 4;
        nb = ((s32 *)(obj + 0x2dc)) + n;
        for (k = 0; k < 4; k++) {
            s32 ax, ay, az;
            s32 bx, by, bz;
            u32 p0, p1;
            *(volatile unsigned int *)0x4000488 = data_ov006_0212e0b0[i * 4 + k];
            *(volatile unsigned int *)0x4000484 = (0 - (*(na++))) & ~0xC0000000;
            ax = va->x;
            ay = va->y;
            ax = (ax << 8) >> 16;
            az = va->z;
            p0 = (u16)ax | ((u32)(u16)((ay << 8) >> 16) << 16);
            p1 = (u16)((az << 8) >> 16);
            *(volatile unsigned int *)0x400048c = p0;
            *(volatile unsigned int *)0x400048c = p1;
            va++;
            *(volatile unsigned int *)0x4000488 = data_ov006_0212e0b0[n];
            *(volatile unsigned int *)0x4000484 = (0 - (*(nb++))) & ~0xC0000000;
            n++;
            bx = vb->x;
            by = vb->y;
            bx = (bx << 8) >> 16;
            bz = vb->z;
            p0 = (u16)bx | ((u32)(u16)((by << 8) >> 16) << 16);
            p1 = (u16)((bz << 8) >> 16);
            *(volatile unsigned int *)0x400048c = p0;
            *(volatile unsigned int *)0x400048c = p1;
            vb++;
        }
        *(volatile unsigned int *)0x4000504 = 0;
    }
    for (i = 0; i < 3; i++) {
        *(volatile unsigned int *)0x4000500 = 2;
        n = (i + 1) * 4;
        va = ((struct Vec *)(obj + 0x5c)) + i * 4;
        vb = ((struct Vec *)(obj + 0x5c)) + n;
        na = ((s32 *)(obj + 0x2dc)) + i * 4;
        nb = ((s32 *)(obj + 0x2dc)) + n;
        for (k = 0; k < 4; k++) {
            s32 ax, ay, az;
            s32 bx, by, bz;
            u32 p0, p1;
            *(volatile unsigned int *)0x4000488 = data_ov006_0212e0b0[n];
            *(volatile unsigned int *)0x4000484 = *(nb++);
            n++;
            bx = vb->x;
            by = vb->y;
            bx = (bx << 8) >> 16;
            bz = vb->z;
            p0 = (u16)bx | ((u32)(u16)((by << 8) >> 16) << 16);
            p1 = (u16)((bz << 8) >> 16);
            *(volatile unsigned int *)0x400048c = p0;
            *(volatile unsigned int *)0x400048c = p1;
            vb++;
            *(volatile unsigned int *)0x4000488 = data_ov006_0212e0b0[i * 4 + k];
            *(volatile unsigned int *)0x4000484 = *(na++);
            ax = va->x;
            ay = va->y;
            ax = (ax << 8) >> 16;
            az = va->z;
            p0 = (u16)ax | ((u32)(u16)((ay << 8) >> 16) << 16);
            p1 = (u16)((az << 8) >> 16);
            *(volatile unsigned int *)0x400048c = p0;
            *(volatile unsigned int *)0x400048c = p1;
            va++;
        }
        *(volatile unsigned int *)0x4000504 = 0;
    }
}
