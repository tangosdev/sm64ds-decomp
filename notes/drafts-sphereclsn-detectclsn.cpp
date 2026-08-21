//cpp
// NONMATCHING: size 0x1bc8.  mismatches 36/1778, ratio 0.9798.
//              1778 instructions, a 0x1b4 frame, every call-gap length AND the
//              whole-function instruction multiset, all exactly the ROM's, and
//              every stack slot but ONE SWAPPED PAIR.  (One of the 36 is not an
//              instruction at all: a literal-pool word rendered as `andeq`.)
//
// 2026-08-20.  1493 -> 125 -> 36 mismatching words.  Every change is a SPELLING;
// none changes what the function computes.  The whole step came from asking
// mwccarm for its own frame map instead of inferring it -- see the DWARF section.
//
// ==================== HOW TO MEASURE, AND HOW IT MISLEADS ===================
//
//   equal     difflib LCS.  Rises when instructions are DELETED, and once missed
//             a +290 gain.  Never rank on it.
//   anchored  call sites as anchors, position-for-position inside each gap.
//             Use only while the instruction count is still wrong.
//   shape     registers and stack slots erased.  "Is the CODE wrong, or only the
//             allocation?"
//   strict    instruction i vs instruction i across all 1778.  Reads 0 unless
//             cand == 1778.  **Once the count is right, rank on this** -- or just
//             on the gate's `mismatches`, which it tracks exactly.
//
// TWO TRAPS THAT EACH COST A WRONG, COMMITTED CONCLUSION:
//
//   * `cand == 1778` IS NOT AN INVARIANT while any gap length is wrong.  The
//     draft's earlier 1778 was a COINCIDENCE -- the sqrt region was three words
//     short and gap 28 three words long, and they cancelled.  Check per-gap
//     lengths (`shapegap.py`), never the total.
//
//   * IDENTIFY A VARIABLE'S SLOT FROM ITS FULL DEFINING SEQUENCE.  Not by
//     matching reference counts between the streams (that pairs two different
//     variables that happen to share a count), and not by "the first store after
//     the index load" either.  At 0x01ffbcfc the ROM reads `tri->edgeNormal3Idx`,
//     then `ldr r6,[r6]` reads `tri->length` and stores IT first:
//
//         ldrh r1,[r1,#0xc]   ; edgeNormal3Idx
//         ldr  r6,[r6]        ; tri->length      <-- intervenes
//         str  r6,[sp,#0x110] ; tlen             <-- NOT en3
//         mul  r6,r1,r0
//         add  r1,r2,r6
//         str  r1,[sp,#0x94]  ; en3
//
//     `en3` is ROM 0x94 / draft 0xc4; 0x110 is `tlen`.  A detector that stopped at
//     the first store reported 0x110, that went into a commit as a correction to
//     a note that had been RIGHT, and a 91-position reachability sweep built on it
//     was measuring tlen -- whose slot naturally does not move when you move en3.
//
// ============================== THE GAINS ===================================
//
//   THE SQRT REGION was the SUM OPERAND ORDER.  mwccarm evaluates a two-term
//   64-bit sum RIGHT TO LEFT -- the `smull` takes the right addend, the `smlal`
//   accumulates the left -- so the source must read `fh*fh + dh*dh`.  Written the
//   other way, hoisting the two `>> 4` shifts makes `axisDot` spill instead of
//   staying in a callee-saved register, which is exactly the failure that got the
//   hoist rejected four times.  Closed gaps 7/8, 11/12, 15/16 outright.
//
//   GAP 28's THREE SURPLUS WORDS, the last gap-length mismatch in the function.
//   Two came from the `tpv` alias (deleted; the ROM has zero `add r,sp,#0x18c`)
//   and one from the slab bounds.  978 -> 302.
//
//   THE SLAB BOUNDS ARE CSE'd REPEATED EXPRESSIONS, NOT LOCALS.  The ROM
//   materialises the upper bound lazily, BETWEEN the first and second compare:
//       add r1,r6,r3 / rsb r2,r1,#0 / cmp / blt reject / sub r1,r6,r3 / cmp
//   A named local forces both to be emitted together.  Both earlier tests here
//   used named locals and so could not have found it.
//
//   `AXIS_DOT0` IN NATURAL x, y, z ORDER **TOGETHER WITH** DELETING BOTH
//   `c = &sphere.pos;` RE-BINDS.  +67, and it erases the whole 64-instruction
//   shape run in gap 28.  Neither half works alone: the natural term order takes
//   shape 1747 -> 1768 but costs one instruction, and deleting the re-binds saves
//   that instruction but is -29 by itself.  An earlier sweep recorded "multiply
//   operand order in AXIS_DOT0, all 8" -- that was operand order INSIDE `FX12`.
//   TERM order is a different axis and had never been swept.
//
//   DECLARATION ORDER, once the above landed: `fn` and `depth` just after `rawX`
//   (+25), `rad6` before `origin` in the prologue block (+10), `cy` before `cz`
//   in the leaf-step block (+8), `-(radius + unk_0ec)` (+3), and `c` moved to
//   just after `den31` (+15).
//
//   THE VORONOI DISPATCH BRANCH POLARITY, +2.  Counter-intuitive: writing
//   `if (dotN > dotM) goto featN; goto feat3;` is what makes mwcc emit the ROM's
//   `ble`, because mwcc inverts.  ble/bgt counts 38/17 -> 40/15, exactly the ROM's.
//
//   `sphere.flags & 0x20` re-read through a CONST reference cast, three sites.
//   The CV-QUALIFIER CHANGE is what defeats the CSE, not volatile.
//
//   THE PROLOGUE reads the sphere centre through a VOLATILE POINTEE.
//   `(const Vector3 *)c` does nothing -- `c` is already const, so it changes no
//   qualifier.
//
//   DELETING the `en3 = f->normals[...]` re-read that was worth +153 the previous
//   session.  The lever reversed once `cr` became non-POD.
//
// ==================== FIVE INVENTED CONSTRUCTS RETIRED ======================
// All byte-neutral or better.  The file contains no untrue construct.
//   * `struct Vtx3` -> `Vector3` (types.h already had it, same empty destructor).
//   * `volatile s16 cr[3]` + casts -> `Vector3s cr` read as `cr.x/.y/.z`, no cast.
//     For a LOCAL AGGREGATE the lever is NON-POD-NESS, not the qualifier.
//   * the `s32 *tpv` alias, worth +42 then +36 in earlier sessions.
//   * all six no-op `c = &sphere.pos;` re-binds -- and deleting the last two is
//     what promoted `c` from a compiler temporary to a declared-chain member.
//   * the slab bounds as `t`/`u` locals.
//
// ============ ASK THE COMPILER FOR THE FRAME MAP -- DO NOT INFER IT =========
//
// `-g` makes mwccarm emit DWARF, and its DW_AT_location entries give the frame
// offset of every declared local BY NAME.  It is byte-neutral, so the map applies
// to the shipping build unchanged.  And because the two instruction streams are
// position-aligned at 1778, the ROM slot referenced at position k holds whatever
// our slot at position k holds -- so naming ours names the CARTRIDGE'S frame too.
//
//     python scratchpad/perm/frame2.py     -> per-name ours/ROM/delta
//
// Two sessions were spent identifying `en3`'s slot by hand and it came out wrong
// twice, in opposite directions, once into a commit.  The compiler knew all along.
// DWARF also splits the two pools cleanly: exactly the declaration chain and the
// aggregates get DW_AT_location entries, and nothing in the temp pool does.
//
// ===================== TWO SLOT POOLS -- THE FRAME MODEL ====================
//
// The frame is [chain][temps][aggregates].  The CHAIN holds declared locals that
// mwcc kept in memory, in DECLARATION ORDER; the temp pool holds compiler
// temporaries; the aggregates go last, above both.
//
// **What decides which pool a declared local lands in is the cv-qualifier on the
// lvalue its value is READ FROM.**  A local assigned straight from a VOLATILE
// lvalue is coalesced with the temporary that read it and lives in the temp pool;
// read the same address through a plain or `const` pointer and the same local
// takes a chain slot.  That one qualifier moved the three hypotenuses from
// 0x110/0x118/0x120 to the cartridge's 0xac/0xb0/0xb4 and was worth 54 references.
// Only a bare read counts -- `x + 0` and `x | 0` fold away and still coalesce,
// while `x * k1` does not fold and does not coalesce.
//
// The previous version of this section said membership "is derived from USE" and
// that the only lever is definition count.  Both are wrong.  Re-measured here:
// declaration position (all placements), block scope, an out-parameter helper, a
// second no-op assignment in five spellings, and turning the whole test into a
// static inline function are ALL byte-identical and none of them move a variable
// between pools.  `volatile` on the declaration does move it, and costs three
// instructions; the qualifier on the SOURCE does it for free.
//
// ================== DECLARATION ORDER IS LOAD-BEARING =======================
//
// Two rules, and they look like they conflict:
//
//   * the six hottest locals win callee-saved registers, numbered r9 DOWN TO r4 in
//     declaration order, and the SEVENTH loses and takes a stack slot;
//   * slots are handed out in declaration order too -- but only across the locals
//     that HAVE slots.
//
// So `en3` has to be declared after en1/en2 or it wins a register and en2 loses
// instead (measured over all six orderings), and moving the group early rotates
// every callee-saved register by two (strict 1652 -> 1308).  But en1/en2 and the
// dot* group occupy no slots, so en3 can be declared after them and still be the
// 34th SLOT -- 0x94, where the cartridge has it -- provided everything that
// follows it in the frame also follows it in the block.  That is the order the
// draft now uses, and it is why `fn` onwards sits below `en3`.
//
// ======================== WHAT IS LEFT: 36 ==================================
//
//    19  ONE SLOT SWAP, and it is the whole remaining slot defect.  `c` and `rsc`
//        are exactly exchanged: the cartridge has c in the chain at 0xc4 and rsc
//        in the spill pool at 0x104; we have rsc at 0xc4 and c at 0x104.
//
//        The lever is the re-bind above -- a second definition moves a local out
//        of the chain -- and it fires on `c` and not on `rsc`.  Characterised:
//        it moves `c` (`&sphere.pos`, one `add` from fp) and does NOT move `f`
//        (`this->kclFile`, a load) or `rsc` (a load and a shift), so it needs a
//        value that rematerialises WITHOUT touching memory.  Aimed at rsc:
//        duplicate assignment, `= rsc`, `|= k0`, `+= k0`, `-= k0`, `^= k0`,
//        `&= ~k0`, `*= k1` at every statement site the variable reaches -- 1,810
//        compiles -- and none of them shortens the chain.  Naming the radius
//        first, so `rsc = rad << 4` is one shift from a register, does not help
//        either (another 181).
//
//        rsc is also NOT reachable as a plain CSE.  Spelled inline the chain and
//        pool land exactly right, but the temp first-fits into the 4-byte
//        alignment gap at 0xd4 -- which the ROM leaves EMPTY -- and `sphere` goes
//        hot enough to move fp -> sb with the frame at 0x1bc.
//
//    15  prologue scheduling, indices 2-17: the same 17 instructions in the same
//        registers, differently ordered.  The cartridge issues the three centre
//        loads as y, x, z and never reads through the pointer the `add` just
//        produced; we read x from it directly.  Statement order, both orders of
//        c and f, all six raw* orders, splitting the loads from the shifts,
//        loading into temps first, reading x via sphere.pos, and a volatile
//        pointer are all byte-neutral or worse.
//     1  index 1502, a commutative `add`.  The other three went away by NAMING
//        THE DIFFERENCE (`s32 dX = rawX - origin->x;`), which also reads better.
//        This one resists: flipping the source terms swaps the two LOADS instead
//        and costs three.
//     1  index 1046 is a literal-pool word, not an instruction.
//
// ==================== MEASURED AND DEAD -- DO NOT RE-DERIVE =================
//   * A PRAGMA.  `#pragma opt_common_subs off` costs 140 INSTRUCTIONS here, even
//     though a byte-matched sibling calls it "original and load-bearing" and 158
//     files in the tree use it.  **This TU was compiled with CSE ON.**
//   * A HEADER RETYPE of dBgW_Kc.h's mis-declared Vector3 members: byte-identical.
//   * The vertex tail as a loop: mwcc unrolls then CSEs the address, six short.
//   * `cr`'s address count (14 vs 15): 390 spellings, all byte-identical.  For a
//     STACK object mwcc knows the exact address, so no cast can force a
//     re-materialisation -- unlike `sphere.flags`, a reference parameter.
//   * Declaration order: full-resolution single moves (8,464), 2,000 pair swaps,
//     all 91 adjacent swaps.  Re-run after ANY structural change -- that rule has
//     paid nine times, and change #2 above came from exactly such a re-run.
//   * The 36 non-degenerate (cross, denominator) pairings of the KCL vertex
//     formula.  Several score higher; every one wins by being degenerate or by
//     reusing a denominator, i.e. by removing arithmetic rather than being right.
//
// PROVENANCE.  The body descends from lunavyqo's PR #1549 tip, restored
// 2026-08-19 from nearmiss/db.jsonl at align equal 565 / size 0x1b58.  All six
// levers documented there are still present.
//
// SCORING.  Use the flags that BUILD the ROM -- `rombuild.CFLAGS` with the
// language swapped carries `-Cpp_exceptions off` where `swarm.CPP_FLAGS` carries
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
/* The sqrt unit's result register.  Read through a CONST pointer, not a volatile
   one, and that is load-bearing rather than sloppy: mwccarm coalesces a local that is
   assigned straight from a volatile lvalue into a compiler temporary, so a volatile
   read here puts the three hypotenuses in the spill pool at 0x110/0x118/0x120.  The
   cartridge has them at 0xac/0xb0/0xb4 -- packed inside the declaration chain, which
   is where an ordinary local goes.  The busy-wait in SqrtStart is the synchronisation;
   by the time this is read the value is already settled. */
#define SQRT_RESULT (*(const s32 *)0x40002b4)

static inline void SqrtStart(u64 x, s32 zval, s32 one)
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
}

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
#define AXIS_DOT0(v) (FX12(unk_28, (v).x) \
                    + FX12(unk_2c, (v).y) \
                    + FX12(unk_30, (v).z))

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
#define EDGE_FILTER(d, zval, armA, dsqL, hypv)                                      \
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
        s32 axisDot;                                                      \
        s32 dh;                                                           \
        s32 fh;                                                           \
        if (cls != 0) continue;                                           \
        if (((const dBgCh_SphCrr &)sphere).flags & 0x20) continue;        \
        dh = (d) >> 4;                                                    \
        fh = faceDot >> 4;                                                \
        axisDot = DotVec3((const s32 *)&sn, (const Vector3 *)&unk_28);    \
        SqrtStart((u64)((s64)fh * fh + (s64)dh * dh), zval, k1);        \
        hypv = SQRT_RESULT;      \
        if (func_020397dc(hypv)) continue;                                 \
        if (axisDot > cstd::fdiv(fh, hypv)) continue;                      \
    }                                                                         \
    goto dsqL;                                                                \
    armA:                                                                     \
    if (cls == 1) {                                                           \
        if ((d) > faceDot) continue;                                          \
    } else if ((d) > (faceDot >> unk_48)) continue;                           \
    dsqL:;

s32 dBgW_Kc::DetectClsn(dBgCh_SphCrr &sphere)
{
    /* DECLARATION ORDER BELOW IS LOAD-BEARING -- do not tidy it.
       Two mwccarm rules constrain it, and together they fix the whole frame:

         * The six hottest locals win callee-saved registers, numbered r9 down to r4
           in declaration order, and the SEVENTH loses and takes a stack slot.  So
           faceDot, dot1, dot2, dot3, en1, en2 must be declared in that order and en3
           after them, or every callee-saved register rotates.
         * The stack slots are handed out in declaration order too, but only over the
           locals that HAVE slots.  en1/en2 and the dot* group take none, so en3 can
           be declared after them and still be the 34th slot -- which is where the
           cartridge has it, at 0x94.

       That is why everything from fn onwards is declared after en3: it has to follow
       en3 in the frame, so it has to follow en3 here.  Reordering this block back into
       a natural reading order costs 58 instructions. */
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
    /* volatile POINTEE, not a volatile pointer.  The ROM loads `c` from its stack
       home before almost every component read -- twice for the three `raw` reads in
       the prologue alone -- and mwcc will otherwise keep it in a register and read
       through it once.  `const volatile` is worth 290 instructions back onto their
       correct index and is what brings the count to exactly 1778.  A plain
       `(const Vector3 *)c` cast does nothing here: `c` is already const, so it
       changes no qualifier, and it is the QUALIFIER CHANGE that re-issues a load.
       Making the POINTER volatile instead (`const Vector3 *volatile c`) is -57 shape. */
    s32 d1h;
    s32 d2h;
    s32 d3h;
    s32 nn;
    s32 nnh;
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
    s16 *en1;
    s16 *en2;
    s16 *en3;
    s16 *fn;
    s32 depth;
    s16 triID;
    s32 cls;
    s32 contactKind;
    s32 hyp1;
    s32 hyp2;
    s32 hyp3;
    s32 den12;
    s32 den23;
    s32 den31;
    const volatile Vector3 *c;
    s32 rsc;
    s32 rawX;
    s32 rawY;
    s32 rawZ;
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
        s32 rad6 = sphere.radius >> 6;
        const Vector3 *origin = &f->origin;

        rawX = c->x >> 6;
        /* A no-op re-bind, and load-bearing -- but NOT for the reason it looks
           like.  It does not force the reload: delete it and the prologue emits
           exactly the same two reloads.  What it does is give `c` a SECOND
           DEFINITION, which moves it out of the declaration chain and into the
           spill pool.  The chain is then one word shorter, and every slot in the
           spill pool lands on the cartridge's address instead of four bytes
           high -- 44 references from one word.  Worth +24 net.

           The cost is that `c` then sits at 0x104 where the ROM has it at 0xc4,
           and `rsc` takes 0xc4 where the ROM has it at 0x104.  Those two are the
           only misplaced locals left; see WHAT IS LEFT. */
        c = &sphere.pos;
        rawY = c->y >> 6;
        rawZ = c->z >> 6;

        s32 dX = rawX - origin->x;
        loX = (dX - (rad6 + 0x40)) >> 6;
        if (loX < 0) loX = 0;
        hiX = (dX + (rad6 + 0x40)) >> 6;
        if (hiX > (s32)~f->xMask) hiX = ~f->xMask;
        if (loX >= hiX) return 0;

        s32 dY = rawY - origin->y;
        loY = (dY - (rad6 + 0x40)) >> 6;
        if (loY < 0) loY = 0;
        hiY = (dY + (rad6 + 0x40)) >> 6;
        if (hiY > (s32)~f->yMask) hiY = ~f->yMask;
        if (loY >= hiY) return 0;

        s32 dZ = rawZ - origin->z;
        loZ = (dZ - (rad6 + 0x40)) >> 6;
        if (loZ < 0) loZ = 0;
        hiZ = (dZ + (rad6 + 0x40)) >> 6;
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
                cy = size - (y & mask);
                cz = size - (z & mask);
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
                        EDGE_FILTER(dot1, z118, arm1_, dsq1_, hyp1)
                        d1h = dot1 >> 31;
                        dsq = rsq - (s64)dot1 * dot1;
                        goto tail;

                    edge2:
                        EDGE_FILTER(dot2, z11c, arm2_, dsq2_, hyp2)
                        d2h = dot2 >> 31;
                        dsq = rsq - (s64)dot2 * dot2;
                        goto tail;

                    edge3:
                        EDGE_FILTER(dot3, z120, arm3_, dsq3_, hyp3)
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
                            vb.x -= c->x;
                            vb.y -= c->y;
                            vb.z -= c->z;
                            db = AXIS_DOT0(vb);
                            vc.x -= c->x;
                            vc.y -= c->y;
                            vc.z -= c->z;
                            dc = AXIS_DOT0(vc);
                            if (da >= -(sphere.radius + sphere.unk_0ec) && da <= sphere.unk_0ec - sphere.radius
                             && db >= -(sphere.radius + sphere.unk_0ec) && db <= sphere.unk_0ec - sphere.radius
                             && dc >= -(sphere.radius + sphere.unk_0ec) && dc <= sphere.unk_0ec - sphere.radius)
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
