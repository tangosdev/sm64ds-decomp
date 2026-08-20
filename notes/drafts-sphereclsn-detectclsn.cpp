//cpp
// NONMATCHING: size 0x1bc8 -- INSTRUCTION COUNT EXACT.  align equal=1374 ratio=0.773
//              Frame exact too: sub sp,#0x1b4 and 102 stack slots, both matching
//
// 2026-08-20, second session (equal 1047 -> 1335).  Every gain below is a
// SPELLING: each one re-reads a value the candidate already had in a register,
// because the ROM does not share it.  None changes what the function computes.
//
//   +153 `en3 = f->normals[tri->edgeNormal3Idx];` between the two KCL_VERTEX
//        rounds.  mwcc was CSE-ing the three en3[i] loads across both rounds;
//        the ROM reloads them.  One line.  It took the alignment's `delete`
//        count from 100 to 21, so it is a structural fix, not an alignment
//        accident -- confirmed independently below.
//   +56  the sphere centre re-bound per COMPONENT in the slab block, TOGETHER
//        with reading `cr` through a `(volatile s16 *)` view in KCL_VERTEX.
//        Neither alone helps: separately they are -27 and -64.
//   +6   six branch-polarity flips in the Voronoi dispatch -- `if (MUL10(nn,
//        dotA) > dotB) goto edgeN; goto vXX;` instead of the `<=` form.  The
//        ROM branches INTO the vertex block; we branched into the edge block.
//        Exactly +1 each, independent and additive (all 64 subsets swept).
//   +42  the prism origin read through `tpv` at five of six vertex-tail sites.
//        This one came from the permuter and is documented at its site.
//   +36  the `tpv` alias RE-SWEPT after the frame fix: round 1 now reads the
//        array directly and only round 2's [0] and [1] go through the pointer.
//        The slot map is what pointed at it -- the ROM's three `tp` slots are
//        (2 loads, 2 stores, 0 address-takes) and ours were (1,2,5), i.e. the
//        ROM never takes tp's address at all.  Sixth time a swept lever came
//        back after a structural change.  This is what took `cand` to 1778.
//   +29  THE FRAME LEVER (measured on the anchored count, not on `equal` --
//        see below).  `tp`/`vb`/`vc` are `Vtx3`, an aggregate with a
//        user-declared empty destructor, because mwccarm was SCALARIZING vb
//        and vc into loose stack slots where the ROM keeps them as arrays.
//        Frame goes 0x1bc -> 0x1b4 and 105 slots -> 102, both exactly the
//        ROM's, and the load deficit reaches zero.  Found by diffing the two
//        frames slot by slot: the ROM had nine slots with two stores each
//        that we did not, and we had eight loose one-load/one-store slots
//        that it did not.  The destructor is what blocks the scalarization --
//        the same aggregate WITHOUT it is byte-identical to the old draft.
//   +31  TWO declaration moves -- `rawX/rawY/rawZ` and `den12/den23/den31`.
//        Found by a greedy sweep over every declaration line x every position
//        (3,135 compiles per round, converged in three rounds).  The recorded
//        verdict this overturned was "377 compiles, ZERO improving moves",
//        measured before the four edits above changed the allocator's input.
//        Fifth time the re-test rule has paid on this function.
//
// HOW THE GAINS WERE CHECKED.  `equal` comes out of a difflib alignment, which
// can re-anchor: deleting instructions can raise it without anything getting
// closer.  So every step was also scored on an alignment-free metric -- the 34
// call sites are fixed anchors both streams agree on, and inside each gap
// instruction i is compared to instruction i with no slack at all:
//
//     session start   equal 1047   anchored 496/1778 (0.2790)   cand 1734
//     after +153      equal 1200   anchored 558/1778 (0.3138)   cand 1745
//     after +56/+6    equal 1262   anchored 574/1778 (0.3228)   cand 1752
//     after +42       equal 1304   anchored 629/1778 (0.3538)   cand 1777
//     after +31       equal 1335   anchored 656/1778 (0.3690)   cand 1777
//     after the frame equal 1338   anchored 685/1778 (0.3853)   cand 1780
//     now             equal 1374   anchored 765/1778 (0.4303)   cand 1778
//
// The last row is why the anchored metric is kept.  The frame fix moved
// `equal` by THREE and the anchored count by TWENTY-NINE; on `equal` alone it
// would have looked like noise and been discarded.
//
// Both metrics move together at every step.  The tool is scratchpad
// `anchored.py`; it is worth rebuilding before believing any future gain.
//
// WHAT IS LEFT.  The structure is done: 27 instructions of drift summed over
// all 35 call-gap regions, one instruction short overall, and the load deficit
// that drove everything above is down to 2.  What remains is allocation, and
// 340 of the 544 `replace`s are one thing, THE STACK FRAME: `str r0,[sp,#0xc4]`
// against `str r0,[sp,#0x104]`, the same instruction on a different slot.
// Declaration order is the lever for that and it is now EXHAUSTED -- the greedy
// sweep converged, and re-running the other three axes on the new structure
// (186 compiles: 64 tpv subsets, 64 polarity subsets, 58 re-read sites) moved
// nothing.  The next lever on the frame is not a source reorder.
//
// Swept and dead ON THIS STRUCTURE (re-run them after any structural change --
// that rule has now paid four times):
//   * 58 redundant re-read insertions for fn/en1/en2/en3/vtx/tri/c at every
//     use site: all byte-neutral or worse.  The en3 win above is not one of a
//     family; it is the only site where the CSE spanned two rounds.
//   * 32-way product of centre-rebind granularity x cr spelling x collider-axis
//     spelling: nothing beats what is here.
//   * all 36 non-degenerate (cross, denominator) pairings of the KCL vertex
//     formula.  The draft's is the documented Mario Kart Wii form and stays.
//     Several alternatives score higher; every one of them is degenerate
//     (cross(n,X) . X == 0) or reuses a denominator, i.e. they win by removing
//     arithmetic, not by being right.  Do not take them.
//
// PROVENANCE. Restored 2026-08-19 from nearmiss/db.jsonl, attempt
// 8273344dc1434a9e86882b88eebf7ffa (divergences 1213, parent 1332).
// This body was banked but never committed back to notes/, so the file that lived
// here was the WORSE, earlier draft. Both were re-scored on one worktree at
// 2004/b56 before the swap:
//
//     this body          cand=1750  equal=565  ratio=0.3203  delete=159  (28 insn short)
//     what it replaced   cand=1725  equal=366  ratio=0.2090  delete=182  (53 insn short)
//
// The replaced file's banner claimed "first draft -- head/AABB only, the walk and
// prism tests are stubs". That was false: it was 618 lines with every mechanism
// written. Do not reinstate it.
//
// Levers already swept and DEAD (#1197, positive controls on each): nineteen
// declaration-level variants across hoist / permutation / re-scoping / folding /
// en1-en2 placement, all byte-neutral; frame-size chasing (the surplus is register
// pressure from structural difference); loop rotation (mwccarm rotates this loop
// unprompted and always did). "Declaration order IS the stack layout" holds for the
// 0x498 dBgCh_Gnd twin and NOT for this 0x1bc8 function.
//
// What is left: source-shape change that reduces simultaneous liveness across the
// prism body. This draft reloads edge normals (ldr [sp,#0xc4] three times in ten
// instructions) where the ROM keeps en1 in r5, en2 in r4 and spills only en3.
//
// Score with plain --align; --align-shape normalises away stack offsets and reads
// flat across real gains. mismatches=N/M is frozen at 999 until the sizes match.
// Map and status: notes/collision-system.md.
//
// @symbol _ZN7dBgW_Kc10DetectClsnER12dBgCh_SphCrr
/* dBgW_Kc::DetectClsn(dBgCh_SphCrr &) at 0x01ffb830 (ITCM), 0x1bc8 bytes. */
#include "dBgW_Kc.h"
#include "dBgCh_SphCrr.h"
#include "dBgPi.h"
#include "SurfaceInfo.h"   /* for the real CopyNormalTo call below */

extern "C" void func_02037a6c(dBgCh_SphCrr *self, s32 loX, s32 loY, s32 loZ,
                              s32 hiX, s32 hiY, s32 hiZ);
extern "C" s32 DotVec3(const s32 *a, const Vector3 *b);
extern "C" s16 func_020396dc(dBgW_Kc *self, KCL_Tri *tri);
/* SurfaceInfo::CopyNormalTo is declared in include/SurfaceInfo.h. */
extern "C" s32 func_02039794(s32 normalY);
extern SurfaceInfo data_020a0cec;
extern "C" void func_02037fd4(dBgPi *res, s16 triID, SurfaceInfo *info);
extern "C" void func_020379f4(dBgCh_SphCrr *self, s16 triID, SurfaceInfo *info);
extern "C" void func_020379c0(dBgCh_SphCrr *self, s16 triID, SurfaceInfo *info);
extern "C" void func_0203798c(dBgCh_SphCrr *self, s16 triID, SurfaceInfo *info);
extern "C" void func_0203794c(dBgCh_SphCrr *self, const Vector3 *n);
extern "C" int _ZN5dBgCh21ShouldPassThroughImplEPvRK4CLPSRKS_b(void *self, SurfaceInfo *info,
                                                              dBgCh_SphCrr *q, int flag);
extern "C" int func_020397dc(int x);
extern "C" int func_02037e58(unsigned int *p);
namespace cstd { int fdiv(int a, int b); }

/* The ROM inlines a RAW hardware sqrt at four sites -- NOT cstd::sqrt(u64)
   (0x0203d744), which pre-shifts `x << 2` and rounds its result `(r + 1) >> 1`.
   Neither the shift nor the rounding is present here, so this is a separate
   inline helper, and the `0` and `1` it writes are loaded from frame slots
   (sp+0x118, sp+0x10c) rather than immediates -- which is what four expansions
   of one inline function look like on this compiler. */
static inline s32 SqrtRaw(u64 x, s32 zval, s32 one)
{
    volatile u16 *ime = (volatile u16 *)0x4000208;
    u16 saved = *ime;
    *ime = (u16)zval;
    *(volatile u16 *)0x40002b0 = (u16)one;
    *(volatile u64 *)0x40002b8 = x;
    *ime;
    *ime = saved;
    while (*(volatile u16 *)0x40002b0 & 0x8000)
        ;
    return *(volatile s32 *)0x40002b4;
}

/* The scale-preserving product for the normals' 1.0 == 0x400 scale. Both
   operands stay 64-bit into the shift; letting this collapse to 32 bits is the
   documented way this function diverges. */
#define MUL10(a, b) ((s32)(((s64)(a) * (b)) >> 10))

/* nn = cos between two edge normals, at the same 0x400 scale. */
#define EDGENORMAL_DOT(a, b) \
    (MUL10((a)[0], (b)[0]) + MUL10((a)[1], (b)[1]) + MUL10((a)[2], (b)[2]))

/* The ROUNDED Fix12 multiply -- `smull` / `adds #0x800` / `adc` / `lsr #0xc` /
   `orr lsl #20`. The wall block uses this and nothing else does: the collider's
   axis is Fix12i where the edge normals are 0x400-scale, so MUL10 is wrong there
   and this is wrong everywhere else. */
#define FX12(a, b) ((s32)((((s64)(a) * (b)) + 0x800) >> 12))

/* One vertex of the triangle, recovered from the KCL prism: the prism stores a
   position, a face normal, three edge normals and a length, and the vertex is
   `pos + cross(fn, ea) * (length / dot(cross(fn, ea), en3))`. The cross goes
   through a 6-byte s16 scratch (sp+0x16c, reused by both rounds), so it really
   does truncate to 16 bits. The quotient's `>> 2` and the product's `>> 14` are
   fdiv's Fix12 result reconciled with the 0x400 normal scale. */
#define KCL_VERTEX(out, ea, o0, o1, o2)                                                   \
    cr[0] = (s16)(MUL10(fn[1], (ea)[2]) - MUL10(fn[2], (ea)[1]));             \
    cr[1] = (s16)(MUL10(fn[2], (ea)[0]) - MUL10(fn[0], (ea)[2]));             \
    cr[2] = (s16)(MUL10(fn[0], (ea)[1]) - MUL10(fn[1], (ea)[0]));             \
    t = MUL10(cr[0], en3[0]) + MUL10(cr[1], en3[1]) + MUL10(cr[2], en3[2]);   \
    if (func_020397dc(t)) continue;                                           \
    u = cstd::fdiv(tri->length, t) >> 2;                                \
    (out)[0] = (o0) + (s32)(((s64)((volatile s16 *)cr)[0] * u) >> 14);                         \
    (out)[1] = (o1) + (s32)(((s64)((volatile s16 *)cr)[1] * u) >> 14);                         \
    (out)[2] = (o2) + (s32)(((s64)((volatile s16 *)cr)[2] * u) >> 14);

/* Post-subtraction form: components already relative to the centre. */
#define AXIS_DOT0(v) (FX12((v)[0], unk_28)                                    \
                    + FX12((v)[1], unk_2c)                                    \
                    + FX12((v)[2], unk_30))

/* That vertex's offset from the sphere centre, projected on the collider axis. */
#define AXIS_DOT(v) (FX12((v)[0] - c->x, unk_28)                              \
                   + FX12((v)[1] - c->y, unk_2c)                              \
                   + FX12((v)[2] - c->z, unk_30))

/* A vertex region: the closest point is where edge normals `ea` and `eb` meet.
   `nn` is their cosine, already formed by the dispatch. Solve the 2x2 for the
   point's coefficients in that basis, then hand the offset to the vertex tail.

   `func_020397dc(den)` is the divisor guard: |den| <= 8 means the two edge
   normals are within a hair of parallel and the 2x2 is singular. The `>> 2` on
   the quotient is cstd::fdiv's Fix12 result brought back to the 0x400 scale. */
#define VERTEX_BLOCK(nn, nnh, den, ea, eb, dotI, dotJ)                        \
    den = MUL10(nn, nn) - 0x400;                                              \
    den += (nnh) - (nnh);                                                     \
    if (func_020397dc(den)) continue;                                         \
    t = cstd::fdiv(MUL10(nn, dotJ) - (dotI), den) >> 2;                  \
    u = (dotJ) - MUL10(t, nn);                                                \
    vx = MUL10(t, (ea)[0]) + MUL10(u, (eb)[0]);                               \
    vy = MUL10(t, (ea)[1]) + MUL10(u, (eb)[1]);                               \
    vz = MUL10(t, (ea)[2]) + MUL10(u, (eb)[2]);                               \
    goto vtail;

/* The filter each edge block runs before its distance is taken, and the reason
   dBgW_Kc::unk_48, unk_4d and the 0x28 vector exist. unk_48 is a SHIFT
   COUNT, not a value: the test is "is the lateral distance outside this edge
   more than faceDot >> unk_48", i.e. a slope tolerance expressed as a fraction
   of the penetration. Only a floor (cls 0) that fails it gets the expensive
   path -- a real hypotenuse through the hardware sqrt, then the contact angle
   through cstd::fdiv, compared against the collider's stored axis at +0x28.
   func_020397dc guards the divisor: |x| <= 8 means near-zero, so bail. */
#define EDGE_FILTER(d, zval, armA, dsqL)                                      \
    if (sphere.flags & 2) goto armA;                                          \
    if (cls == 1) {                                                           \
        if (func_02037e58((unsigned int *)&data_020a0cec) == 1) {             \
            if ((d) > (faceDot >> unk_48)) continue;                          \
        } else if (unk_4d) {                                                  \
            if ((d) > (faceDot >> unk_48)) continue;                          \
        } else {                                                              \
            if ((d) > faceDot) continue;                                      \
        }                                                                     \
    } else if ((d) > (faceDot >> unk_48)) {                                   \
        s32 hyp;                                                          \
        s32 axisDot;                                                      \
        if (cls != 0) continue;                                           \
        if (sphere.flags & 0x20) continue;                                \
        axisDot = DotVec3((const s32 *)&sn, (const Vector3 *)&unk_28);    \
        hyp = SqrtRaw((u64)((s64)((d) >> 4) * ((d) >> 4)                  \
                          + (s64)(faceDot >> 4) * (faceDot >> 4)), zval, k1); \
        if (func_020397dc(hyp)) continue;                                 \
        if (axisDot > cstd::fdiv(faceDot >> 4, hyp)) continue;            \
    }                                                                         \
    goto dsqL;                                                                \
    armA:                                                                     \
    if (cls == 1) {                                                           \
        if ((d) > faceDot) continue;                                          \
    } else if ((d) > (faceDot >> unk_48)) continue;                           \
    dsqL:;

/* A three-word vertex held as an AGGREGATE, not scalarized.  mwccarm splits a
   plain `s32 v[3]` local into independent stack slots; a user-declared destructor
   blocks that, which is the same lever include/types.h uses on Vector3 and the same
   one the dBgCh_Lin twin needed.  The destructor is empty: this costs nothing at
   runtime and changes nothing the function computes. */
struct Vtx3 {
    s32 e[3];
    ~Vtx3() {}
};

s32 dBgW_Kc::DetectClsn(dBgCh_SphCrr &sphere)
{
    KCL_File *f;
    s32 loX;
    s32 hiX;
    s32 loY;
    s32 hiY;
    s32 loZ;
    s32 hiZ;
    s32 loPX;
    s32 hiPX;
    s32 loPY;
    s32 hiPY;
    s32 loPZ;
    s32 hiPZ;
    s32 hitFlags;
    s32 hitFlags2;
    u16 *prev1;
    u16 *prev2;
    u16 *prev3;
    u16 *p1;
    u16 *p2;
    u16 *p3;
    s64 rsq;
    s32 stepX;
    s32 stepY;
    s32 stepZ;
    s32 s1;
    s32 s2;
    s32 s3;
    u32 y;
    u32 x;
    u16 *leaf;
    KCL_Tri *tri;
    s32 *vtx;
    s32 rawX;
    s32 rawY;
    s32 rawZ;
    s16 triID;
    s32 cls;
    s32 contactKind;
    const Vector3 *c;
    s32 d1h;
    s32 d2h;
    s32 d3h;
    s32 nn;
    s32 nnh;
    s32 rsc;
    s32 z108;
    s32 k1;
    s32 k0;
    s32 z118;
    s32 z11c;
    s32 z120;
    s32 z154;
    s32 k3;
    s32 z15c;
    s32 k2;
    s32 tlen;
    s32 passArg;
    u32 z;
    u32 shift;
    u32 *node;
    u32 idx;
    s32 word;
    s32 size;
    s32 mask;
    s32 cy;
    s32 cz;
    s64 dsq;
    s32 t;
    s32 u;
    s32 vx;
    s32 vy;
    s32 vz;
    s32 faceDot;
    s32 v;
    s32 dot1;
    s32 dot2;
    s32 dot3;
    volatile s16 cr[3];
    s32 nrm[3];
    s32 depth;
    s16 *fn;
    s32 den12;
    s32 den23;
    s32 den31;
    s16 *en1;
    s16 *en2;
    s16 *en3;
    s32 *tpv;
    Vector3 sn;
    Vtx3 tp;
    Vtx3 vb;
    Vtx3 vc;

    c = &sphere.pos;
    f = kclFile;
    {
        const Vector3 *origin = &f->origin;
        s32 rad6 = sphere.radius >> 6;

        rawX = c->x >> 6;
        rawY = c->y >> 6;
        rawZ = c->z >> 6;

        loX = (rawX - origin->x - (rad6 + 0x40)) >> 6;
        if (loX < 0) loX = 0;
        hiX = (rawX - origin->x + (rad6 + 0x40)) >> 6;
        if (hiX > (s32)~f->xMask) hiX = ~f->xMask;
        if (loX >= hiX) return 0;

        loY = (rawY - origin->y - (rad6 + 0x40)) >> 6;
        if (loY < 0) loY = 0;
        hiY = (rawY - origin->y + (rad6 + 0x40)) >> 6;
        if (hiY > (s32)~f->yMask) hiY = ~f->yMask;
        if (loY >= hiY) return 0;

        loZ = (rawZ - origin->z - (rad6 + 0x40)) >> 6;
        if (loZ < 0) loZ = 0;
        hiZ = (rawZ - origin->z + (rad6 + 0x40)) >> 6;
        if (hiZ > (s32)~f->zMask) hiZ = ~f->zMask;
        if (loZ >= hiZ) return 0;
    }

    rsc = sphere.radius << 4;

    hiPX = loPX = 0;
    hiPY = loPY = 0;
    hiPZ = loPZ = 0;
    hitFlags2 = hitFlags = 0;
    prev3 = prev2 = prev1 = 0;
    p3 = p2 = p1 = 0;

    rsq = (s64)rsc * rsc;
    k0 = loPX;
    k1 = 1;
    z118 = loPX;
    z11c = loPX;
    z120 = loPX;
    z154 = loPX;
    k3 = 3;
    z15c = loPX;
    k2 = 2;
    z108 = loPX;

    z = loZ;
    do {
        stepZ = 1000000;
        y = loY;
        do {
            stepY = 1000000;
            s1 = z108; s2 = z108; s3 = z108;
            x = loX;
            do {
                shift = f->coordShift;

                idx = (z >> shift) << f->zShift
                    | (y >> shift) << f->yShift;
                idx |= x >> shift;
                node = (u32 *)f->unk_0c;
                word = node[idx];

                while (word >= 0) {
                    node = (u32 *)((u8 *)node + word);
                    shift--;
                    word = node[((z >> shift) & 1) << 2
                              | ((y >> shift) & 1) << 1
                              | ((x >> shift) & 1)];
                }

                leaf = (u16 *)((u8 *)node + (word & 0x7fffffff));

                size = k1 << shift;
                mask = size - 1;
                stepX = size - (x & mask);
                cz = size - (z & mask);
                cy = size - (y & mask);
                if (cz < stepZ) stepZ = cz;
                if (cy < stepY) stepY = cy;

                /* Three leaves already covered by the previous row, not one:
                   the whole cell is skipped when it repeats any of them. */
                if (leaf != prev1 && leaf != prev2 && leaf != prev3) {
                    /* Keep the three widest non-empty leaves of this row, sorted
                       descending on cy -- the generalisation of the Line
                       overload's single rowLeaf/rowStep pair. */
                    if (leaf[1] != 0 && cy > s3) {
                        if (cy > s2) {
                            if (cy > s1) {
                                s3 = s2; s2 = s1; s1 = cy;
                                p3 = p2; p2 = p1; p1 = leaf;
                            } else {
                                s3 = s2; s2 = cy;
                                p3 = p2; p2 = leaf;
                            }
                        } else {
                            s3 = cy; p3 = leaf;
                        }
                    }

                    /* Same three side-plane rejects as the matched
                       DetectClsn(dBgCh_Gnd&) twin, but the sphere's tolerance
                       is its own radius (raw units x 0x400, matching the normals'
                       1.0 == 0x400 scale) where the twin uses a fixed 0x20000. */
                    while (*++leaf) {
                        tri = &f->tris[*leaf];
                        vtx = f->positions[tri->posIdx];
                        /* Three separately live edge-normal pointers, not one
                           reused cursor: the ROM keeps en1 in r5, en2 in r4 and
                           en3 spilled at sp+0x94, and step 5 reads all three
                           again after the reject chain has finished with them. */
                        /* sp+0xa8, seeded per prism at 0x01ffbe8c, set to 3 by the
                           vertex tail and defaulted to 2 by the shared tail. The
                           seed is sp+0x114, which 0x01ffba10 loads from sp+0x28
                           one instruction after 0x01ffb9c8 zeroed it -- so it is
                           the hoisted constant 0, not a value from anywhere. */
                        {
                        s32 dx = rawX - vtx[0];
                        s32 dy = rawY - vtx[1];
                        s32 dz = rawZ - vtx[2];
                        en1 = f->normals[tri->edgeNormal1Idx];
                        dot1 = dx * en1[0] + dy * en1[1] + dz * en1[2];
                        if (dot1 >= rsc) continue;
                        en2 = f->normals[tri->edgeNormal2Idx];
                        dot2 = dx * en2[0] + dy * en2[1] + dz * en2[2];
                        if (dot2 >= rsc) continue;
                        en3 = f->normals[tri->edgeNormal3Idx];
                        tlen = tri->length;
                        dot3 = dx * en3[0] + dy * en3[1] + dz * en3[2] - tlen;
                        if (dot3 >= rsc) continue;

                        fn = f->normals[tri->normalIdx];
                        faceDot = dx * fn[0] + dy * fn[1] + dz * fn[2];
                        if (faceDot > rsc) continue;
                        }

                        /* this->unk_34 / unk_35 gate the face test. These are the
                           dBgW_Kc bytes whose set/clear accessors were among
                           the original eleven ITCM matches (func_01ffb098/0a4 for
                           0x35, func_01ffb0b0/0bc for 0x34); this is what they are
                           for. unk_38 is the Vector3 func_01ffb07c writes, seeded
                           by SetFile to the unit X vector. */
                        if (unk_34) {
                            if (faceDot < -0x50000) continue;
                        } else if (unk_35) {
                            nrm[0] = fn[0] << 2;
                            nrm[1] = fn[1] << 2;
                            nrm[2] = fn[2] << 2;
                            if (DotVec3(nrm, (const Vector3 *)&unk_38) > unk_44) {
                                if (faceDot < -f->unk_10) continue;
                            } else {
                                if (faceDot < -0xa0000) continue;
                            }
                        } else {
                            if (faceDot < -f->unk_10) continue;
                        }

                        /* Accepted. The overlap along the face normal is what gets
                           accumulated; depth * normal is the pushback vector. */
                        depth = rsc - faceDot;
                        triID = func_020396dc(this, tri);

                        /* A REAL virtual call -- slot 3. Same lever as
                           dBgW_Kc::GetSurfaceInfo and the dBgCh_Gnd twin:
                           mwccarm's dispatch reads `this` from r0, a hand-rolled
                           one reads it from the callee-saved copy. */
                        GetSurfaceInfo(triID, data_020a0cec);
                        data_020a0cec.CopyNormalTo(sn);

                        /* 0 = floor, 1 = wall, 2 = underside -- and those select
                           the 0x74 / 0x9c / 0xc4 result slots and the 4 / 8 / 0x10
                           flag bits respectively. */
                        cls = func_02039794(sn.y);
                        contactKind = k0;
                        passArg = k1;
                        if (cls != 1) passArg = k0;
                        if (_ZN5dBgCh21ShouldPassThroughImplEPvRK4CLPSRKS_b(
                                this, &data_020a0cec, &sphere, passArg))
                            continue;

                        /* Voronoi region select: whichever edge the centre is
                           furthest outside decides whether this is a face, edge or
                           vertex contact. All three dots <= 0 means the centre
                           projects inside the triangle and the face distance --
                           already in `depth` -- is the answer. Otherwise the true
                           distance has to be computed against that edge or its
                           endpoints, which is what the squared radius at sp+0x60/64
                           is compared against.

                           TODO: the three symmetric edge/vertex branches. They are
                           gated by this->unk_4c and are ~1100 of the function's
                           1778 words. */
                        /* The dispatch picks the edge the centre is furthest
                           outside, then asks -- against BOTH of that edge's
                           neighbours -- whether the closest point is still on the
                           edge or has passed a vertex.

                           The six distance blocks are SHARED, which is the part
                           the earlier note got wrong: each vertex block is entered
                           from the two edges that meet at it (V12 from edge 1's
                           en2 test and edge 2's en1 test, and so on), and the
                           edge-3 dispatch is entered from both halves of the top
                           test. That sharing is why the ROM branches to labels
                           instead of falling through, and why gotos are the
                           faithful spelling here. */
                        if (dot1 > dot2) {
                            if (dot1 <= dot3) goto feat3;
                            if (dot1 <= 0) goto face;
                            if (!unk_4c) continue;
                            if (dot2 > dot3) {
                                nn = EDGENORMAL_DOT(en1, en2);
                                d1h = dot1 >> 31;
                                nnh = nn >> 31;
                                if (MUL10(nn, dot1) > dot2) goto edge1;
                                goto v12;
                            } else {
                                nn = EDGENORMAL_DOT(en1, en3);
                                d1h = dot1 >> 31;
                                nnh = nn >> 31;
                                if (MUL10(nn, dot1) > dot3) goto edge1;
                                goto v31;
                            }
                            goto edge1;
                        }
                        if (dot2 <= dot3) goto feat3;
                        if (dot2 <= 0) goto face;
                        if (!unk_4c) continue;
                        if (dot3 > dot1) {
                            nn = EDGENORMAL_DOT(en2, en3);
                            d2h = dot2 >> 31;
                            nnh = nn >> 31;
                            if (MUL10(nn, dot2) > dot3) goto edge2;
                            goto v23;
                        } else {
                            nn = EDGENORMAL_DOT(en2, en1);
                            d2h = dot2 >> 31;
                            nnh = nn >> 31;
                            if (MUL10(nn, dot2) > dot1) goto edge2;
                            goto v12;
                        }
                        goto edge2;

                    feat3:
                        if (dot3 <= 0) goto face;
                        if (!unk_4c) continue;
                        if (dot1 > dot2) {
                            nn = EDGENORMAL_DOT(en3, en1);
                            d3h = dot3 >> 31;
                            nnh = nn >> 31;
                            if (MUL10(nn, dot3) > dot1) goto edge3;
                            goto v31;
                        } else {
                            nn = EDGENORMAL_DOT(en3, en2);
                            d3h = dot3 >> 31;
                            nnh = nn >> 31;
                            if (MUL10(nn, dot3) > dot2) goto edge3;
                            goto v23;
                        }
                        goto edge3;

                    edge1:
                        EDGE_FILTER(dot1, z118, arm1_, dsq1_)
                        d1h = dot1 >> 31;
                        dsq = rsq - (s64)dot1 * dot1;
                        goto tail;

                    edge2:
                        EDGE_FILTER(dot2, z11c, arm2_, dsq2_)
                        d2h = dot2 >> 31;
                        dsq = rsq - (s64)dot2 * dot2;
                        goto tail;

                    edge3:
                        EDGE_FILTER(dot3, z120, arm3_, dsq3_)
                        d3h = dot3 >> 31;
                        dsq = rsq - (s64)dot3 * dot3;
                        goto tail;

                    v12:  VERTEX_BLOCK(nn, nnh, den12, en1, en2, dot1, dot2)
                    v23:  VERTEX_BLOCK(nn, nnh, den23, en2, en3, dot2, dot3)
                    v31:  VERTEX_BLOCK(nn, nnh, den31, en3, en1, dot3, dot1)

                    vtail:
                        v = k1;
                        if (sphere.flags & 0x40) v = z154;
                        if (!v) continue;
                        dsq = (s64)vx * vx + (s64)vy * vy + (s64)vz * vz;
                        if (faceDot < 0) continue;
                        if ((s64)faceDot * faceDot < dsq) continue;
                        dsq = rsq - dsq;
                        if (dsq <= 0) continue;
                        contactKind = k3;
                        goto tail;

                    tail:
                        depth = SqrtRaw((u64)dsq, z15c, k1) - faceDot;
                        if (depth < 0) continue;
                        if (!contactKind) contactKind = k2;

                    face:
                        if (sphere.unk_108 < sn.y) continue;

                        /* Walls only, and only once the cheap tests have accepted
                           the prism. Rebuild the triangle's three real vertices --
                           the stored position IS the first one, the other two come
                           off the two crosses -- and reject the contact when all
                           three sit inside a slab about the sphere centre measured
                           along the collider's axis.

                           The slab is NOT symmetric: the bounds are
                           -(unk_ec + radius) and (unk_ec - radius), built at
                           0x01ffcfa8 by `add r1,r6,r3` then `rsb r2,r1,#0` and
                           then re-formed by `sub r1,r6,r3`. And the sense is
                           wholly-INSIDE rejects -- the last compare is
                           `ble 0x1ffd314`, the reject -- so a triangle that
                           escapes the slab in any component keeps its hit. */
                        if (sphere.unk_0ec > 0 && cls == 1
                                && !(tri->length & 0xf0000000)) {

                            tp.e[0] = vtx[0] << 6;
                            tp.e[1] = vtx[1] << 6;
                            tp.e[2] = vtx[2] << 6;
                            /* The prism origin is read through a POINTER at five of
                               the six vertex-tail sites, not as the array.  Same
                               object, same value -- but mwcc addresses an array name
                               as sp+imm and a pointer out of a register, and the ROM
                               wants the register form here.  Worth +42 equal, and it
                               brings the instruction count to 1777 against the ROM's
                               1778.  The one exception (round 2's [1]) is what the
                               byte gate asks for -- all six through the pointer is
                               only +21 -- and is not yet understood.  Found by the
                               permuter (run 3, --stack-diffs) and then swept over
                               all 64 site subsets. */
                            tpv = tp.e;

                            KCL_VERTEX(vb.e, en2, tp.e[0], tp.e[1], tp.e[2])
                            /* The ROM re-reads the denominator normal for the
                               second round; mwcc otherwise CSEs the three en3[i]
                               loads across both rounds.  Same pointer, same value
                               -- this is a spelling, not a semantic change.
                               Worth +153 equal (1047 -> 1200), and it took the
                               alignment's `delete` count from 100 to 21. */
                            en3 = f->normals[tri->edgeNormal3Idx];
                            KCL_VERTEX(vc.e, en1, tpv[0], tpv[1], tp.e[2])

                            t = -(sphere.unk_0ec + sphere.radius);
                            u =   sphere.unk_0ec - sphere.radius;
                            {
                            s32 da, db, dc;
                            /* The ROM reloads the centre pointer from its stack home before EACH
                               component (ldr [sp,#0xc4] three times in call-gap 28); the candidate
                               loaded it once and indexed off it.  `c` already holds &sphere.pos,
                               so each re-bind is a no-op -- it only denies mwcc the CSE.  Worth
                               +56 equal, but ONLY together with the cr pointer-view reads in
                               KCL_VERTEX: alone the two are -27 and -64. */
                            tp.e[0] -= c->x; c = &sphere.pos;
                            tp.e[1] -= c->y; c = &sphere.pos;
                            tp.e[2] -= c->z;
                            da = AXIS_DOT0(tp.e);
                            vb.e[0] -= c->x; c = &sphere.pos;
                            vb.e[1] -= c->y; c = &sphere.pos;
                            vb.e[2] -= c->z;
                            db = AXIS_DOT0(vb.e);
                            vc.e[0] -= c->x; c = &sphere.pos;
                            vc.e[1] -= c->y; c = &sphere.pos;
                            vc.e[2] -= c->z;
                            dc = AXIS_DOT0(vc.e);
                            if (da >= t && da <= u
                             && db >= t && db <= u
                             && dc >= t && dc <= u)
                                continue;
                            }
                        }
                        if (!contactKind) contactKind = k1;

                        func_02037fd4((dBgPi *)&sphere.unk_010, triID, &data_020a0cec);
                        sphere.flags |= 1;

                        if (cls == 0) {
                            if (!(sphere.flags & 4)) {
                                func_020379f4(&sphere, triID, &data_020a0cec);
                                hitFlags |= 1;
                            }
                            hitFlags2 = k1;
                            sphere.flags |= 4;
                            v = (s32)(((s64)depth * sn.y) >> 14) >> 2;
                            if (v > hiPY) hiPY = v; else if (v < loPY) loPY = v;
                            if (sn.y > sphere.unk_100) func_0203794c(&sphere, &sn);
                        } else if (cls == 1) {
                            sphere.flags |= 8;
                            func_020379c0(&sphere, triID, &data_020a0cec);
                            hitFlags |= 2;
                            if (contactKind == 1) {
                                v = (s32)(((s64)depth * sn.x) >> 14) >> 2;
                                if (v > hiPX) hiPX = v; else if (v < loPX) loPX = v;
                                v = (s32)(((s64)depth * sn.z) >> 14) >> 2;
                                if (v > hiPZ) hiPZ = v; else if (v < loPZ) loPZ = v;
                            } else {
                                v = (s32)(((s64)depth * sn.x) >> 14) >> 2;
                                if (v > hiPX) hiPX = v; else if (v < loPX) loPX = v;
                                v = (s32)(((s64)depth * sn.y) >> 14) >> 2;
                                if (v > hiPY) hiPY = v; else if (v < loPY) loPY = v;
                                v = (s32)(((s64)depth * sn.z) >> 14) >> 2;
                                if (v > hiPZ) hiPZ = v; else if (v < loPZ) loPZ = v;
                            }
                        } else {
                            sphere.flags |= 0x10;
                            func_0203798c(&sphere, triID, &data_020a0cec);
                            hitFlags |= 4;
                            v = (s32)(((s64)depth * sn.x) >> 14) >> 2;
                            if (v > hiPX) hiPX = v; else if (v < loPX) loPX = v;
                            v = (s32)(((s64)depth * sn.y) >> 14) >> 2;
                            if (v > hiPY) hiPY = v; else if (v < loPY) loPY = v;
                            v = (s32)(((s64)depth * sn.z) >> 14) >> 2;
                            if (v > hiPZ) hiPZ = v; else if (v < loPZ) loPZ = v;
                        }
                    }
                }

                x += stepX;
            } while (x <= hiX);
            y += stepY;
            prev1 = p1; prev2 = p2; prev3 = p3;
        } while (y <= hiY);
        z += stepZ;
    } while (z <= hiZ);

    /* The accumulated extent goes back as two corners; the flags word is the
       return value. func_02037a6c (0x02037a6c, 0xb0) is still unnamed. */
    if (!hitFlags && !hitFlags2) goto ret0;
    func_02037a6c(&sphere, loPX, loPY, loPZ, hiPX, hiPY, hiPZ);
    return hitFlags;
ret0:
    return 0;
}
