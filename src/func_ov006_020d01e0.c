// NONMATCHING: instruction scheduling and one block-copy spelling
// (49 of 503 code words differ). Run mg12, lane INST.
//
// WHAT IS EXACT: the 0xd4 stack frame, and every referenced slot in it bar one
// -- the ROM leaves sp+0x34 as a hole and this body uses it. All 25 external
// and virtual calls are present, in the ROM's order, with the ROM's own
// destinations; thirteen of them (Vec3_Add +0x0d4, Vec3_MulScalar +0x0e4,
// Vec3_Sub +0x118, func_0203ce80 +0x144, func_0203cf00 +0x16c,
// Vec3_MulScalarInPlace +0x208, SubVec3 +0x21c and +0x230, Vec3_Dist +0x23c,
// func_ov006_020cf040 +0x49c, func_ov006_020cf124 +0x4a4, and the first two
// element virtuals at +0x500 and +0x518) sit at the ROM's own byte offset. The
// other twelve carry the length divergence: 4 bytes from +0x540, 20 from
// +0x698. Both literal pools are 9 words. Every adjudicated constant is the
// ROM's: 0x1f at +0x329, the three accept tests (d3 < 0x100, d2 <= best,
// d2 > -0x24000), the -half < d1 < half extent test, the 0x400000 branch,
// 0x1b1 / 0x1ae, and the 0x180 / 0x100 / 0x555 grid steps.
//
// THE DIVERGENCES, and the 49 are ACCOUNTED FOR IN FULL below -- the five named
// ranges hold 42, and the remaining 7 are the already-stated length delta (-24
// bytes, which walks five branch displacements and the six call sites after
// +0x698 -- the call sites contribute zero edits), the one extra frame slot at
// sp+0x34, and the +0x53c spill named in
// divergence 1. Measured, not eyeballed:
//
//  1. THE 24-BYTE COPY (+0x5f4..+0x628, ~13 words). The ROM issues six ldr then
//     six str to move relPos/rawDir into bestRel/bestDir. A whole-struct
//     assignment gives mwccarm ldm/stm instead -- six words, not twelve.
//     Field-by-field DOES produce twelve ldr/str, but mwccarm then scalarises
//     both destinations out of the aggregate region entirely and the body
//     scores +69 rather than -13. Six copy spellings were measured (whole
//     struct, two Vec3 assignments, field by field, field by field with .x
//     last, through a volatile-qualified read, and as an array of two Vec3);
//     none gives both the twelve stores and the ROM's frame position. The
//     ldm/stm also needs two address registers the ROM never ties up, which is
//     where the one extra spill at sp+0x18 (+0x53c) comes from.
//
//  2. THE INNER LOOP SCHEDULE (~16 words). Same instructions, same count; my
//     i++, colFrac += 0x100, the grid[0] reload and the &grid[1] materialise
//     land earlier in the block than the ROM's. Nothing about the values or the
//     order of the stores changes.
//
//  3. THE TAIL'S SECOND abs (+0x6a4..+0x6bc, ~7 words). The ROM re-reads
//     bestD1 from sp+0xc in BOTH arms (ldrlt / ldrge); this body keeps it in a
//     register (rsblt / movge). Pressure-driven: three spellings (named twice,
//     inlined once, inlined twice) were each measured inert.
//
//  4. THE va SETUP (+0x08c..+0x0a4, 4 words). The order of the two ldrsh/str
//     pairs.
//
//  5. THE OUTER-LOOP SPILL SLOT (+0x290..+0x294, 2 words). The ROM parks the
//     inner counter's reload in sp+0x18 and this body uses sp+0x20:
//         ROM   str r0, [sp, #0x18]  /  ldr r8, [sp, #0x18]
//         here  str r0, [sp, #0x20]  /  ldr r5, [sp, #0x20]
//     Same two instructions, same value, one slot apart. It is the tail of the
//     scalar-region shift that the extra sp+0x34 slot causes -- the ROM's
//     compiler-hoisted temps land at 0x14/0x18/0x1c and this body needs a fourth,
//     so p2, flag and p1 all ride up with it. Whatever puts a live 14th scalar at
//     sp+0x34 closes this at the same time; nineteen spellings were measured
//     against that slot and every one was inert.
//
// NOT REGISTER NAMING, AND THE BANNER DOES NOT CLAIM IT IS. 76 further
// instructions line up but differ in register names or pool offsets; those are
// downstream of the five above and are not counted in the 49.
//
// HOW THE 49 WAS MEASURED, BECAUSE THE NUMBER MEANS NOTHING WITHOUT IT. The
// sizes differ (0x7e8 against 0x800), so match.py's compare() returns its 999
// sentinel and a raw prefix compare is not a usable instrument: one extra
// instruction near the top shifts every later word, and the two earlier drafts
// of this body scored 393 and 457 that way -- which is why both read as
// hopeless when they were not. 49 is a difflib alignment over the decoded
// instruction stream with register tokens holed out, call destinations holed
// (the object's bl slots are unlinked relocs, so their printed target is link
// state rather than codegen -- the byte gate wildcards them for the same
// reason), intra-function branches compared by DISTANCE, and the literal pool
// excluded. Re-scored under that one instrument on one tree at 2004/b56, the
// banked fanout-fable near-miss row is 195 (61.2%) and lane TRM's phase-2 draft
// 205 (59.2%). The frame is NOT normalised out of the figure; a frame difference
// is real.
//
// (An earlier version of this banner said 200 and 210. Those two were measured
// before the literal-pool exclusion was added to the scorer and never re-run
// after, so they were pool-INCLUDED figures being compared against this body's
// pool-EXCLUDED 49 -- five words of unresolved data relocs each, flattering this
// seat by the same five. Both drafts were re-run with the shipped scorer to get
// the pair above. The comparison is like-for-like now.)
//
// WHAT MOVED IT, so the next lane does not re-derive it: p1/p2 kept as
// UNROUNDED s64 across the rounding; rowAbs/f3 computed inside the inner loop
// so mwccarm hoists 0 - rowFrac to a slot by itself (lever 6bn, observed);
// grid[2] reloading *(s32 *)(c + 0x58) rather than the cached half; f2 computed
// before the rowAbs block; the last six Vec3 being ONE ARRAY OF SEVEN (0x80 +
// 7*12 == 0xd4 exactly, and the ROM never touches the seventh -- a dead struct,
// scalar, array and volatile were each measured inert, so the seventh survives
// only because the array's address escapes to the calls: this alone took the
// frame from 0xc4 to the ROM's 0xd4); assigning bestObj from obj rather than
// re-indexing the table (-55, the largest single step, and semantically
// identical because the table cannot change inside the iteration); and hoisting
// j to function scope so it takes sp+0x8 and bestD1 takes sp+0xc as in the ROM.
//
// Counts as decompiled, not matched. No delinks block, by the NONMATCHING seat
// convention (src/MgTrampolineTerror_Spawn.c precedent, and its sibling
// src/func_ov006_020d0c38.c in this same family).
//
// WHAT IT DOES: this is the trampoline-family STROKE INSTALLER, the body that
// turns an accepted stylus stroke into a trampoline. src/func_ov006_020d0b78.c
// calls it to install the scene's own starting trampoline at setup, and
// src/func_ov006_020d0c38.c (the stylus hit test on vtable slot 23) calls it
// once a stroke has been judged and a free record found. Given the record and
// the two endpoints it: stores the raw endpoints at +0x50/+0x54; shifts both by
// -0x80 in x and negates y; swaps them if a.x > b.x; builds the midpoint into
// +0x08 and the along-stroke vector into +0x20; crosses that with world up into
// +0x14, flipping it if its y is negative; writes the two draw vectors at +0x38
// and +0x44; measures the stroke with Vec3_Dist, keeping half at +0x58 and a
// third as the row pitch; fills a 4x4 grid of Fix12 vertices at +0x5c and
// copies it to +0x11c, the row and column weights running -0x180..+0x180 in
// steps of 0x100 and the along-stroke parameter 0..0xfff in steps of 0x555;
// then walks the five element records in data_ov006_0214097c, asking each three
// virtuals, and keeps the one whose projections fall inside the stroke's own
// extents. On a hit it fills the element's fields, plays 0x1b1 or 0x1ae through
// func_ov006_020e6db4 with a blend of data_ov006_0213b2f8/2fc/300/304, and
// SETS THE RENDER GATE AT +0x328 TO 3. With no hit it writes 0x258 at +0x31c
// and sets the gate to 1. Either way the gate goes nonzero, which is what lets
// src/func_ov006_020d09e0.c and src/func_ov006_020d0ac0.c look at the record at
// all -- so this body is the last thing between a drawn line and a trampoline
// on screen.
//
// The reading is corroborated, not asserted: lane TRM's phase-2 derivation and
// an independent fanout-fable row in nearmiss/db.jsonl agree on every
// adjudicated constant, with no disagreement, and this lane re-derived the
// disassembly from extracted/overlays/overlay_0006.bin (never the shifted dsd
// copy) and found 512 of 512 words identical to TRM's listing.
//
// ONE OF ITS CALLS IS DEAD IN THE ROM: the fourth DotVec3 in the element loop
// (+0x5b8) has its result discarded. It is kept because the ROM keeps it.
//
// THE `volatile` IN THE SWAP TEST IS LOAD-BEARING, not leftover noise. The ROM
// re-reads both shorts from memory at +0x05c/+0x060, after the four stores just
// above; without the volatile casts mwccarm keeps them in registers and the body
// comes out two words SHORT and two edits worse (measured both ways, plain casts
// and bare shorts). Do not tidy it away.
typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef unsigned int u32;
typedef int s32;
typedef long long s64;
typedef s32 Fix12i;

struct Vec3 { s32 x, y, z; };
struct Pair { struct Vec3 a, b; };
typedef struct Vec3 Vector3;

extern void Vec3_Add(struct Vec3* out, struct Vec3* a, struct Vec3* b);
extern void Vec3_MulScalar(struct Vec3* out, struct Vec3* in, int s);
extern void Vec3_Sub(struct Vec3* out, struct Vec3* a, struct Vec3* b);
extern void func_0203ce80(struct Vec3* dst, struct Vec3* src);
extern void func_0203cf00(struct Vec3* out, struct Vec3* a, struct Vec3* b);
extern void Vec3_MulScalarInPlace(int *v, int s);
extern void SubVec3(struct Vec3 *a, struct Vec3 *b, struct Vec3 *c);
extern Fix12i Vec3_Dist(const Vector3* a, const Vector3* b);
extern Fix12i DotVec3(const Vector3 *a, const Vector3 *b);
extern int _ZN4cstd4fdivEii(int a, int b);
extern void func_ov006_020cf040(void *a, void *b, void *c);
extern void func_ov006_020cf124(void *a);
extern void func_ov006_020e6db4(int a0, int a1, int a2);
extern void func_ov006_020cfa28(char *p);
extern void func_ov006_020cfc58(char *p);

extern void *data_ov006_0214097c[];
extern s32 data_ov006_0213b2fc;
extern s32 data_ov006_0213b300;
extern s32 data_ov006_0213b2f8;
extern s32 data_ov006_0213b304;

typedef s32 (*IsActiveFn)(void *);
typedef struct Vec3 *(*GetVecFn)(void *);

void func_ov006_020d01e0(short *g, short *p1, short *p2)
{
    char *c = (char *)g;
    struct Vec3 va, vb;
    struct Pair bestP;
    struct Pair curP;
    struct Vec3 tv[7];
    short tx, tz;
    Fix12i dist, half, rowStep;
    s32 j, i;
    s32 flag;
    struct Vec3 *sl, *sb;
    s32 bestVal, bestD1;
    void *bestObj;
    s32 k;
    void *obj;
    struct Vec3 *pos, *dir;
    s32 d1, d2, d3;
    s32 absC, blendVal, ratio;

    *(s16 *)(c + 0x50) = p1[0];
    *(s16 *)(c + 0x52) = p1[1];
    *(s16 *)(c + 0x54) = p2[0];
    *(s16 *)(c + 0x56) = p2[1];

    p1[0] = (s16)(p1[0] - 0x80);
    p1[1] = (s16)(0 - p1[1]);
    p2[0] = (s16)(p2[0] - 0x80);
    p2[1] = (s16)(0 - p2[1]);

    if ((s32)*(volatile s16 *)&p1[0] > (s32)*(volatile s16 *)&p2[0]) {
        tx = p1[0];
        tz = p1[1];
        p1[0] = p2[0];
        p1[1] = p2[1];
        p2[0] = tx;
        p2[1] = tz;
    }

    {
        s32 ax = p1[0];
        s32 ay = p1[1];
        va.x = ax << 12;
        va.y = ay << 12;
        va.z = 0;
    }
    {
        s32 bx = p2[0];
        s32 by = p2[1];
        vb.x = bx << 12;
        vb.y = by << 12;
        vb.z = 0;
    }

    Vec3_Add(&tv[0], &va, &vb);
    Vec3_MulScalar(&tv[1], &tv[0], 0x800);
    *(s32 *)(c + 0x8) = tv[1].x;
    *(s32 *)(c + 0xC) = tv[1].y;
    *(s32 *)(c + 0x10) = tv[1].z;

    Vec3_Sub(&tv[2], &vb, &va);
    *(s32 *)(c + 0x20) = tv[2].x;
    *(s32 *)(c + 0x24) = tv[2].y;
    *(s32 *)(c + 0x28) = tv[2].z;

    func_0203ce80(&tv[3], (struct Vec3 *)(c + 0x20));

    tv[4].x = 0;
    tv[4].y = 0;
    tv[4].z = 0x1000;
    func_0203cf00(&tv[5], (struct Vec3 *)(c + 0x20), &tv[4]);
    *(s32 *)(c + 0x14) = tv[5].x;
    *(s32 *)(c + 0x18) = tv[5].y;
    *(s32 *)(c + 0x1C) = tv[5].z;

    if (*(s32 *)(c + 0x18) < 0) {
        *(s32 *)(c + 0x14) = 0 - *(s32 *)(c + 0x14);
        *(s32 *)(c + 0x18) = 0 - *(s32 *)(c + 0x18);
    }

    *(s32 *)(c + 0x38) = 0;
    *(s32 *)(c + 0x3C) = 0;
    *(s32 *)(c + 0x40) = 0;
    *(s32 *)(c + 0x44) = 0 - *(s32 *)(c + 0x14);
    *(s32 *)(c + 0x48) = 0 - *(s32 *)(c + 0x18);
    *(s32 *)(c + 0x4C) = 0 - *(s32 *)(c + 0x1C);
    Vec3_MulScalarInPlace((s32 *)(c + 0x44), 0x800);

    SubVec3(&va, (struct Vec3 *)(c + 8), &va);
    SubVec3(&vb, (struct Vec3 *)(c + 8), &vb);
    dist = Vec3_Dist(&va, &vb);

    half = dist >> 1;
    rowStep = dist / 3;
    *(s32 *)(c + 0x58) = half;

    {
        s32 rowFrac, colFrac, colWeight;
        s32 rowAbs, colAbs;
        s32 f1, f2, f3, ip, lr, w, w2;
        s32 accum;
        s64 p1a, p2a;

        j = 0;
        accum = 0;
        rowFrac = -0x180;
        sl = (struct Vec3 *)(c + 0x5C);
        sb = (struct Vec3 *)(c + 0x11C);
        do {
            colFrac = -0x180;
            colWeight = 0;
            i = 0;
            do {
                colAbs = (colFrac < 0) ? (0 - colFrac) : colFrac;
                f1 = 0x1000 - (0x180 - colAbs);
                f2 = 0x1000 - colWeight;
                rowAbs = (rowFrac < 0) ? (0 - rowFrac) : rowFrac;
                f3 = 0x1000 - (0x180 - rowAbs);
                flag = 0;

                ip = (s32)(((s64)f1 * f3 + 0x800) >> 12);
                p1a = (s64)f2 * ip;
                p2a = (s64)ip * ip;
                w = (s32)(((s64)colWeight * ip + 0x800) >> 12);

                sl->x = (s32)(((s64)vb.x * w + 0x800) >> 12);
                lr = (s32)((p1a + 0x800) >> 12);
                colWeight += 0x555;
                sl->x += (s32)(((s64)va.x * lr + 0x800) >> 12);
                sl->y = (s32)(((s64)vb.y * w + 0x800) >> 12);
                sl->y += (s32)(((s64)va.y * lr + 0x800) >> 12);

                i++;
                w2 = (s32)((p2a + 0x800) >> 12);
                colFrac += 0x100;
                sl->z = accum - *(s32 *)(c + 0x58);
                sl->z = (s32)(((s64)sl->z * w2 + 0x800) >> 12);

                sb->x = sl->x;
                sb->y = sl->y;
                sb->z = sl->z;

                sl++;
                sb++;
            } while (i < 4);
            j++;
            accum += rowStep;
            rowFrac += 0x100;
        } while (j < 4);
    }

    *(s16 *)(c + 0x320) = (s16)flag;
    *(s16 *)(c + 0x322) = 0x1400;
    *(u8 *)(c + 0x329) = 0x1F;
    func_ov006_020cf040(c, c + 0x38, c + 0x44);
    func_ov006_020cf124(c);

    *(s32 *)(c + 0x2C) = 0x1000;
    *(s32 *)(c + 0x30) = 0x1000;
    *(s32 *)(c + 0x34) = 0x1000;

    bestObj = 0;
    bestVal = 0;
    bestD1 = 0;
    bestP.a.x = 0;
    bestP.a.y = 0;
    bestP.a.z = 0;
    bestP.b.x = 0;
    bestP.b.y = 0;
    bestP.b.z = 0;

    k = 0;
    do {
        obj = data_ov006_0214097c[k];
        if (obj != 0) {
            if (((IsActiveFn)((*(void ***)obj))[3])(obj) != 0) {
                obj = data_ov006_0214097c[k];
                pos = ((GetVecFn)((*(void ***)obj))[0])(obj);
                curP.a.x = pos->x;
                curP.a.y = pos->y;
                curP.a.z = pos->z;
                obj = data_ov006_0214097c[k];
                dir = ((GetVecFn)((*(void ***)obj))[2])(obj);
                curP.b.x = dir->x;
                curP.b.y = dir->y;
                curP.b.z = dir->z;

                SubVec3(&curP.a, (struct Vec3 *)(c + 8), &curP.a);
                d1 = DotVec3((struct Vec3 *)(c + 0x20), &curP.a);
                d2 = DotVec3((struct Vec3 *)(c + 0x14), &curP.a);
                d3 = DotVec3((struct Vec3 *)(c + 0x14), &curP.b);
                DotVec3((struct Vec3 *)(c + 0x20), &curP.b);

                if (d3 < 0x100 && d2 <= bestVal && d2 > -0x24000) {
                    s32 h = *(s32 *)(c + 0x58);
                    if (d1 > -h && d1 < h) {
                        bestP = curP;
                        bestObj = obj;
                        bestVal = d2;
                        bestD1 = d1;
                    }
                }
            }
        }
        k++;
    } while (k < 5);

    if (bestObj != 0) {
        *(s32 *)(c + 0x38) = bestP.a.x;
        *(s32 *)(c + 0x3C) = bestP.a.y;
        *(s32 *)(c + 0x40) = bestP.a.z;
        *(s32 *)(c + 0x44) = bestP.b.x;
        *(s32 *)(c + 0x48) = bestP.b.y;
        *(s32 *)(c + 0x4C) = bestP.b.z;

        absC = (bestD1 < 0) ? (0 - bestD1) : bestD1;
        ratio = _ZN4cstd4fdivEii(absC, *(s32 *)(c + 0x58));
        absC = (bestD1 < 0) ? (0 - bestD1) : bestD1;
        blendVal = (s32)(((s64)absC * *(s32 *)(c + 0x58) + 0x800) >> 12);
        if (blendVal < 0x400000) {
            *(s16 *)((char *)bestObj + 0x18) = 2;
            func_ov006_020e6db4(0x1B1, *(s32 *)(c + 8),
                (s32)(data_ov006_0213b2fc * ratio + data_ov006_0213b300 * (0x1000 - ratio)) >> 0xC);
        } else {
            *(s16 *)((char *)bestObj + 0x18) = 1;
            func_ov006_020e6db4(0x1AE, *(s32 *)(c + 8),
                (s32)(data_ov006_0213b2f8 * ratio + data_ov006_0213b304 * (0x1000 - ratio)) >> 0xC);
        }
        *(s32 *)((char *)bestObj + 0x4) = *(s32 *)(c + 0x14);
        *(s32 *)((char *)bestObj + 0x8) = *(s32 *)(c + 0x18);
        *(s32 *)((char *)bestObj + 0xC) = *(s32 *)(c + 0x1C);
        *(s32 *)((char *)bestObj + 0x10) = bestD1;
        *(s32 *)((char *)bestObj + 0x14) = *(s32 *)(c + 0x58);
        *(u8 *)(c + 0x328) = 3;
        func_ov006_020cf040(c, c + 0x38, c + 0x44);
        func_ov006_020cfa28(c);
        return;
    }

    *(s16 *)(c + 0x31C) = 0x258;
    *(u8 *)(c + 0x328) = 1;
    func_ov006_020cfc58(c);
}
