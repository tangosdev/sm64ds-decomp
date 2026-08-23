//cpp
// NONMATCHING: size 0xe40 vs the ROM's 0xe48 -- two code words short and the
// literal pool the SAME FIFTEEN WORDS (ROM 899 code + 15 pool = 914; candidate
// 897 code + 15 pool = 912). Best of all twenty-five installed mwccarm builds
// is 2004/b56; the next best is 1.2/base at 199 differing instructions, so this
// is not a version-pinning question. Counts as decompiled, not matched.
//
// THE RESIDUE IS THREE SITES AND IT IS MEASURED, not estimated. A structural
// diff of the two disassemblies (branch targets and pc-relative pool references
// normalised away, difflib alignment) leaves TWELVE ROM instructions and TEN
// candidate instructions without a counterpart, and they decompose as:
//
//   (1) CONTROL-FLOW SHAPE, and it is ELEVEN of the twelve.
//       0x020D2D40 plus the block at 0x020D2D60..0x020D2D80, the `rounds < 5`
//       decision at the end of the non-mode-2 arrival arm. The ROM branches --
//       `blt 0x020d2d6c` -- to a SECOND full epilogue, so the two arms get one
//       epilogue each (seven ROM instructions: the blt, then
//       add sp / pop / bx lr / add r0,sb,#0x4000 / mov r1,#0 / str r1,[r0,#0x6d0]).
//       mwccarm IF-CONVERTS it instead and predicates the whole short arm plus
//       its epilogue (six candidate instructions, addlt / movlt / strlt /
//       addlt sp / poplt / bxlt). Costs ONE word.
//   (2) REGISTER ALLOCATION, 0x020D2934, three ROM instructions vs two.
//       The ROM keeps the walker base sb + i*8 alive in r2 across the colp
//       computation -- `ldr r1,[pool] / mov r2,r7 / add r7,r2,r1` -- where the
//       candidate overwrites it, `ldr r0,[pool] / add r7,r7,r0`. Costs ONE word.
//   (3) SCHEDULE, one instruction, no size cost. `mov r8,#0` (the per-walker
//       loop's index init) sits at 0x020D282C in the ROM, inside the conditional
//       block that follows the countdown loop, and in the candidate it sits
//       about twelve instructions later among the constant materialisation.
//
//   1 + 1 = 2 words, which IS the size deficit exactly. Nothing else moved.
//
//   AND A FOURTH ROW THAT IS NOT A DIVERGENCE, recorded so nobody counts it as
//   one: pool word fifteen at 0x020D3610 reads 0x0212e1c0 in the ROM and zero in
//   the candidate object, because it carries the load RELOCATION to
//   data_ov006_0212e1c0 and an unlinked object holds zero under a relocation.
//
// THE IF-CONVERSION IS A FLOOR AT THIS OPTIMISATION LEVEL and was not merely
// observed. Eight spellings of that decision were compiled -- if/else with a
// return in each arm, if/else with one shared trailing return, early-return with
// the short arm first, early-return with the long arm first, and the negated
// forms of each -- and all eight emit byte-identical code. So do
// opt_conditional_moves off, opt_lifetimes off and opt_dead_assignments off.
//
// RELOC DESTINATIONS ARE VERIFIED, AND THE GATE'S OWN VERDICT NEEDS EXPLAINING.
// tools/reloc_audit.check_destinations run directly on the 2004/b56 object
// returns 68 rows, verdict EXTRA on all 68, and missing 68. THAT IS AN ALIGNMENT
// ARTEFACT, NOT A WRONG DESTINATION: check_destinations looks its config row up
// by absolute address, `cfgmap.get(addr + o)`, and every one of the 68
// relocations sits after the first lost word at 0x020D2934, so every offset
// misses. The substantive check a size-shifted body admits was run instead and
// it is stronger than a spot check: 68 relocations on both sides, the
// destination SEQUENCE in address order IDENTICAL, and the per-relocation offset
// shift is exactly 4 bytes for the first EIGHT relocations and exactly 8 bytes
// for the remaining SIXTY -- the split falling precisely at 0x020D2D40, which is
// site (1). The destination histogram is identical on both sides:
//   42x 0x020d25fc (the probe), 11x 0x02012718, 6x 0x020126e8, 6x 0x020126ac,
//   1x 0x02012dbc, 1x 0x01ffabe4 (__aeabi_idiv, from the one `/`), and
//   1x load 0x0212e1c0.
//
// THE TWO LEVERS THAT MATTER, measured on THIS file rather than on an earlier
// draft (differing-instruction counts at 2004/b56, this file = 12):
//   #pragma optimization_level 2 removed .......... 303
//   #pragma opt_common_subs off removed .......... 336
//   both removed ................................. 329
//   optimization_level 1 ......................... 187
//   optimization_level 3 or 4 .................... 303
//   opt_dead_code off ............................. 93
//   optimize_for_size on ......................... 113
//   ADDRESS OPERAND ORDER reverted ................ 60
//   no effect at all (byte-identical output): opt_strength_reduction off,
//   opt_propagation off, opt_lifetimes off, opt_dead_assignments off,
//   optimize_for_size off, opt_loop_invariants off, opt_unroll_loops off,
//   opt_conditional_moves off.
//
// THE ADDRESS OPERAND ORDER IS THE FINDING WORTH CARRYING and it is cheap. Every
// member macro below is spelled `(sb) + (i) * K + 0xNNNN` and NOT
// `(sb) + 0xNNNN + (i) * K`. The two are the same expression in C and they are
// not the same instructions: written constant-first, mwccarm folds sb with the
// constant and emits `add rX,sb,#0x4000 / ldr rD,[rX, i lsl 3]`; written
// index-first it emits the ROM's `add rX,sb,i lsl 3 / ldr rD,[rX,#0xNNN]`. That
// one edit took this body from 84 differing instructions to 40. It is the same
// family of fact as src/func_ov006_020d36a4.c's "indexed pointer versus
// byte-offset expression" note, from the other end.
//
// ---- WHAT THIS BODY IS ----------------------------------------------------
//
// dScMgAmida_c's WALKER TICK, the largest single hole in any seated minigame at
// 0xe48 = 3656 bytes, one caller (0x020d4d78, inside vtable slot 6, the
// Behavior). It is the body that MAKES THE WALKERS WALK: before it was seated,
// scene 371 rendered a correct amidakuji board whose 300-frame and 1200-frame
// frames were byte-identical, because nothing on the scene moved.
//
// IT IS AN EIGHT-NEIGHBOUR LINE FOLLOWER. It calls func_ov006_020d25fc forty-two
// times and every call site of that function in the whole overlay is in here.
// That callee offsets the walker's (col,row) at +0x4660/+0x4664 by one of eight
// compass directions and reports whether the drawn line continues there:
//     0 up-left   1 up     2 up-right
//     3 left               4 right
//     5 down-left 6 down   7 down-right
// Each tick, for each walker whose release delay at +0x46b8 has run out, this
// body tries five directions IN PREFERENCE ORDER and takes the first that hits.
// The order is per-direction and hand-written rather than generated -- the two
// 45-degree neighbours come before the two 90-degree ones, but which of each
// pair comes first varies -- and the FALLBACK when all five miss is always the
// 180-degree reverse, which is what stops a walker dead-ending:
//     dir  probes in order        fallback
//      0   0, 1, 3, 5, 2              7
//      1   1, 0, 2, 3, 4              6
//      2   2, 1, 4, 0, 7              5
//      3   3, 0, 5, 1, 6              4
//      4   4, 2, 7, 1, 6              3
//      5   5, 6, 3, 0, 7              2
//      6   6, 5, 7, 3, 4              1
//      7   7, 6, 4, 5, 2              0
// Forty of the forty-two probes are those eight rows; the other two are the
// left-then-right pair a walker still in step state 0 or 1 tries after moving
// down one row.
//
// THE RAIL SNAP IS THE OTHER HALF, and it is what ties this body to the board
// src/func_ov004_020ae5c4 draws. Six of the eight directions carry a guard on
// the walker's column: the three with a leftward component (0, 3, 5) check for
// 0x21, 0x61, 0xa1, 0xe1 and step LEFT; the three with a rightward component
// (2, 4, 7) check for 0x1f, 0x5f, 0x9f, 0xdf and step RIGHT. Both land the
// walker on 0x20, 0x60, 0xa0 or 0xe0 -- the four vertical rails
// src/func_ov006_020d3ba0.c draws and the four start columns
// src/func_ov006_020d36a4.c writes as LANE[i] * 64 + 32. So a walker that has
// run one pixel past a rail is snapped back onto it, its direction is forced to
// 6 (straight down), its step state is set to 1, and a note is played through
// func_020126e8 / func_020126ac at sound id 0x1c0. Directions 1 and 6 carry no
// guard because they do not move the column at all. Nothing in this paragraph
// was read off the two neighbouring files: the rails were derived from the
// constants here and then found to agree.
//
// THE ARRIVAL ARM is the other branch, taken when the walker's row has passed
// the board bottom at +0x4700. It switches the walker's column against the same
// four rails, compares it against the goal columns at +0x4714, +0x4718, +0x471c
// and +0x4720, and on a miss sets the per-rail arrival byte at +0x5398 through
// +0x539b. WHAT IT COMPARES AGAINST IS THE MODE, and this is where the three
// slot-36 dispatches live (0x020D2924, 0x020D2ABC, 0x020D2D8C): in mode 2
// ("Connect the Characters") each walker has its OWN goal index at +0x46a4, and
// otherwise every walker is looking for the single goal index 1. The port has
// never run this class with a mode byte of 2, so the mode-2 arms here are still
// code no run in this tree has entered.
//
// THE THIRTEEN CONSTANTS the map recorded are now identified. 0xbf, 0xc7, 0xcf
// and 0xd7 are four sound ids indexed by the winner's goal index, 0xe0 is the
// arrival sound, 0x1c0 is the rail-snap note, and 1..7 are the direction
// numbers. They are materialised in the entry block because mwccarm gives every
// integer literal in the function its own pseudo-register; nine spill to the
// frame and four live in r4, r5, r6 and fp.

#pragma optimization_level 2
#pragma opt_common_subs off

typedef unsigned char u8;
typedef int s32;

struct Obj {
    virtual void v0();  virtual void v1();  virtual void v2();  virtual void v3();
    virtual void v4();  virtual void v5();  virtual void v6();  virtual void v7();
    virtual void v8();  virtual void v9();  virtual void v10(); virtual void v11();
    virtual void v12(); virtual void v13(); virtual void v14(); virtual void v15();
    virtual void v16(); virtual void v17(); virtual void v18(); virtual void v19();
    virtual void v20(); virtual void v21(); virtual void v22(); virtual void v23();
    virtual void v24(); virtual void v25(); virtual void v26(); virtual void v27();
    virtual void v28(); virtual void v29(); virtual void v30(); virtual void v31();
    virtual void v32(); virtual void v33(); virtual void v34(); virtual void v35();
    virtual int m_90();
};

extern "C" {
void func_02012718(int id, int x);
int  func_020126e8(int x);
void func_020126ac(int a0, int a1, int a2, int a3, int s0);
void func_02012dbc(int a);
int  func_ov006_020d25fc(char *p, int b, int dir);
char data_ov006_0212e1c0[];
}

#define VCALL90(p)   (((struct Obj *)(void *)(p))->m_90())

#define COUNT(sb)    (*(s32 *)((sb) + 0x46c8))
#define DELAY(sb, i) (*(s32 *)((sb) + (i) * 4 + 0x46b8))
#define GOAL(sb, i)  (*(s32 *)((sb) + (i) * 4 + 0x46a4))
#define COLP(sb, i)  ((s32 *)((sb) + (i) * 8 + 0x4660))
#define ROWP(sb, i)  ((s32 *)((sb) + (i) * 8 + 0x4664))
#define DIRP(sb, i)  ((s32 *)((sb) + (i) * 4 + 0x4684))
#define STEPP(sb, i) ((u8 *)((sb) + (i) + 0x4680))
#define DONEP(sb, i) ((u8 *)((sb) + (i) + 0x46b4))

extern "C" void func_ov006_020d27dc(char *sb)
{
    int i;
    int j;
    int n;
    int y;
    int bottom;
    int hit;
    int frac;
    int tone;
    int kind;
    int goal;
    s32 *dp;
    s32 *rowp;
    s32 *colp;
    s32 *dirp;
    u8 *donep;
    u8 *stepp;
    char *w;

    j = 0;
    n = COUNT(sb);
    if (n > 0) {
        do {
            dp = (s32 *)(sb + j * 4 + 0x46b8);
            if (*dp > 0)
                (*dp)--;
            j++;
            n = COUNT(sb);
        } while (j < n);
    }
    if (n <= 0)
        return;

    i = 0;
    do {
        if (DELAY(sb, i) > 0)
            continue;

        w = sb + i * 8;
        rowp = (s32 *)(w + 0x4664);
        y = *rowp;
        bottom = *(s32 *)(sb + 0x4700);

        if (y > bottom) {
            donep = DONEP(sb, i);
            if (*donep == 1)
                continue;

            hit = 0;
            if (VCALL90(sb)) {
                colp = (s32 *)(w + 0x4660);
                goal = GOAL(sb, i);
                switch (*colp) {
                case 0x20:
                    if (*(s32 *)(sb + 0x4714) == goal) hit = 1;
                    else *(u8 *)(sb + 0x5398) = 1;
                    break;
                case 0x60:
                    if (*(s32 *)(sb + 0x4718) == goal) hit = 1;
                    else *(u8 *)(sb + 0x5399) = 1;
                    break;
                case 0xa0:
                    if (*(s32 *)(sb + 0x471c) == goal) hit = 1;
                    else *(u8 *)(sb + 0x539a) = 1;
                    break;
                case 0xe0:
                    if (*(s32 *)(sb + 0x4720) == goal) hit = 1;
                    else *(u8 *)(sb + 0x539b) = 1;
                    break;
                }
            } else {
                colp = (s32 *)(w + 0x4660);
                switch (*colp) {
                case 0x20:
                    if (*(s32 *)(sb + 0x4714) == 1) hit = 1;
                    else *(u8 *)(sb + 0x5398) = 1;
                    break;
                case 0x60:
                    if (*(s32 *)(sb + 0x4718) == 1) hit = 1;
                    else *(u8 *)(sb + 0x5399) = 1;
                    break;
                case 0xa0:
                    if (*(s32 *)(sb + 0x471c) == 1) hit = 1;
                    else *(u8 *)(sb + 0x539a) = 1;
                    break;
                case 0xe0:
                    if (*(s32 *)(sb + 0x4720) == 1) hit = 1;
                    else *(u8 *)(sb + 0x539b) = 1;
                    break;
                }
            }

            if (hit == 1) {
                if (VCALL90(sb)) {
                    if (*(u8 *)(sb + 0x46d5) == 0) {
                        func_02012718(0xe0, *colp << 12);
                        switch (GOAL(sb, i)) {
                        case 0: func_02012718(0xbf, *colp << 12); break;
                        case 1: func_02012718(0xc7, *colp << 12); break;
                        case 2: func_02012718(0xcf, *colp << 12); break;
                        case 3: func_02012718(0xd7, *colp << 12); break;
                        }
                    } else {
                        func_02012718(0x1c3, *colp << 12);
                    }
                    *donep = 1;
                    (*(s32 *)(sb + 0x46cc))++;
                    if (*(s32 *)(sb + 0x46cc) < COUNT(sb))
                        continue;
                    if (*(u8 *)(sb + 0x46d5) == 0) {
                        (*(s32 *)(sb + 0x5374))++;
                        (*(s32 *)(sb + 0x53e8))++;
                        if (*(s32 *)(sb + 0x53e8) > 0x270f)
                            *(s32 *)(sb + 0x53e8) = 0x270f;
                        *(s32 *)(sb + 0x53e0) = 0x78;
                    } else {
                        *(s32 *)(sb + 0x53e0) = 0x1e;
                    }
                    return;
                } else {
                    colp = COLP(sb, i);
                    func_02012718(0xe0, *colp << 12);
                    func_02012718(0x1c1, *colp << 12);
                    *donep = 1;
                    (*(s32 *)(sb + 0x46cc))++;
                    if (*(s32 *)(sb + 0x46cc) < COUNT(sb))
                        continue;
                    (*(s32 *)(sb + 0x5374))++;
                    (*(s32 *)(sb + 0x53e8))++;
                    if (*(s32 *)(sb + 0x53e8) > 0x270f)
                        *(s32 *)(sb + 0x53e8) = 0x270f;
                    kind = *(s32 *)(data_ov006_0212e1c0 +
                                    *(s32 *)(sb + 0x53d4) * 0x1c);
                    switch (kind) {
                    case 0:
                        break;
                    case 1:
                        if (*(s32 *)(sb + 0x5374) % 2 == 0)
                            *(s32 *)(sb + 0x5368) += 5;
                        break;
                    case 2:
                        *(s32 *)(sb + 0x5368) += 5;
                        break;
                    }
                    if (*(s32 *)(sb + 0x5368) > 0x64)
                        *(s32 *)(sb + 0x5368) = 0x64;
                    if (*(s32 *)(sb + 0x5374) >= 5) {
                        *(s32 *)(sb + 0x53c0) = 0x3c;
                        *(s32 *)(sb + 0x46d0) = 2;
                        *(u8 *)(sb + 0x46d4) = 1;
                        return;
                    } else {
                        *(s32 *)(sb + 0x46d0) = 0;
                        return;
                    }
                }
            } else {
                if (VCALL90(sb)) {
                    func_02012dbc(5);
                    if (*(u8 *)(sb + 0x46d5) == 0)
                        func_02012718(0x1c2, *COLP(sb, i) << 12);
                    else
                        func_02012718(0x1c3, *COLP(sb, i) << 12);
                    *donep = 1;
                    *(u8 *)(sb + 0x46d5) = 1;
                    (*(s32 *)(sb + 0x46cc))++;
                    if (*(s32 *)(sb + 0x46cc) >= COUNT(sb))
                        *(s32 *)(sb + 0x53e0) = 0x3c;
                    return;
                } else {
                    func_02012718(0xe1, *COLP(sb, i) << 12);
                    *(s32 *)(sb + 0x53c0) = 0x3c;
                    *(s32 *)(sb + 0x46d0) = 2;
                    *(u8 *)(sb + 0x46d5) = 1;
                    return;
                }
            }
        }

        if (y < -0xc0 || y >= 0x98) {
            (*rowp)++;
            continue;
        }

        stepp = STEPP(sb, i);
        if (*stepp <= 1) {
            *rowp = y + 1;
            if (!func_ov006_020d25fc(sb, i, 3))
                func_ov006_020d25fc(sb, i, 4);
            continue;
        }

        frac = (y + 0xd4) * 0x1f4 / (bottom + 0xd4);
        dirp = DIRP(sb, i);

        switch (*dirp) {
        case 0:
            y = *COLP(sb, i);
            if (y == 0x21 || y == 0x61 || y == 0xa1 || y == 0xe1) {
                colp = COLP(sb, i);
                (*colp)--;
                *dirp = 6;
                *stepp = 1;
                tone = func_020126e8(*colp << 12);
                func_020126ac(0x1c0, 6, 0, frac, tone);
                break;
            }
            if (func_ov006_020d25fc(sb, i, 0)) break;
            if (func_ov006_020d25fc(sb, i, 1)) break;
            if (func_ov006_020d25fc(sb, i, 3)) break;
            if (func_ov006_020d25fc(sb, i, 5)) break;
            if (func_ov006_020d25fc(sb, i, 2)) break;
            *dirp = 7;
            break;

        case 1:
            if (func_ov006_020d25fc(sb, i, 1)) break;
            if (func_ov006_020d25fc(sb, i, 0)) break;
            if (func_ov006_020d25fc(sb, i, 2)) break;
            if (func_ov006_020d25fc(sb, i, 3)) break;
            if (func_ov006_020d25fc(sb, i, 4)) break;
            *dirp = 6;
            break;

        case 2:
            y = *COLP(sb, i);
            if (y == 0x1f || y == 0x5f || y == 0x9f || y == 0xdf) {
                colp = COLP(sb, i);
                (*colp)++;
                *dirp = 6;
                *stepp = 1;
                tone = func_020126e8(*colp << 12);
                func_020126ac(0x1c0, 6, 0, frac, tone);
                break;
            }
            if (func_ov006_020d25fc(sb, i, 2)) break;
            if (func_ov006_020d25fc(sb, i, 1)) break;
            if (func_ov006_020d25fc(sb, i, 4)) break;
            if (func_ov006_020d25fc(sb, i, 0)) break;
            if (func_ov006_020d25fc(sb, i, 7)) break;
            *dirp = 5;
            break;

        case 3:
            y = *COLP(sb, i);
            if (y == 0x21 || y == 0x61 || y == 0xa1 || y == 0xe1) {
                colp = COLP(sb, i);
                (*colp)--;
                *dirp = 6;
                *stepp = 1;
                tone = func_020126e8(*colp << 12);
                func_020126ac(0x1c0, 6, 0, frac, tone);
                break;
            }
            if (func_ov006_020d25fc(sb, i, 3)) break;
            if (func_ov006_020d25fc(sb, i, 0)) break;
            if (func_ov006_020d25fc(sb, i, 5)) break;
            if (func_ov006_020d25fc(sb, i, 1)) break;
            if (func_ov006_020d25fc(sb, i, 6)) break;
            *dirp = 4;
            break;

        case 4:
            y = *COLP(sb, i);
            if (y == 0x1f || y == 0x5f || y == 0x9f || y == 0xdf) {
                colp = COLP(sb, i);
                (*colp)++;
                *dirp = 6;
                *stepp = 1;
                tone = func_020126e8(*colp << 12);
                func_020126ac(0x1c0, 6, 0, frac, tone);
                break;
            }
            if (func_ov006_020d25fc(sb, i, 4)) break;
            if (func_ov006_020d25fc(sb, i, 2)) break;
            if (func_ov006_020d25fc(sb, i, 7)) break;
            if (func_ov006_020d25fc(sb, i, 1)) break;
            if (func_ov006_020d25fc(sb, i, 6)) break;
            *dirp = 3;
            break;

        case 5:
            y = *COLP(sb, i);
            if (y == 0x21 || y == 0x61 || y == 0xa1 || y == 0xe1) {
                colp = COLP(sb, i);
                (*colp)--;
                *dirp = 6;
                *stepp = 1;
                tone = func_020126e8(*colp << 12);
                func_020126ac(0x1c0, 6, 0, frac, tone);
                break;
            }
            if (func_ov006_020d25fc(sb, i, 5)) break;
            if (func_ov006_020d25fc(sb, i, 6)) break;
            if (func_ov006_020d25fc(sb, i, 3)) break;
            if (func_ov006_020d25fc(sb, i, 0)) break;
            if (func_ov006_020d25fc(sb, i, 7)) break;
            *dirp = 2;
            break;

        case 6:
            if (func_ov006_020d25fc(sb, i, 6)) break;
            if (func_ov006_020d25fc(sb, i, 5)) break;
            if (func_ov006_020d25fc(sb, i, 7)) break;
            if (func_ov006_020d25fc(sb, i, 3)) break;
            if (func_ov006_020d25fc(sb, i, 4)) break;
            *dirp = 1;
            break;

        case 7:
            y = *COLP(sb, i);
            if (y == 0x1f || y == 0x5f || y == 0x9f || y == 0xdf) {
                colp = COLP(sb, i);
                (*colp)++;
                *dirp = 6;
                *stepp = 1;
                tone = func_020126e8(*colp << 12);
                func_020126ac(0x1c0, 6, 0, frac, tone);
                break;
            }
            if (func_ov006_020d25fc(sb, i, 7)) break;
            if (func_ov006_020d25fc(sb, i, 6)) break;
            if (func_ov006_020d25fc(sb, i, 4)) break;
            if (func_ov006_020d25fc(sb, i, 5)) break;
            if (func_ov006_020d25fc(sb, i, 2)) break;
            *dirp = 0;
            break;
        }
    } while (++i < COUNT(sb));
}
