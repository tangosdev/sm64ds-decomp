//cpp
// NONMATCHING: size 0x1bc8.  1778 instructions, exactly the ROM's, in a frame of
//              exactly the ROM's size.  809 of them are now byte-for-byte right
//              AT THE RIGHT INDEX, against 276 at the start of this session.
//
// 2026-08-20, third session.  Every gain is a SPELLING -- none changes what the
// function computes -- and two of them also delete a construct this draft had
// invented.
//
// FOUR METRICS, AND THEY DISAGREE.  That is the whole methodology here, so read
// this before trusting any number below.
//
//   equal     difflib LCS over the two instruction streams.  Can RISE when
//             instructions are deleted, so it can reward a change for making
//             the candidate shorter.
//   anchored  the 34 call sites are anchors both streams agree on; inside each
//             gap, instruction i is compared to instruction i with no slack.
//             Immune to the above, but blind to nothing else.
//   shape     `fdiff.shape`, which erases register names and stack offsets.
//             Says whether the CODE is right, ignoring allocation.
//   strict    instruction i against instruction i across all 1778, no gaps and
//             no slack.  Only meaningful once the counts agree -- which they now
//             do -- and it is the one closest to a byte match.
//
// This session they disagreed in both directions, and each disagreement was a
// real finding rather than noise:
//
//   * the Voronoi dispatch fix is +180 anchored and +2 equal.  `equal` barely
//     moved because difflib was already sliding to find those matches; what
//     changed is that they are now at the right OFFSET.
//   * the prologue fix is +290 anchored and MINUS 2 equal.
//   * the tpv subset that wins on anchored (000111, +2) costs 22 shape.
//   * hoisting the two sqrt shifts is +48 equal and -46 anchored: a pure
//     re-anchoring artifact, and it is rejected on that basis.
//
// SHAPE IS WHAT MADE THIS ROUND WORK.  Measured per call-gap at the start:
//
//     whole function   shape 1666/1778 -- the code was already 93.7% right
//     146 wrong insns  89 of them in ONE gap, number 28
//
// That turned an open-ended search into named defects.  The order-independent
// version -- the multiset of instruction shapes we emit too many or too few of
// -- is now surplus 10, deficit 10, across 1778 instructions.
//
// THE GAINS
//
//   +13  `sphere.flags & 0x20` re-read through a CONST reference cast, at the
//        three EDGE_FILTER sites.  The ROM reads sphere.flags EIGHT times; we
//        read it five, and the three missing ones are exactly these.  What
//        defeats mwcc's CSE is the CV-QUALIFIER CHANGE, not volatile: const
//        pointer cast, const reference cast, volatile view and volatile
//        reference cast all give identical bytes, while `(&sphere)->flags`, a
//        u8 temp and a rebound local pointer all leave the CSE in place.  Const
//        is the honest spelling -- it does not claim the memory changes.
//
//   +71  DELETING `en3 = f->normals[tri->edgeNormal3Idx];`, the line that was
//        worth +153 last session.  The lever reversed; this is not a retraction
//        of the old measurement.  The recompute costs seven instructions (ldr f,
//        ldr normals, ldr tri, ldrh idx, mov #6, mul, add) where the ROM reloads
//        the POINTER from its home in one, `ldr r0,[sp,#0x98]`.  Right idea,
//        wrong spelling.  Took gap 26 from 60 missed instructions to 4.
//
//   +180 THE VORONOI DISPATCH IS A REAL if/else.  Gap 6 was short exactly two
//        unconditional branches, and the shape diff put both at the top of the
//        dispatch: the ROM tests dot1 vs dot2, branches to the dot2 arm, and
//        ENDS BOTH ARMS in a branch to their own body.  The draft wrote the
//        dot1 arm as a block that falls through into the dot2 arm, so mwcc laid
//        it out the other way and dropped the two branches -- which shifted the
//        whole 218-instruction gap by two and cost 180 positions.  Writing it as
//        an if/else with the bodies behind `feat1:`/`feat2:` labels also makes
//        the three-edge symmetry visible instead of hiding it in fall-through.
//
//   +290 THE PROLOGUE READS THE CENTRE THROUGH A VOLATILE POINTEE.  Gap 0 was
//        short exactly one `ldr r,[sp]`: the ROM loads `c` TWICE for the three
//        `raw` components (y and z off the first load, x off the second) and we
//        loaded it once.  Declaring `c` as `const volatile Vector3 *` supplies
//        it.  That single instruction brought the count back to exactly 1778 --
//        and because a length mismatch had been shifting all of gap 0, it moved
//        290 instructions back onto their correct index.
//        A `(const Vector3 *)c` cast does NOT work here and should not be
//        expected to: `c` is already const, so that cast changes no qualifier.
//        It is the QUALIFIER CHANGE that re-issues a load, every time.
//
//   +5   two selects the ROM writes as if/else and the draft wrote as
//        assign-then-override -- the vtail guard (`ldreq r1,[sp,#0x10c]` /
//        `ldrne r1,[sp,#0x154]`, BOTH arms predicated) and the
//        ShouldPassThrough argument (`ldreq r3,[sp,#0x10c]` / `movne r3,r0`).
//        Found from the mix deficit, additive at +2 and +3.
//
// THREE INVENTED CONSTRUCTS RETIRED, all byte-neutral or better
//
//   * `struct Vtx3 { s32 e[3]; ~Vtx3(){} }` was this draft's own type, invented
//     to block mwccarm's scalarization of tp/vb/vc.  include/types.h already has
//     `Vector3`: same three words, same empty destructor, declared for the same
//     reason.  BYTE-IDENTICAL.  It also lets AXIS_DOT0 take the vector instead
//     of `&v.x` -- also byte-identical, and it stops the source claiming a
//     Vector3 is an array.
//
//   * `volatile s16 cr[3]` read through `((volatile s16 *)cr)[i]`, and then the
//     `const s16 *` view that replaced it.  BOTH were treating the symptom.  The
//     cross product of two normals is a VECTOR: declared `Vector3s cr;` and read
//     as `cr.x/.y/.z` with no cast anywhere, it is byte-identical to the const
//     view.  The mechanism was never the qualifier, it was NON-POD-NESS -- the
//     same lever as `Vector3 tp/vb/vc`, so the file now uses one mechanism where
//     it used two.  Control: a plain s16 array with plain reads is strict 0,
//     anchored -676, and the frame blows out to 0x1dc.  It is also what made
//     deleting the en3 recompute a +71 instead of a -210.
//
//   * four of the six `c = &sphere.pos;` no-op re-assignments in the slab block.
//     All 64 subsets were swept individually (the previous sweep only ever tried
//     all six against none, which cannot find a per-site answer); keeping only
//     the two in the vc block is identical on strict and anchored and +3 shape.
//
// THE INTERACTION IS THE POINT.  Full 2^4 factorial on the anchored count:
//
//     cr const alone                     +0      looks like pure readability
//     drop the en3 recompute alone     -210      looks like a catastrophe
//     both together                     +71
//     both + the flags re-read          +84
//
// Fourth time on this function that two levers were worthless alone and paid
// only together, and the FIRST time one of them sign-flipped another.  A
// swept-and-dead verdict here is scoped to the structure it was measured
// against; that rule has now paid eight times.  Do not trust any "dead" note
// below without re-running it.
//
// PROGRESSION (build flags -- see SCORING at the end)
//
//     session start          strict 276  anchored  820  shape 1666  cand 1778
//     + flags re-read                    anchored  833  shape 1669  cand 1781
//     + Vector3                          anchored  833  shape 1669  cand 1781
//     + cr const                         anchored  833  shape 1668  cand 1782
//     + drop en3 recompute               anchored  904  shape 1680  cand 1775
//     + AXIS_DOT0 by value               anchored  904  shape 1680  cand 1775
//     + two if/else selects              anchored  909  shape 1683  cand 1775
//     + dispatch if/else                 anchored 1089  shape 1685  cand 1777
//     + const volatile centre  strict 809  anchored 1379  shape 1684  cand 1778
//     + rebinds trimmed        strict 809  anchored 1379  shape 1687  cand 1778
//
// (strict is 0 while cand != 1778, which is why it is only quoted at the ends.)
//
// WHAT IS LEFT, by call-gap
//
//     gap 28  0x01ffcda4  151 insn   norm-miss 127   shape-miss 66
//     gap  0  0x01ffb830  364 insn   norm-miss  59   shape-miss 10
//     gap  6  0x01ffbea0  218 insn   norm-miss  32   shape-miss  2
//     gaps 8/12/16 (the three EDGE_FILTER sqrt expansions)  18 norm / 5 shape
//     everything else                                       0-14 norm / 0-2 shape
//
// One region holds half the remaining shape defect and a third of the whole
// divergence: the slab block, from the second KCL_VERTEX round through the three
// axis dot products.  Its instruction MIX is nearly exact -- surplus 7, deficit 4
// of 151 -- so what is wrong there is ORDER, i.e. mwccarm scheduling under
// register pressure, and no statement reordering tried so far reaches it.
//
// Named, understood, and not yet reachable:
//   * the ROM computes both `>> 4` halves of the hypotenuse BEFORE the DotVec3
//     call; we compute them inside the sqrt expansion after it.  Four hoist
//     spellings tried, all negative on anchored.
//   * the ROM writes the 64-bit sqrt parameter as `str / mov r2,r3 / str [r2,#4]`
//     -- a second, COPIED pointer -- where we emit two stores off one base.
//     Three spellings tried, all neutral or worse.  This is 3 of the function's
//     10 remaining deficit entries (`mov r,r`).
//   * one `vc.z -= c->z` is addressed straight off the object (`ldr r8,[fp,#0x44]`)
//     instead of through `c`; mwcc folds that one rebind away and the ROM never
//     does.  Worth 3 of the 10 deficit entries.
//
// RE-SWEPT AND AT A LOCAL OPTIMUM ON THIS EXACT STRUCTURE -- all re-run after the
// changes above, because the rule above says a stale verdict is worthless:
//   * tpv subsets, all 64.  000110 wins on anchored; 000111 is +2 anchored but
//     -22 shape, and shape is the one that is right about structure.
//   * branch polarity in the dispatch, all 64 -- the all-`>` form wins outright.
//   * cr representation, nine spellings -- s16 array or Vector3s, x member read /
//     const view / volatile view / per-component address.  ALL NINE byte-identical.
//     That axis is saturated: only non-POD-ness matters, and the count of `&cr`
//     materialisations sits at 14 against the ROM's 15 no matter what.
//   * centre re-bind sites, all 64 individually.
//   * the centre pointer's declaration: const / const volatile / volatile
//     pointee x prologue spelling x rebinds, 12 combinations.
//   * slab-bound placement -- computing them after the third dot product, where
//     the ROM does, is -220 anchored.  Giving them their own `lo`/`hi` locals
//     instead of reusing the `t`/`u` scratch is byte-identical.
//   * multiply operand order in AXIS_DOT0 / AXIS_DOT / EDGENORMAL_DOT, all 8.
//   * the four sqrt hoist spellings x three u64-store spellings, 12.
//
//
// FOUR OUT-OF-FUNCTION HYPOTHESES, ALL MEASURED AND ALL NEGATIVE.  These are the
// expensive questions -- do not re-open one without reading the number.
//
//   * A PRAGMA.  The byte-matched sibling that wraps this function,
//     `src/_ZN10dBgW_KcMbg10DetectClsnER12dBgCh_SphCrr.cpp`, carries
//     `#pragma opt_common_subs off` and calls it "original and load-bearing", and
//     the tree has 158 files using that pragma and 171 using
//     opt_strength_reduction.  Since this function's whole defect family is a CSE
//     the ROM does not make, it was the obvious hypothesis.  It is wrong, and
//     decisively: `opt_common_subs off` costs 140 INSTRUCTIONS and 1120 anchored.
//     opt_propagation, opt_dead_assignments, opt_lifetimes and optimize_for_size
//     are all negative too; opt_strength_reduction, opt_loop_invariants,
//     scheduling, opt_unroll_loops and opt_vectorize_loops are inert.
//     **This function was compiled with CSE ON.** The ROM's extra loads are
//     genuinely different source expressions, which is what justifies the
//     per-site approach.
//
//   * A HEADER RETYPE.  include/dBgW_Kc.h says `0x28..0x30 are ONE Vector3, not
//     three scalars ... left as three fields only because retyping it would touch
//     already-matched callers`, and those three are read NINE times inside the one
//     gap still structurally wrong.  Reading them through a Vector3 view instead
//     is BYTE-IDENTICAL, so the retype is free but buys nothing here.  Binding
//     them to a local pointer or reference first is -368 anchored and +8
//     instructions.  Same answer for the 0x38 vector.
//
//   * THE AUTHOR'S OWN DECLARATION STYLE.  The byte-matched twin
//     `src/_ZN7dBgW_Kc10DetectClsnER9dBgCh_Gnd.cpp` opens with
//     `KCL_File *file = kclFile; Vector3 *pos = &ray.pos;` above the C89 block, and
//     its `pos` is a PLAIN `Vector3 *`.  Hoisting our two initialised pointers the
//     same way is byte-identical (so it is adopted), but the plain pointer is not:
//     const-only and non-const both lose 290.  The volatile pointee stays.
//
//   * THE VERTEX TAIL AS A LOOP.  The ROM re-materialises `&cr` once per component
//     (`add rX,sp,#0x16c` three times) where we take the address once, which is
//     what an unrolled loop looks like.  Written as `for (i = 0; i < 3; i++)` mwcc
//     unrolls it and then CSEs the address anyway, coming out SIX INSTRUCTIONS
//     SHORT and -307 anchored.  Six variants tried.  Not a loop.
//
// Do not take the alternative (cross, denominator) pairings of the KCL vertex
// formula: all 36 non-degenerate ones were swept and several score higher, but
// every one wins by being degenerate (cross(n,X) . X == 0) or by reusing a
// denominator -- by removing arithmetic, not by being right.
//
// PROVENANCE. Restored 2026-08-19 from nearmiss/db.jsonl, attempt
// 8273344dc1434a9e86882b88eebf7ffa (divergences 1213, parent 1332).  This body
// was banked but never committed back to notes/, so the file that lived here was
// the WORSE, earlier draft, whose banner falsely claimed the walk and prism tests
// were stubs.  Do not reinstate it.
//
// SCORING.  Use the flags that BUILD the ROM, not the ones fdiff defaults to:
// `rombuild.CFLAGS` with the language swapped carries `-Cpp_exceptions off` where
// `swarm.CPP_FLAGS` carries `-w illpragmas`.  Across 550 enrolled //cpp files the
// two are byte-identical; on this draft, which has a destructor on a stack local
// inside loops that `continue` out of its scope, they differ by one instruction
// in the build's favour.  Every number above is a build-flag number.
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
#define AXIS_DOT0(v) (FX12((v).x, unk_28)                                     \
                    + FX12((v).y, unk_2c)                                     \
                    + FX12((v).z, unk_30))

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
        if (((const dBgCh_SphCrr &)sphere).flags & 0x20) continue;        \
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
    s32 *tpv;
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
                            if (dot1 <= dot3) goto feat3;
                            goto feat1;
                        } else {
                            if (dot2 <= dot3) goto feat3;
                            goto feat2;
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
                            /* Round 1 reads the prism origin as the object; round 2's
                               first two components read it through this pointer.  Same
                               object, same value -- but mwcc addresses a member as
                               sp+imm and a pointer out of a register, and the ROM wants
                               the register form at exactly those two sites.  All 64
                               subsets have been swept three times, most recently after
                               the dispatch and centre-pointer changes; 000110 wins on
                               anchored each time.  000111 is +2 anchored and -22 shape,
                               and shape is the metric that is right about structure, so
                               it is not taken. */
                            tpv = &tp.x;

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
                            KCL_VERTEX(&vc.x, en1, tpv[0], tpv[1], tp.z)

                            t = -(sphere.unk_0ec + sphere.radius);
                            u =   sphere.unk_0ec - sphere.radius;
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
                            vb.x -= c->x;
                            vb.y -= c->y;
                            vb.z -= c->z;
                            db = AXIS_DOT0(vb);
                            vc.x -= c->x; c = &sphere.pos;
                            vc.y -= c->y; c = &sphere.pos;
                            vc.z -= c->z;
                            dc = AXIS_DOT0(vc);
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
