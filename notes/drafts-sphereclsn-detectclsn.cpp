//cpp
// NONMATCHING: size 0x1bc8.  mismatches 253/1778, ratio 0.8341.
//              1778 instructions and a 0x1b4 frame, both exactly the ROM's, and
//              EVERY call-gap is now exactly the ROM's length.
//              The instruction MULTISET is surplus 3 / deficit 3 across the whole
//              function, and one of those three pairs is literal-pool data that
//              capstone renders as `andeq` -- so the code is two instructions from
//              exact and the rest is register and stack-slot allocation.
//
// 2026-08-20.  Session went 1493 -> 302 mismatching words.  Every gain is a
// SPELLING; none changes what the function computes.
//
// ===================== READ THIS BEFORE MEASURING ANYTHING ==================
//
// FOUR METRICS.  They disagree, and each disagreement this session was a real
// finding rather than noise.
//
//   equal     difflib LCS.  Can RISE when instructions are DELETED, and can also
//             miss a +290 gain because it was already sliding to find the match.
//             Never rank on it.
//   anchored  the 34 call sites as fixed anchors; position-for-position inside
//             each gap.  Use while the instruction count is still wrong.
//   shape     `fdiff.shape`, registers and stack slots erased.  Answers "is the
//             CODE wrong or only the allocation".
//   strict    instruction i vs instruction i across all 1778, no slack.  Reads 0
//             unless cand == 1778.
//
// **Once cand == 1778, rank on `strict`, or just on the gate's own `mismatches`.**
// That was established the hard way: a candidate scoring +3 strict / -81 anchored
// turned out to be the better one (gate 971 vs 974), and the strict and gate
// deltas agreed exactly.  `anchored` re-aligns inside each gap, so it forgives a
// shift the byte gate does not.
//
// The order-independent view is the one that finds things: difference the two
// instruction-shape MULTISETS.  Every deficit entry is a specific missing
// instruction you can go and find.  Three of this session's gains came straight
// out of reading that list.
//
// DO NOT TREAT cand == 1778 AS AN INVARIANT WHILE ANY GAP LENGTH IS WRONG.  That
// mistake nearly cost the largest fix of the day.  The draft's earlier 1778 was a
// COINCIDENCE: the sqrt region was three words short and gap 28 was three words
// long, and they cancelled.  Check per-gap lengths (`shapegap.py`), not the total.
//
// ============================== THE GAINS ===================================
//
//   THE SQRT REGION, gaps 7/8, 11/12, 15/16 -- SOLVED, and it was the SUM
//   OPERAND ORDER, not the hoist.  mwccarm evaluates a two-term 64-bit sum
//   RIGHT TO LEFT: the `smull` takes the right addend and the `smlal`
//   accumulates the left.  The ROM emits d^2 then faceDot^2, so the source has to
//   read `fh*fh + dh*dh`.  Written the other way round, hoisting the two `>> 4`
//   shifts makes `axisDot` spill instead of staying in a callee-saved register --
//   which is exactly the failure that got the hoist rejected four times and
//   recorded as a re-anchoring artifact.  With the terms swapped, all six gaps
//   become the ROM's exact code.  Control: swap them back and it is -95 anchored.
//
//   THE THREE SURPLUS WORDS IN GAP 28 -- and with them, every remaining gap-length
//   mismatch.  Two came from the `tpv` pointer alias and one from the slab bounds.
//   Worth 666 on strict, because three surplus words displace everything after
//   them.  978 -> 302 in one step.
//
//   THE SLAB BOUNDS ARE NOT LOCALS.  The ROM materialises the upper bound LAZILY,
//   between the first and second comparison:
//       add r1,r6,r3 / rsb r2,r1,#0 / cmp r0,r2 / blt reject / sub r1,r6,r3 / cmp
//   A named local cannot produce that split -- it forces both to be emitted
//   together.  Lazy split materialisation is the signature of a CSE'd repeated
//   expression, so both bounds are written out in full at all six comparison
//   sites.  Two earlier tests here (move t/u later; give them their own lo/hi
//   locals) both used named locals and so could not find this.
//
//   THE VORONOI DISPATCH BRANCH POLARITY, +2 and gap 6 to exact.  Counter-
//   intuitive: writing `if (dotN > dotM) goto featN; goto feat3;` is what makes
//   mwcc emit the ROM's `ble`.  mwcc inverts, so the `<=` spelling produces `bgt`.
//   `ble`/`bgt` counts go 38/17 -> 40/15, exactly the ROM's.
//
//   `sphere.flags & 0x20` re-read through a CONST reference cast, three sites.
//   The ROM reads sphere.flags eight times and we read five.  What defeats the
//   CSE is the CV-QUALIFIER CHANGE, not volatile: const pointer cast, const
//   reference cast, volatile view and volatile reference cast all give identical
//   bytes, while `(&sphere)->flags`, a u8 temp and a rebound local pointer all
//   leave it in place.
//
//   THE PROLOGUE reads the sphere centre through a VOLATILE POINTEE.  The ROM
//   loads `c` twice for the three `raw` components; we loaded it once.
//   `(const Vector3 *)c` does nothing here -- `c` is already const, so the cast
//   changes no qualifier, and it is the qualifier change that re-issues a load.
//
//   DELETING the `en3 = f->normals[tri->edgeNormal3Idx];` re-read that was worth
//   +153 the previous session.  The recompute costs seven instructions where the
//   ROM reloads the pointer in one.  See the interaction note below.
//
//   Two selects the ROM writes as if/else where the draft wrote
//   assign-then-override.
//
// ==================== FOUR INVENTED CONSTRUCTS RETIRED ======================
// All byte-neutral or better.  The file no longer contains an untrue construct.
//
//   * `struct Vtx3 { s32 e[3]; ~Vtx3(){} }` -> `Vector3`.  include/types.h
//     already had it, with the same empty destructor declared for the same
//     reason.
//   * `volatile s16 cr[3]` read through a cast -> `Vector3s cr` read as
//     `cr.x/.y/.z`, no cast anywhere.  For a LOCAL AGGREGATE the lever is not the
//     cv-qualifier at all, it is NON-POD-NESS -- the same thing that stops
//     mwccarm scalarizing tp/vb/vc.  Control: a plain s16 array with plain reads
//     is strict 0 and the frame blows out to 0x1dc.
//   * the `s32 *tpv` pointer alias, worth +42 and then +36 in earlier sessions.
//     The ROM has ZERO `add r,sp,#0x18c`; it reads the three slots directly.
//   * four of the six no-op `c = &sphere.pos;` re-binds.
//
// ================= THE INTERACTION IS THE POINT =============================
//
// Full 2^4 factorial on the anchored count, earlier in the session:
//
//     cr volatile -> const, alone          +0     looks like pure readability
//     delete the en3 recompute, alone    -210     looks like a catastrophe
//     both together                       +71
//     both + the flags re-read            +84
//
// A change that measures as a NO-OP can be the enabler for one that measures as a
// disaster, and no greedy one-at-a-time sweep can find that.  A swept-and-dead
// verdict here is scoped to the structure it was measured against; that rule has
// now paid nine times.  Do not trust any "dead" note below without re-running it.
//
// ======================== WHAT IS LEFT ======================================
//
// Whole-function instruction multiset, order-independent:
//
//     +1 add r,r,#0x3c      -1 ldr r,[sp]        the centre re-bind recomputes
//                                                &sphere.pos where the ROM reloads
//     -1 add r,sp,#0x16c    +1 str r,[sp]        cr's address, 14 against 15
//     -1 andeq ... +1 andeq                      literal-pool data, not code
//
// Two real instructions.  Everything else is allocation:
//
//     gap 28  0x01ffcda4  151 insn   norm-miss 135   shape-miss 39
//     gap  0  0x01ffb830  364 insn   norm-miss  59   shape-miss 10
//     gap  6  0x01ffbea0  218 insn   norm-miss  30   shape-miss  0
//     gap 24  0x01ffc910  192 insn   norm-miss  14   shape-miss  0
//     everything else                norm-miss 0-10, shape-miss 0-1
//
// ========= THE REMAINING DEFECT, DECOMPOSED AND LARGELY CLOSED OFF ==========
//
// All 253 remaining mismatches by KIND (position-for-position):
//
//     exact 1490 | slot-number-only 136 | register-name-only 32 | both 3 |
//     shape or order 83
//
// So 136 of 253 -- over half -- are positions where the instruction is RIGHT and
// only the stack offset differs.  Our frame is a PERMUTATION of the ROM's at the
// same 0x1b4 size: 99 distinct slots each side, 80 of 103 carrying identical
// traffic, and the differences form a near-perfect bijection over 28 pairs.
// Four slots are displaced by exactly three words:
//
//     ROM  0xc8 0xd0 0xd8 0xe0   (stride 8)
//     cand 0xd4 0xdc 0xe4 0xec   each +0xc
//
// and the three busiest slots are relocated too -- the ROM's 0x94 / 0x98 / 0x9c
// carry 22 / 11 / 14 references where ours carry 2 / 3 / 3.  WHICH variables those
// are is NOT established; do not assume, identify them structurally.
//
// THERE ARE TWO SLOT POOLS, AND DECLARATION ORDER ONLY CONTROLS ONE OF THEM.
// Swapping each adjacent pair of declarations and diffing the slot traffic gives
// the declaration -> slot map directly.  61 of the 91 adjacent swaps are
// BYTE-NEUTRAL; the 30 that are not all move slots in 0xc..0xb8 or the four
// Vector3s at 0x180+.  Those are the function-wide locals, and for them mwccarm
// really does assign in declaration order:
//
//     33 rawX -> 0x94   34 rawY -> 0x98   35 rawZ -> 0x9c   80 depth -> 0xb0
//     81 fn   -> 0xb4   21 rsq  -> 0x60   30 leaf -> 0x88   31 tri   -> 0x8c
//
// The hot loop-local spills are NOT in that pool.  IDENTIFY THEM STRUCTURALLY
// BEFORE MEASURING: en3 is the store that follows the `tri->edgeNormal3Idx` load
// at tri+0xc, which puts it at **0x104 in our build and 0x110 in the ROM** -- a
// -0xc displacement like the others.  (Pairing "the busiest slot in each stream"
// instead gives 0xc4 / 0x94, which is wrong, and an earlier session's note that
// the ROM's 0x94 is en3 is also wrong.  That error was committed here once.)
//
// With en3 identified properly, two axes are exhausted and both are inert:
//
//   * DECLARATION POSITION.  Moved to every one of the 91 other positions, en3's
//     slot is 0x104 in all 91.  It never reaches 0x110.  Moving en3, fn and depth
//     together to where the raw* trio sits -- all 18 distinct orderings, verified
//     distinct by hash -- costs two instructions and helps nothing.
//   * LIVE RANGE.  Writing it later, splitting it via a copy, scoping it to the
//     prism loop, scoping the whole en1/en2/en3 group, and hoisting `f->normals`
//     to a shared base are ALL BYTE-IDENTICAL, and en3 stays at 0x104 in every
//     one.  mwccarm normalises all five away.
//
// So the 136 slot-only mismatches are not reachable from declaration order, and
// not from any live-range reshaping tried so far.  I do NOT have a demonstrated
// mechanism for moving them; treat "change the live range" as an untested guess
// rather than a plan.  The one framing that is at least concrete: our en3 sits
// 0xc BELOW the ROM's, so the ROM has three more words allocated ahead of it in
// whatever order the allocator uses -- it spills something we keep in a register.
// Finding what would be the next real lead.
//
// This is also the precise sense in which "declaration order IS the stack layout"
// is true for the 0x498 dBgCh_Gnd twin and false here: the twin has only the
// function-wide pool.
//
// ============ RE-SWEPT AND AT A LOCAL OPTIMUM ON THIS STRUCTURE =============
//   * declaration order at full resolution: 93 movable units, 8,464 candidates,
//     ZERO improving moves.  (An earlier run of this reported "converged" after
//     seeing only 39 of the 93 -- its block detector stopped at the first comment
//     line.  If you re-run it, check the unit count it prints.)
//   * tpv subsets x re-bind sites, the full 64 x 64 product, plus 1,536 more with
//     the slab-bound forms crossed in.
//   * cr representation, nine spellings -- all byte-identical.  Saturated.
//   * the centre pointer's declaration: const / const volatile / volatile pointee
//     x prologue spelling x re-binds.
//   * branch polarity in the dispatch, all 64.
//   * the sqrt region: 2,922 scored candidates over hoist mode, placement,
//     declaration form and order, storage class, reusing eight pairs of existing
//     scratch locals, block vs function scope, and twelve spellings of the 64-bit
//     store.  Ten of the twelve store spellings are byte-identical -- the ROM's
//     `mov r2,r3` is EMERGENT once the shifts hoist, not a source construct.
//     Leave SqrtRaw alone.
//   * multiply operand order in AXIS_DOT0 / EDGENORMAL_DOT, all 8.
//   * declaration PAIR SWAPS -- 2,000 of 4,278 scored before the run was killed,
//     none better.  Combined with the converged single-move sweep and the 91
//     adjacent swaps, the declaration axis is closed three ways.
//   * moving en3 alone to all 91 other positions: byte-neutral or worse in every
//     one, and its slot never changes.
//
// ============ MEASURED AND DEAD, INCLUDING OUT-OF-FUNCTION ==================
//   * A PRAGMA.  The byte-matched sibling that wraps this function carries
//     `#pragma opt_common_subs off` and calls it "original and load-bearing", and
//     158 files in the tree use it.  Here it costs 140 INSTRUCTIONS and 1120
//     anchored.  opt_propagation / opt_dead_assignments / opt_lifetimes /
//     optimize_for_size all negative; five others inert.  **This TU was compiled
//     with CSE ON**, which is what justifies the per-site approach.
//   * A HEADER RETYPE.  include/dBgW_Kc.h says 0x28..0x30 are ONE Vector3 left as
//     three scalars "only because retyping it would touch already-matched
//     callers".  Reading them through a Vector3 view is byte-identical: the
//     retype is free but buys nothing here.
//   * THE VERTEX TAIL AS A LOOP.  mwcc unrolls it and then CSEs the address
//     anyway, six instructions short.
//   * The alternative (cross, denominator) pairings of the KCL vertex formula:
//     all 36 non-degenerate ones swept.  Several score higher and every one of
//     those is degenerate (cross(n,X) . X == 0) or reuses a denominator, i.e.
//     wins by removing arithmetic rather than by being right.  Do not take them.
//
// PROVENANCE. Restored 2026-08-19 from nearmiss/db.jsonl, attempt
// 8273344dc1434a9e86882b88eebf7ffa.  The file that lived here before was the
// WORSE, earlier draft, whose banner falsely claimed the walk and prism tests
// were stubs.  Do not reinstate it.
//
// SCORING.  Use the flags that BUILD the ROM: `rombuild.CFLAGS` with the language
// swapped carries `-Cpp_exceptions off` where `swarm.CPP_FLAGS` carries
// `-w illpragmas`.  Across 550 enrolled //cpp files the two are byte-identical;
// on this draft they differ by one instruction, in the build's favour.
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
   fdiv's Fix12 result reconciled with the 0x400 normal scale.

   `cr` being a Vector3s rather than an `s16[3]` is load-bearing and is why nothing
   here needs a cast -- see its declaration. */
#define KCL_VERTEX(out, ea, o0, o1, o2)                                                   \
    cr.x = (s16)(MUL10(fn[1], (ea)[2]) - MUL10(fn[2], (ea)[1]));             \
    cr.y = (s16)(MUL10(fn[2], (ea)[0]) - MUL10(fn[0], (ea)[2]));             \
    cr.z = (s16)(MUL10(fn[0], (ea)[1]) - MUL10(fn[1], (ea)[0]));             \
    t = MUL10(cr.x, en3[0]) + MUL10(cr.y, en3[1]) + MUL10(cr.z, en3[2]);   \
    if (func_020397dc(t)) continue;                                           \
    u = cstd::fdiv(tri->length, t) >> 2;                                \
    (out)[0] = (o0) + (s32)(((s64)cr.x * u) >> 14);                         \
    (out)[1] = (o1) + (s32)(((s64)cr.y * u) >> 14);                         \
    (out)[2] = (o2) + (s32)(((s64)cr.z * u) >> 14);

/* Post-subtraction form: components already relative to the centre. */
#define AXIS_DOT0(v) (FX12(unk_28, (v).x)                             \
                    + FX12(unk_30, (v).z)                             \
                    + FX12(unk_2c, (v).y))

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
        s32 dh;                                                           \
        s32 fh;                                                           \
        if (cls != 0) continue;                                           \
        if (((const dBgCh_SphCrr &)sphere).flags & 0x20) continue;        \
        dh = (d) >> 4;                                                    \
        fh = faceDot >> 4;                                                \
        axisDot = DotVec3((const s32 *)&sn, (const Vector3 *)&unk_28);    \
        hyp = SqrtRaw((u64)((s64)fh * fh + (s64)dh * dh), zval, k1);      \
        if (func_020397dc(hyp)) continue;                                 \
        if (axisDot > cstd::fdiv(fh, hyp)) continue;                      \
    }                                                                         \
    goto dsqL;                                                                \
    armA:                                                                     \
    if (cls == 1) {                                                           \
        if ((d) > faceDot) continue;                                          \
    } else if ((d) > (faceDot >> unk_48)) continue;                           \
    dsqL:;

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
    /* volatile POINTEE, not a volatile pointer.  The ROM loads `c` from its stack
       home before almost every component read -- twice for the three `raw` reads in
       the prologue alone -- and mwcc will otherwise keep it in a register and read
       through it once.  `const volatile` is worth 290 instructions back onto their
       correct index and is what brings the count to exactly 1778.  A plain
       `(const Vector3 *)c` cast does nothing here: `c` is already const, so it
       changes no qualifier, and it is the QUALIFIER CHANGE that re-issues a load.
       Making the POINTER volatile instead (`const Vector3 *volatile c`) is -57 shape. */
    const volatile Vector3 *c;
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
    /* The cross product of two normals, and therefore a vector -- not a scratch
       array.  It has to be a NON-POD type: as a plain three-element s16 array with
       plain reads, mwccarm scalarizes it and shares the loads, which is strict 0,
       anchored -676, and a frame of 0x1dc instead of 0x1b4.  Vector3s carries the
       same empty destructor Vector3 does, for the same reason, and fixes it with no
       cast anywhere -- the `(const s16 *)` view this used to need was treating the
       symptom. */
    Vector3s cr;
    s32 nrm[3];
    s32 depth;
    s16 *fn;
    s32 den12;
    s32 den23;
    s32 den31;
    s16 *en1;
    s16 *en2;
    s16 *en3;
    Vector3 sn;
    /* These three MUST be a type with a user-declared destructor.  mwccarm
       scalarizes a plain `s32[3]` local into loose stack slots; the ROM keeps all
       three as three-word objects, and the empty `~Vector3()` in include/types.h is
       what blocks the scalarization.  It is the difference between a 0x1bc frame with
       105 slots and the ROM's 0x1b4 with 102.  Do not "simplify" this to s32[3]. */
    Vector3 tp;
    Vector3 vb;
    Vector3 vc;

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
                        if (cls == 1) passArg = k1; else passArg = k0;
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
                            if (dot1 > dot3) goto feat1;
                            goto feat3;
                        } else {
                            if (dot2 > dot3) goto feat2;
                            goto feat3;
                        }
                    feat1:
                        {
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
                    feat2:
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
                        if (sphere.flags & 0x40) v = z154; else v = k1;
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

                            tp.x = vtx[0] << 6;
                            tp.y = vtx[1] << 6;
                            tp.z = vtx[2] << 6;
                            KCL_VERTEX(&vb.x, en2, tp.x, tp.y, tp.z)
                            /* There used to be an `en3 = f->normals[tri->edgeNormal3Idx];`
                               re-read here, worth +153 when `cr` was volatile.  It is
                               deleted: the ROM reloads the POINTER from its home in one
                               instruction (`ldr r0,[sp,#0x98]`) and the recompute costs
                               seven.  With `cr` read through a const view instead, mwcc
                               no longer shares the en3 loads across the two rounds and
                               the recompute is pure cost -- +71 anchored to remove it,
                               and gap 26 went from 60 missed instructions to 4.
                               Do not put it back without re-measuring BOTH. */
                            KCL_VERTEX(&vc.x, en1, tp.x, tp.y, tp.z)

                            {
                            s32 da, db, dc;
                            /* `c` being a volatile pointee already forces most of the
                               reloads the ROM makes (ldr [sp,#0xc4], nine times in this
                               block).  These two re-binds are what is left: without them
                               mwcc folds `c` back to `sphere.pos` and addresses the
                               component straight off the object.  Each is a no-op --
                               `c` already holds &sphere.pos.  All 64 site subsets were
                               swept individually; four of the original six are
                               unnecessary and are gone.  One fold survives even so:
                               `vc.z -= c->z` still compiles to `ldr r8,[fp,#0x44]`
                               where the ROM goes through the pointer. */
                            tp.x -= c->x;
                            tp.y -= c->y;
                            tp.z -= c->z;
                            da = AXIS_DOT0(tp);
                            vb.x -= c->x; c = &sphere.pos;
                            vb.y -= c->y;
                            vb.z -= c->z;
                            db = AXIS_DOT0(vb);
                            vc.x -= c->x; c = &sphere.pos;
                            vc.y -= c->y;
                            vc.z -= c->z;
                            dc = AXIS_DOT0(vc);
                            if (da >= -(sphere.unk_0ec + sphere.radius) && da <= sphere.unk_0ec - sphere.radius
                             && db >= -(sphere.unk_0ec + sphere.radius) && db <= sphere.unk_0ec - sphere.radius
                             && dc >= -(sphere.unk_0ec + sphere.radius) && dc <= sphere.unk_0ec - sphere.radius)
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
