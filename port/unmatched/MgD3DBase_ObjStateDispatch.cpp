// PORT_HOST_ABI. The two per-frame TICK BODIES of the two shared sub-object
// families in ov006's minigame-object region -- func_ov006_020c4cd8 (member
// pointer at +0x30) and func_ov006_020c7860 (member pointer at +0x3c) -- each
// of which dispatches a pointer-to-member the host cannot spell. Run mg11,
// lanes BNP (0x174, scene 372) and BNT (0x175, scene 373), UNIFIED AT THE
// MERGE GATE.
//
// ---- 0. THE MERGE RULING, AND THE CHECK THAT MISSED THE COLLISION ---------
//
// THIS FILE IS THE UNION OF TWO LANES' INDEPENDENT HOST COPIES OF THE SAME TWO
// ROM BODIES, and the way that went unnoticed until link time is the useful
// part. Lane BNP wrote them here. Lane BNT wrote them in
// port/unmatched/MgJump2_SubDispatch.cpp -- same two symbols, DIFFERENT
// FILENAME -- so git saw no add/add conflict, both files merged clean, and the
// first thing that objected was the linker:
//
//     MgJump2_SubDispatch.cpp.obj : error LNK2005:
//       _func_ov006_020c4cd8 already defined in MgD3DBase_ObjStateDispatch.cpp.obj
//       _func_ov006_020c7860 already defined in MgD3DBase_ObjStateDispatch.cpp.obj
//
// The wave's convergence review compared the three files the two lanes had
// given the SAME name and concluded there was no disagreement anywhere. It was
// comparing filenames. THE RULE THIS COST US: a convergence check must compare
// by SYMBOL, not by filename -- two lanes can converge on identical repairs in
// differently-named files and read as no-conflict right up to the link.
//
// Both of the paragraphs the two files used to carry on this point were wrong
// in the same direction and are deleted rather than softened. BNP's said lane
// BNT "defines none of these symbols" and that this file "has no BNP/BNT
// add/add conflict at the merge". BNT's said its copy "defines nothing another
// lane in this wave also defines, and the merge owes it no dedup ruling". Each
// lane checked the other's tree for its own FILENAME and found nothing.
//
// WHAT THE TWO COPIES ACTUALLY DISAGREED ON, measured at the merge rather than
// argued: nothing that runs. The two routing tables are identical -- the same
// twenty-five case addresses, no asymmetry. The receiver differed in SPELLING
// only: BNP passed the bare `c`, BNT passed `c + (p->adj >> 1)`. Those are the
// same value on every path this tree can take, because the routed path is
// gated on `adj == 0` (where the adjustment is a no-op) and the fallback
// port_mg_call0 REFUSES any nonzero adjustment outright -- it reports
// mg_unhandled and returns without dispatching. BNT's is kept below because it
// is the ROM-faithful spelling of the Itanium member-pointer sequence (the
// `add r0, r4, r1, asr#1` at 0x020c4ce8 and 0x020c789c is exactly this), and
// keeping the faithful form costs nothing while the bare receiver would quietly
// become wrong the day a nonzero adjustment is measured and implemented.
// The ApproachLinear argument differed the same way: BNP declared `int *` and
// passed a pointer, BNT declared `int &` and passed `*ptr`. Same address, and
// the `int *` form is kept because it is the tree's convention -- both
// port/hal/player_bridges.cpp's definition and the decorated name in the
// /alternatename pragma in port/hal/cxx_aliases.cpp spell that parameter PAH.
//
// BOTH LANES' CENSUS APIS SURVIVE WITH THEIR EXACT NAMES AND SEMANTICS, which
// is the whole reason this is a union and not a pick. BNP's four
// port_mg_objstate_* entry points feed hal/scene_mg_jump.cpp's report and
// BNT's four port_mg_jump2_sub_* entry points feed hal/scene_mg_jump2.cpp's,
// including BNT's per-site which=0/1 split; neither seat file is touched. Both
// counter sets tick inside the one dispatch below. That is sound because ONE
// SCENE RUNS PER PROCESS: each seat resets its own counters at init and reads
// its own at report, so a tick charged by the other lane's scene can never
// surface in this lane's census.
//
// ---- 1. HOW THEY WERE FOUND, AND WHY THE FIRST SWEEP MISSED THEM ----------
//
// port/slice_bnp.txt section 6 records a `::*` sweep over the 41 TUs of
// dScMgJump_c's own code block and dScMgD3DBase_c's, and it returns exactly one
// file. THAT SWEEP WAS SCOPED TOO NARROWLY and the lane's own first proof run
// is what said so: with the state machine started, scene 372 faulted with
//
//     FAULT code c0000005 at +0x01cc4d1c accessing 020c4d1c
//       walker node 307FA7F4 actor 307FA7CC id 0x174
//       regs eax=020c4d1c ebx=020ee5b8
//
// -- a jump to a RAW DS ADDRESS, with ebx still holding func_ov006_020ee5b8,
// the state this class had just entered. 0x020c4d1c is a real four-byte ov006
// function, so the fault is not a wild pointer: it is a member pointer nobody
// routed.
//
// LANE LKY'S ENCODING DETECTOR RUN OVER THE WHOLE CLOSURE IS WHAT NAMES THEM.
// Decoding every word of the TUs in port/slice_bnp.txt (84 when this was first
// written, 129 once the twenty-five state bodies below were added) and looking for
// `add Rd,Rn,Rm,asr #1` immediately followed by `ands Rm,Rm,#1` finds THREE
// sites, not one:
//
//     020ee290  func_ov006_020ee27c   vtable slot 6, dScMgJump_c's own
//     020c4ce8  func_ov006_020c4cd8   the object adapter's tick, +0x30
//     020c7898  func_ov006_020c7860   the second object's tick, +0x3c
//
// The lesson is the scope, not the tool: a `::*` sweep over the CLASS is not a
// sweep over the CLOSURE, and a seat links the closure.
//
// LANE BNT REACHED THE SAME TWO BODIES FROM THE OTHER SIDE, which is worth
// keeping now that the two records live together: its encoding scan over
// dScMgJump2_c's closure found three dispatch sites too -- 0x020ef450 (that
// class's own vtable slot 6), 0x020c4cec and 0x020c789c -- and its independent
// `::*` source sweep returned four hits, src/func_ov004_020b87e0.cpp (the
// framework state setter, already host-copied in
// unmatched/MgBase_StateSetter.cpp and excluded from every minigame slice),
// src/func_ov006_020c4cd8.cpp, src/func_ov006_020c7860.cpp and
// src/func_ov006_020ef3e0.cpp. Two lanes, two closures, two detectors each,
// and the same two shared bodies at the end of all four.
//
// ---- 2. THE TWO FIELDS AND THE TWENTY-FIVE STATES -------------------------
//
// func_ov006_020c4cd8 dispatches the pair at self+0x30 and then runs
// Animation::Advance(self+0x88) and func_ov006_020c4c00(self).
// func_ov006_020c7860 dispatches the pair at self+0x3c between an
// ApproachLinear/AddVec3 prologue and a four-call tail.
//
// Both read their pair out of MOUNTED .data, which is why nothing in the build
// names the state bodies and why port/tools/closure.py reported this slice
// closed: the code words are DS addresses in a data table, not symbol
// references. Enumerated from the RELOCATION SET rather than by walking an
// address range -- every load relocation whose source lies in ov006's .data,
// whose next word is zero, and whose target is a function start inside
// 0x020c4000..0x020c9000 -- there are TWENTY-FIVE distinct state bodies, and
// every one of them has a src file:
//
//   020c4d1c (0x4, `bx lr`, the idle state and the address the fault jumped to)
//   020c4e8c 020c4fa4 020c53f8 020c5530 020c5658 020c5928 020c5bf8 020c5d28
//   020c6088 020c61c4 020c627c 020c6378 020c6400 020c66bc 020c6a9c 020c78ec
//   020c7a30 020c7c68 020c8048 020c814c 020c833c 020c85bc 020c864c 020c8680
//
// All twenty-five are `void f(receiver)` in the ROM except 020c4d1c, which
// takes none and returns immediately.
//
// ---- 2b. WHICH FIELD EACH STATE BELONGS TO (lane BNT's measurement) -------
//
// THIRTY-EIGHT eight-byte {code, 0} records sit in two runs of ov006 .data,
// and the split between the two fields was MEASURED (each writer's own store
// offset) rather than inferred from the addresses. Each run is bounded by its
// FIRST and LAST ACTUAL RECORD, not by a swept address range -- section 2c is
// what that distinction is worth:
//
//   FIELD +0x30, TWENTY-THREE records, first 0x0213af18, last 0x0213afc8,
//     SIXTEEN distinct code words: 0x020c4d1c (a 0x18-byte idle), 0x020c4e8c,
//     0x020c4fa4, 0x020c53f8, 0x020c5530, 0x020c5658, 0x020c5928, 0x020c5bf8,
//     0x020c5d28, 0x020c6088, 0x020c61c4, 0x020c627c, 0x020c6378, 0x020c6400,
//     0x020c66bc and 0x020c6a9c.
//   FIELD +0x3c, FIFTEEN records, first 0x0213b020, last 0x0213b090, NINE
//     distinct code words: 0x020c78ec, 0x020c7a30, 0x020c7c68, 0x020c8048,
//     0x020c814c, 0x020c833c, 0x020c85bc, 0x020c864c and 0x020c8680.
//
// 23 + 15 = 38 records, 16 + 9 = 25 distinct codes, and there are 25 cases in
// the switch below. The two lanes' enumerations agree body for body: BNP's
// relocation-set walk and BNT's record walk return the same twenty-five.
//
// THE SPLIT WAS MEASURED ON THE WRITERS THEMSELVES, not read off the record
// addresses: every function that installs one of these records was
// disassembled and its own store offset taken. Eight write +0x30/+0x34
// (0x020c4060, 0x020c4148, 0x020c4710, 0x020c4c54, 0x020c4d20, 0x020c64e4,
// 0x020c68f4, 0x020c6ca4) plus the eleven-arm switch inside
// func_ov006_020c6a9c and func_ov006_020c4f68; the rest write +0x3c/+0x40
// (0x020c7300, 0x020c762c, 0x020c79a8, 0x020c7ba4, 0x020c802c, 0x020c8084,
// 0x020c8270, 0x020c81e0, 0x020c85a0, 0x020c862c, 0x020c8658, 0x020c8768).
// EVERY adjustment word is zero and EVERY one of the twenty-five code words
// resolves to a matched src TU with no NONMATCHING banner, so neither machine
// has a floor -- checked per body against src/ rather than inferred from the
// costs table's nosrc column, which is computed over the VTABLE only and
// cannot see a state-table floor at all.
//
// ---- 2c. WHY THE ENDPOINTS ARE RECORDS AND NOT A RANGE --------------------
//
// An earlier version of this block quoted the +0x30 run as
// "0x0213af10..0x0213afd0", and NEITHER of those two addresses is a record:
// 0x0213af10 is {0,0} with no relocation and 0x0213afd0 is {0, 0x0213b000}.
// Quoting a range invites a sweep, and a sweep over this neighbourhood
// over-reads. Widening the window by 0x118 bytes picks up FIVE more real
// records -- 0x0213aec0, 0x0213aec8, 0x0213aed0, 0x0213aed8 and 0x0213aee0,
// holding 0x020c3ad8, 0x020c395c, 0x020c38ac, 0x020c3904 and 0x020c3ad8 --
// which would put the distinct count at TWENTY rather than sixteen.
//
// THEY BELONG TO A DIFFERENT FAMILY AND NOT TO THIS ONE. Their five loaders
// (func_ov006_020c38b0, _020c3908, _020c3990, _020c3adc, _020c3b80) are the
// func_ov006_020c3d18 machine that run mg5 lane FLW already host-copies in
// port/unmatched/MgFlower_SubDispatch.cpp, and hal/scene_mg_flower.cpp's own
// GROUP D note already aliases func_ov006_020c3adc. NONE of the nine
// addresses appears anywhere in lane BNT's 230-body closure -- checked, not
// assumed. That is why this block names records and this file's switch keys
// on CODE WORDS: an address-keyed switch cannot mis-route a neighbour's state
// even when a reader's window is too wide.
//
// THE FIRST VERSION OF BNT'S FILE CARRIED TEN OF THE TWENTY-FIVE, and the run
// is what caught it. A scan that took only pair records whose single relocation
// came from inside the closure-as-then-known found six and four; the writers
// that install the rest -- func_ov006_020c4f68, _020c762c, _020c8270,
// _020c81e0, _020c85a0 and _020c8768, plus the eleven-arm switch inside
// func_ov006_020c6a9c, which is six writers and a switch and not the "other
// twelve" that sentence used to say -- were not in that closure BECAUSE they
// are only reachable through the dispatch this file implements. The closure
// and the switch are mutually recursive, and the fix is to walk it again with
// every code word as a root until it stabilises. The successive answers were
// 172 (vtable and class field states as roots), 207 (plus the first ten
// sub-object states), 223 (plus the rest of the twenty-two then known) and
// finally 230, from ONE authoritative walk over all FIFTY-NINE roots -- the
// factory, the eight own overrides, the seventeen middle-base overrides, the
// five class states, the twenty-five sub-object states and the three
// element-vtable bodies. Only the 230 is a measurement; the three before it
// are under-counts, kept so the shape of the mistake stays legible.
//
// THE 600-FRAME RUN NAMED THE TWO THAT MATTERED, which is why the run is the
// acceptance and not the scan: "MINIGAME STATE DISPATCH UNHANDLED: DS address
// 0x020c833c" and "... 0x020c6378", 1266 + 914 ticks that reached the
// framework and did nothing. A second 1800-frame run named one more,
// 0x020c7c68, once lane BNT's element vtable stopped faulting the run early.
// ALL TWENTY-FIVE are cases below, and the acceptance is a run whose
// framework-routed count is ZERO on both machines rather than a scan that
// says it should be.
//
// ---- 3. THE STORED PAIRS KEEP THE ROM'S OWN WORDS -------------------------
//
// Same rule as unmatched/MgBounceAndPounce_StateDispatch.cpp section 2 and for
// the same reason: this file DECODES the address the ROM wrote, it does not
// rewrite the mounted pairs. The pair run these two read from is shared with
// classes this port has not seated, and rewriting it would decide their
// behaviour from inside one lane.
//
// BOTH SWITCHES CARRY BOTH SETS -- or rather, now that the two lanes' copies
// are one file, the ONE switch carries both sets and always did. A field is one
// address and a code word is one address, so the two key sets are disjoint and
// a shared switch cannot mis-route; splitting them would only mean a state that
// migrated between the two object families reported UNHANDLED instead of
// running. The per-site counters below say which site actually routed what, so
// the claim that the two sets are disjoint in practice is measured on every
// run.
//
// ONE CASE IS UNREACHABLE ON A SCENE-373 BUILD AND SAYS SO HERE.
// func_ov006_020c81e0 -- the sole installer of the {0x020c814c, 0} record at
// 0x0213b080 -- is reachable ONLY from 0x174's own state machine
// (func_ov006_020ee508 -> _020ee3bc -> _020c7418 -> _020c81e0). It has a src TU
// and its own delink block and it is deliberately NOT in port/slice_bnt.txt, so
// nothing a scene-373 build runs can reach it; it goes live on a scene-372
// build, which is now the same binary. `case 0x020c814cu` is kept for both
// because the two classes drive the SAME two shared sub-object families through
// this same code, and a switch that is complete over the two data runs cannot
// mis-route -- it keys on a code word.

#include <cstdio>

struct MgPmf { unsigned code; int adj; };

extern "C" {

/* the family chain's end */
void port_mg_call0(void *self, unsigned code, int adj);

/* the two dispatchers' own non-state callees, kept exactly as src has them.
   _Z14ApproachLinearRiii takes int* here rather than int&: the two lanes spelled
   it both ways and they are the same address, and int* is what
   hal/player_bridges.cpp defines and what the /alternatename pragma in
   hal/cxx_aliases.cpp decorates (PAH). */
void _ZN9Animation7AdvanceEv(void *a);
void func_ov006_020c4c00(void *c);
void _Z14ApproachLinearRiii(int *r, int target, int step);
void AddVec3(void *a, void *b, void *c);
void func_ov006_020bfec0(void *a, char *b, short *d);
void func_ov006_020c76e0(char *c);
extern int   data_ov006_0213b010;
extern int   data_ov006_0213b018;
extern void *data_ov006_02141a40;

/* the twenty-five state bodies: field +0x30's sixteen ... */
void func_ov006_020c4d1c(void);
void func_ov006_020c4e8c(char *c);
void func_ov006_020c4fa4(char *c);
void func_ov006_020c53f8(char *c);
void func_ov006_020c5530(char *c);
void func_ov006_020c5658(char *c);
void func_ov006_020c5928(char *c);
void func_ov006_020c5bf8(char *c);
void func_ov006_020c5d28(char *c);
void func_ov006_020c6088(char *c);
void func_ov006_020c61c4(int c);
void func_ov006_020c627c(char *c);
void func_ov006_020c6378(int c);
void func_ov006_020c6400(char *c);
void func_ov006_020c66bc(char *c);
void func_ov006_020c6a9c(char *c);
/* ... and field +0x3c's nine */
void func_ov006_020c78ec(char *c);
void func_ov006_020c7a30(char *c);
void func_ov006_020c7c68(char *c);
void func_ov006_020c8048(void *c);
void func_ov006_020c814c(char *c);
void func_ov006_020c833c(char *c);
void func_ov006_020c85bc(char *c);
void func_ov006_020c864c(int *c);
void func_ov006_020c8680(char *c);

/* the two host copies, defined below. func_ov006_020c7860 must be char* to
   agree with include/decl_common.h:2432 or MSVC raises C2733 on the extern "C"
   overload. */
void func_ov006_020c4cd8(char *c);
void func_ov006_020c7860(char *c);

/* the witness lane BNP's seat reads (hal/scene_mg_jump.cpp) */
void     port_mg_objstate_call0(void *self, unsigned code, int adj);
void     port_mg_objstate_counts(unsigned *calls, unsigned *hits,
                                 unsigned *nullpmf);
unsigned port_mg_objstate_count(void);
unsigned port_mg_objstate_addr(unsigned i);
unsigned port_mg_objstate_hit(unsigned i);

/* the witness lane BNT's seat reads (hal/scene_mg_jump2.cpp), per dispatch
   site: which=0 is func_ov006_020c4cd8, which=1 is func_ov006_020c7860 */
void     port_mg_jump2_sub_counts(int which, unsigned *calls, unsigned *routed,
                                  unsigned *unknown, unsigned *distinct);
unsigned port_mg_jump2_sub_row(int which, unsigned i, unsigned *code);
unsigned port_mg_jump2_sub_dropped(int which);
unsigned port_mg_jump2_sub_capacity(void);

}  /* extern "C" */

// ---- the address switch ----------------------------------------------------

static unsigned g_obj_calls, g_obj_hits, g_obj_nullpmf;

static const unsigned kObjStates[] = {
    0x020c4d1cu, 0x020c4e8cu, 0x020c4fa4u, 0x020c53f8u, 0x020c5530u,
    0x020c5658u, 0x020c5928u, 0x020c5bf8u, 0x020c5d28u, 0x020c6088u,
    0x020c61c4u, 0x020c627cu, 0x020c6378u, 0x020c6400u, 0x020c66bcu,
    0x020c6a9cu, 0x020c78ecu, 0x020c7a30u, 0x020c7c68u, 0x020c8048u,
    0x020c814cu, 0x020c833cu, 0x020c85bcu, 0x020c864cu, 0x020c8680u,
};
enum { kObjStateCount = sizeof kObjStates / sizeof kObjStates[0] };
static unsigned g_obj_state_hits[kObjStateCount];

static void obj_note(unsigned code)
{
    for (unsigned i = 0; i < (unsigned)kObjStateCount; ++i)
        if (kObjStates[i] == code) { ++g_obj_state_hits[i]; return; }
}

static int obj_try(void *self, unsigned code)
{
    char *c = (char *)self;
    obj_note(code);
    switch (code) {
    case 0x020c4d1cu: func_ov006_020c4d1c();  return 1;
    case 0x020c4e8cu: func_ov006_020c4e8c(c); return 1;
    case 0x020c4fa4u: func_ov006_020c4fa4(c); return 1;
    case 0x020c53f8u: func_ov006_020c53f8(c); return 1;
    case 0x020c5530u: func_ov006_020c5530(c); return 1;
    case 0x020c5658u: func_ov006_020c5658(c); return 1;
    case 0x020c5928u: func_ov006_020c5928(c); return 1;
    case 0x020c5bf8u: func_ov006_020c5bf8(c); return 1;
    case 0x020c5d28u: func_ov006_020c5d28(c); return 1;
    case 0x020c6088u: func_ov006_020c6088(c); return 1;
    case 0x020c61c4u: func_ov006_020c61c4((int)(size_t)c); return 1;
    case 0x020c627cu: func_ov006_020c627c(c); return 1;
    case 0x020c6378u: func_ov006_020c6378((int)(size_t)c); return 1;
    case 0x020c6400u: func_ov006_020c6400(c); return 1;
    case 0x020c66bcu: func_ov006_020c66bc(c); return 1;
    case 0x020c6a9cu: func_ov006_020c6a9c(c); return 1;
    case 0x020c78ecu: func_ov006_020c78ec(c); return 1;
    case 0x020c7a30u: func_ov006_020c7a30(c); return 1;
    case 0x020c7c68u: func_ov006_020c7c68(c); return 1;
    case 0x020c8048u: func_ov006_020c8048(c); return 1;
    case 0x020c814cu: func_ov006_020c814c(c); return 1;
    case 0x020c833cu: func_ov006_020c833c(c); return 1;
    case 0x020c85bcu: func_ov006_020c85bc(c); return 1;
    case 0x020c864cu: func_ov006_020c864c((int *)c); return 1;
    case 0x020c8680u: func_ov006_020c8680(c); return 1;
    default:                                  return 0;
    }
}

// ---- lane BNT's per-site census --------------------------------------------

/* THE DISTINCT-STATE TABLE HOLDS EVERY STATE EITHER MACHINE HAS, and the
   number is checked against the switch rather than picked: field +0x30 has
   SIXTEEN code words and field +0x3c has NINE, so 32 leaves headroom for a
   state a later reading adds without the table quietly truncating.

   IT USED TO BE EIGHT, WHICH IS EXACTLY THE +0x30 MACHINE'S OBSERVED COUNT,
   so every "8 distinct" that census had ever printed was AT THE CAP and could
   not be told from saturation. The overflow counter below exists so that can
   never be true silently again: a census that drops a state must SAY it
   dropped one. The lane review that caught this is the reason both numbers
   are now printed. */
enum { SUB_SEEN_MAX = 32 };

static unsigned g_sub_calls[2], g_sub_routed[2], g_sub_unknown[2];
static unsigned g_sub_distinct[2], g_sub_dropped[2];
static unsigned g_sub_seen[2][SUB_SEEN_MAX], g_sub_hits[2][SUB_SEEN_MAX];

static void sub_note(int which, unsigned code)
{
    for (unsigned i = 0; i < g_sub_distinct[which]; ++i)
        if (g_sub_seen[which][i] == code) { ++g_sub_hits[which][i]; return; }
    if (g_sub_distinct[which] < SUB_SEEN_MAX) {
        g_sub_seen[which][g_sub_distinct[which]] = code;
        g_sub_hits[which][g_sub_distinct[which]] = 1;
        ++g_sub_distinct[which];
    } else {
        ++g_sub_dropped[which];
    }
}

extern "C" unsigned port_mg_jump2_sub_dropped(int which)
{
    return (which < 0 || which > 1) ? 0u : g_sub_dropped[which];
}

extern "C" unsigned port_mg_jump2_sub_capacity(void) { return SUB_SEEN_MAX; }

extern "C" void port_mg_jump2_sub_counts(int which, unsigned *calls,
                                         unsigned *routed, unsigned *unknown,
                                         unsigned *distinct)
{
    if (which < 0 || which > 1)
        return;
    if (calls)    *calls    = g_sub_calls[which];
    if (routed)   *routed   = g_sub_routed[which];
    if (unknown)  *unknown  = g_sub_unknown[which];
    if (distinct) *distinct = g_sub_distinct[which];
}

extern "C" unsigned port_mg_jump2_sub_row(int which, unsigned i, unsigned *code)
{
    if (which < 0 || which > 1 || i >= g_sub_distinct[which])
        return 0;
    if (code) *code = g_sub_seen[which][i];
    return g_sub_hits[which][i];
}

// ---- the one dispatch, feeding both censuses --------------------------------

/* Both lanes' counters tick here. The two sets are read by different seats and
   only one scene runs per process, so a tick charged by the other lane's scene
   can never reach this lane's report. Each arm keeps the semantics its lane
   shipped: BNP counts calls / hits / null-pmf and per-state hits (the per-state
   tick lives in obj_try, which is where it was); BNT counts calls / routed /
   unknown / distinct per SITE, with the saturation guard above. */
static void shared_call0(int which, void *self, unsigned code, int adj)
{
    ++g_obj_calls;
    ++g_sub_calls[which];
    if (code == 0)
        ++g_obj_nullpmf;
    if (code != 0 && adj == 0 && obj_try(self, code)) {
        ++g_obj_hits;
        ++g_sub_routed[which];
        sub_note(which, code);
        return;
    }
    ++g_sub_unknown[which];
    port_mg_call0(self, code, adj);
}

/* Retained entry point: lane BNP exported this and it is part of the API the
   merge ruling preserves. Nothing outside this file calls it today -- the two
   bodies below reach shared_call0 directly so they can name their site -- and
   an external caller has no site to name, so it is charged to site 0. */
extern "C" void port_mg_objstate_call0(void *self, unsigned code, int adj)
{
    shared_call0(0, self, code, adj);
}

extern "C" void port_mg_objstate_counts(unsigned *calls, unsigned *hits,
                                        unsigned *nullpmf)
{
    if (calls)   *calls   = g_obj_calls;
    if (hits)    *hits    = g_obj_hits;
    if (nullpmf) *nullpmf = g_obj_nullpmf;
}

extern "C" unsigned port_mg_objstate_count(void)
{ return (unsigned)kObjStateCount; }
extern "C" unsigned port_mg_objstate_addr(unsigned i)
{ return i < (unsigned)kObjStateCount ? kObjStates[i] : 0u; }
extern "C" unsigned port_mg_objstate_hit(unsigned i)
{ return i < (unsigned)kObjStateCount ? g_obj_state_hits[i] : 0u; }

// ---- the two host copies ---------------------------------------------------

/* src/func_ov006_020c4cd8.cpp. Its `struct Foo` carried nothing but the padding
   and the member pointer, so replacing the dispatch removes the struct and no
   layout moves: the two remaining calls keep src's offsets exactly. The
   receiver is `c + (p->adj >> 1)`, the ROM's own `add r0, r4, r1, asr#1` at
   0x020c4ce8 -- provably the bare `c` on every path this tree takes (routing is
   gated on adj == 0 and port_mg_call0 refuses a nonzero adjustment), and the
   faithful spelling is kept for the day one is measured. Site 0. */
// PORT_HOST_ABI: mwcc field pointer-to-member dispatch on the 8-byte pair at +0x30 MSVC's 4-byte pmf cannot reproduce
extern "C" void func_ov006_020c4cd8(char *c)
{
    {
        const MgPmf *p = (const MgPmf *)(c + 0x30);
        shared_call0(0, c + (p->adj >> 1), p->code, p->adj);
    }
    _ZN9Animation7AdvanceEv(c + 0x88);
    func_ov006_020c4c00(c);
}

/* src/func_ov006_020c7860.cpp. Same shape: `struct C { char pad[0x3c]; PMF m; }`
   is only there to hold the member pointer, and the dispatch sits between an
   unchanged prologue and an unchanged tail. Every other access in the TU is a
   raw char* offset and is copied verbatim. Same receiver reasoning as above,
   the ROM's sequence at 0x020c789c. Site 1. */
// PORT_HOST_ABI: mwcc field pointer-to-member dispatch on the 8-byte pair at +0x3c MSVC's 4-byte pmf cannot reproduce
extern "C" void func_ov006_020c7860(char *c)
{
    _Z14ApproachLinearRiii((int *)(c + 0x24), data_ov006_0213b010,
                           data_ov006_0213b018);
    AddVec3(c + 0x14, c + 0x20, c + 0x14);
    {
        const MgPmf *p = (const MgPmf *)(c + 0x3c);
        shared_call0(1, c + (p->adj >> 1), p->code, p->adj);
    }
    func_ov006_020bfec0(*(void **)&data_ov006_02141a40, c + 0x14,
                        (short *)(c + 0x36));
    func_ov006_020c76e0(c);
    _ZN9Animation7AdvanceEv(c + 0x9c);
}
