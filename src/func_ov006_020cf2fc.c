// NONMATCHING: one extra code word and the vertex-pack instruction schedule
// (37 divergences against 258 ROM code words). Run mg13, lane TRAMP.
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
// value difference. Every adjudicated constant is the ROM's: the three
// MTX_SCALE writes of 0x100000, the mode sequence 2 / 1 / 3, the identity
// write, the 0x8da70000 | (data_ov006_02140844 >> 3) texture image parameter,
// the data_ov006_02140814 >> 4 palette base, the 0x82 polygon-attribute base
// with the record's +0x31e (plus one, shifted 24) and +0x329 (shifted 16),
// the 0x8000 specular/emission word, the 0xc0000000 normal mask, the
// (v << 8) >> 16 packing, the strip type 2, and both loop bounds (rows 0..2,
// columns 0..3).
//
// THE DIVERGENCES, ACCOUNTED FOR IN FULL. 222 of the ROM's 258 code words
// align exactly; the 36 that do not, plus this body's one extra word, are the
// 37, and they fall into six classes:
//
//  1. THE VERTEX-PACK SCHEDULE (24 of the 36 ROM words: +0x1c8..+0x1cc,
//     +0x1d4..+0x1e8 and +0x200; +0x230..+0x234, +0x23c..+0x250 and +0x268;
//     +0x384..+0x38c, +0x3a4..+0x3a8 and +0x3bc). Same instructions, same
//     count, same values, same store order. The ROM loads x, y and z up front
//     and then runs the x and y shift chains in lockstep with z lagging two
//     slots; this body runs z, then x, then y, and defers the y load to sit
//     beside the pointer bump. Three of the four pack sites carry it -- loop
//     two's first pack (+0x31c..+0x360) matches the ROM instruction for
//     instruction. EIGHTY-FOUR SPELLINGS WERE COMPILED AND MEASURED against
//     this and not one moved the schedule: seven emit forms (raw locals packed
//     into two words, shifted locals, u16 locals, an s32 helper, an s16 helper,
//     a bare two-store expression, a whole-struct read), four declaration
//     orders, both scopes for the temporaries and for the packed pair
//     independently, and all six orders of the three component reads. The
//     whole-struct read costs 56 bytes and the bare two-store expression loses
//     24; every other spelling is 0x460 and lands on 37. mwcc canonicalises
//     this ahead of scheduling, so source order does not reach it.
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
//  6. THE SPECULAR/EMISSION STORE (1 word: +0x138). It lands four words later
//     here than in the ROM. The value is identical and nothing reads the
//     register between the two positions.
//
//  AND THE ONE EXTRA WORD, which is this body's, not the ROM's: a mov at
//  +0x2bc that copies loop one's induction register into loop two's. The ROM
//  reuses the register in place because the zero it wrote to END_VTXS through
//  loop one is still sitting there. Spilling that pair to distinct source
//  variables closes the word (both A-outer and F-outer forms compile to
//  exactly 0x45c) but costs 32 more schedule divergences, so it is not taken.
//
// HOW THE 37 WAS MEASURED, BECAUSE THE NUMBER MEANS NOTHING WITHOUT IT. The
// sizes differ (0x460 against 0x45c), so match.py's compare() returns its
// sentinel and a raw prefix compare is not a usable instrument here -- the one
// extra word walks every later pc-relative pool displacement, which is why the
// byte-identical prefix reads as six words when 222 of 258 actually line up.
// 37 is a difflib alignment over the decoded instruction stream with register
// tokens holed out, call destinations holed (the object's bl slots are
// unlinked relocs, so their printed target is link state rather than codegen
// -- the byte gate wildcards them for the same reason), pc-relative pool
// displacements holed, intra-function branches compared by DISTANCE, and the
// literal pool excluded. The frame is NOT normalised out of the figure; it
// happens to be exact. Re-scored under that one instrument on one clean tree
// at 2004/b56, the near-miss row banked in nearmiss/db.jsonl before this run
// is 61 (76.4%) and this body is 37 (86.0%).
//
// NOT REGISTER NAMING, AND THE BANNER DOES NOT CLAIM IT IS. The instrument
// holes register names out, so none of the 37 is a colouring difference; they
// are all position or count. Register naming does differ widely and is not
// counted here.
//
// THE HOST BUILD REWRITES EVERY STORE BELOW. All thirty-two geometry-engine
// writes are raw MMIO, which on the host latches into mapped memory and draws
// nothing, so func_ov006_020cf2fc is a TT_SHARED_HOSTGEN symbol in
// port/CMakeLists.txt and both trampoline slices take the generated copy.
// Seating it without that wiring would compile, link, run, and render an empty
// screen.

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
            az = va->z;
            p0 = (u16)((ax << 8) >> 16) | ((u32)(u16)((ay << 8) >> 16) << 16);
            p1 = (u16)((az << 8) >> 16);
            *(volatile unsigned int *)0x400048c = p0;
            *(volatile unsigned int *)0x400048c = p1;
            va++;
            *(volatile unsigned int *)0x4000488 = data_ov006_0212e0b0[n];
            *(volatile unsigned int *)0x4000484 = (0 - (*(nb++))) & ~0xC0000000;
            n++;
            bx = vb->x;
            by = vb->y;
            bz = vb->z;
            p0 = (u16)((bx << 8) >> 16) | ((u32)(u16)((by << 8) >> 16) << 16);
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
            bz = vb->z;
            p0 = (u16)((bx << 8) >> 16) | ((u32)(u16)((by << 8) >> 16) << 16);
            p1 = (u16)((bz << 8) >> 16);
            *(volatile unsigned int *)0x400048c = p0;
            *(volatile unsigned int *)0x400048c = p1;
            vb++;
            *(volatile unsigned int *)0x4000488 = data_ov006_0212e0b0[i * 4 + k];
            *(volatile unsigned int *)0x4000484 = *(na++);
            ax = va->x;
            ay = va->y;
            az = va->z;
            p0 = (u16)((ax << 8) >> 16) | ((u32)(u16)((ay << 8) >> 16) << 16);
            p1 = (u16)((az << 8) >> 16);
            *(volatile unsigned int *)0x400048c = p0;
            *(volatile unsigned int *)0x400048c = p1;
            va++;
        }
        *(volatile unsigned int *)0x4000504 = 0;
    }
}
