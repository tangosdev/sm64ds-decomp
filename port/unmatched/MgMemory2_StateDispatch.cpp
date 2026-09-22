// PORT_HOST_ABI. The mwcc POINTER-TO-MEMBER WALL, per-class half:
// dScMgMemory2_c's four TABLE dispatchers and its twenty-nine state addresses.
// Run mg6, lane MEM.  Actor id 0x16b, scene 363, "Memory Master".
//
// Read unmatched/MgBase_StateDispatch.cpp's header first, then
// unmatched/MgCoin_StateDispatch.cpp.  The first carries the ROM disassembly of
// the mwcc dispatch sequence and the finding that the overlay constructors
// already copy the pairs at the right stride; the second is the template this
// file follows.  Neither is repeated here.
//
// The three dispatchers that read a member pointer out of an OBJECT FIELD
// rather than out of a table are in unmatched/MgMemory2_FieldPmf.cpp, because
// they are a different defect (a four-byte field where the ROM has eight moves
// every field after it) and two of the three are shared ov006/ov004 glue rather
// than this class's.
//
// ---- 1. THE TWENTY-NINE ADDRESSES, AND WHERE THEY COME FROM ----------------
//
// src/__sinit_ov006_021314e4.c, this class's own overlay constructor, read
// assignment by assignment -- NOT by sweeping the pair address range, which is
// the trap port/mg_fanout_costs.txt section 4 names and which this class walks
// straight into: g_profile_MG_MEMORY_J sits at 0x0213d288, INSIDE the run of
// pair symbols (0x0213d280 is a pair, 0x0213d290 is the next one), so a sweep
// would find the factory word and the doubled id 0x016b016b as a "pair".  The
// constructor's own list skips it.
//
// Every pair was then read out of extracted/overlays/overlay_0006.bin at base
// 0x020bfec0.  Twenty-nine pairs, twenty-nine DISTINCT code words, and every
// adjustment word reads zero.  The listing is
// runs/mg6/out/MEM/state_pairs_16b.txt.
//
//   table                n  arity  dispatched by
//   -------------------  -  -----  -------------------------------------------
//   data_ov006_021423e0  5    0    _ZN14dScMgMemory2_c8BehaviorEv  (vtable slot 6)
//   data_ov006_02142440  10   0    _ZN14dScMgMemory2_c9StatePlayEv
//   data_ov006_021423c0  4    0    _ZN14dScMgMemory2_c11StateResultEv
//   data_ov006_02142408  7    1    _ZN14dScMgMemory2_c11UpdateCardsEv
//   data_ov006_021423a8  3    ?    NOTHING -- see section 3
//
// TWO OF THE FIVE TABLES ARE DISPATCHED BY A STATE OF ANOTHER TABLE.
// _ZN14dScMgMemory2_c9StatePlayEv is slot 1 of data_ov006_021423e0 and _ZN14dScMgMemory2_c11StateResultEv
// is slot 3 of it, so both appear in the arity-0 switch below AND are host
// copies further down this file.  That is the MgCoin_StateDispatch shape
// (0x020de26c and 0x020de440) and not a new one.
//
// ---- 2. THE ROM'S OWN DISPATCH SHAPES, DISASSEMBLED --------------------
//
// Read out of the shipped overlay image, not taken from src, because the state
// index offset and the `this` a callee is handed are the two things a wrong
// host copy gets silently wrong.
//
//   _ZN14dScMgMemory2_c8BehaviorEv  vtable slot 6, Behavior
//     add r0,r4,#0x5000 / ldr r0,[r0,#0x3d4]      the index at +0x53d4
//     ldr r1,[pc,#0x3c]                           = 0x021423e0
//     add r3,r1,r0,lsl #3                         stride EIGHT
//     ldr r1,[r3,#4] / ands r1,r1,#1 / ...        the ordinary PMF sequence
//     add r0,r4,r1,asr #1 / blx                   this = the class base
//     bl func_ov004_020b65e4
//     ldr r0,[pc,#0x14] (= 0x00004f38) / add r0,r4,r0 / bl func_ov006_020c19d0
//     mov r0,#1
//
//   _ZN14dScMgMemory2_c9StatePlayEv  index at +0x53d8, table 0x02142440, then a
//     `mov r0,r4 / bl 0x020f5c40` tail whose r0 IS the return value
//   _ZN14dScMgMemory2_c11StateResultEv  index at +0x53d8, table 0x021423c0, returns nothing
//   _ZN14dScMgMemory2_c11UpdateCardsEv  the one-argument loop:
//     mov r7,r0          r7 = the CLASS BASE, and it never changes
//     mov r5,r7          r5 = the per-record cursor
//     mov r6,#0          r6 = i
//     ldr r4,[pc,#0x54]  = 0x02142408
//   loop:
//     add r0,r5,#0x5000 / ldrb r1,[r0,#0x1bb]     the live flag
//     cmp r1,#0 / beq skip
//     ldrb r0,[r0,#0x1bc]                         the state byte
//     add r3,r4,r0,lsl #3                         stride EIGHT
//     ...the PMF sequence...
//     add r0,r7,r1,asr #1                         THIS = the class base
//     mov r1,r6                                   ARG  = i, the loop counter
//     blx r2
//     add r6,r6,#1 / cmp r6,#0x14 / add r5,r5,#0x18
//
// So the arity-1 callee is passed (class base, i) and NOT (record base, i);
// r5 is only ever used to fetch the two bytes.  The seven bodies in that table
// agree: five of them are spelled (char *c, int i) in their own src and the
// other two take (void) and are four bytes of `bx lr` in the ROM.
//
// ---- 3. ONE TABLE HAS NO DISPATCHER AND IS CARRIED, NOT ROUTED ------------
//
// data_ov006_021423a8 takes three pairs -- 0x020f58d0, 0x020f5744 and
// 0x020f5740 -- and port/mg_fanout_costs.txt section 3 measures that it has
// exactly ONE relocation pointing at it in the whole overlay, from 0x02131898,
// which is inside this class's own .init constructor.  The other four tables
// have two each: the constructor's, and their dispatching TU's literal-pool
// load.  So no code in ov006 names this table and its arity is derivable from
// nothing.
//
// THE THREE ADDRESSES ARE DELIBERATELY NOT IN EITHER SWITCH BELOW.  Two of the
// three are spelled (char *, int) in src and the third is a four-byte `bx lr`,
// which LOOKS like arity 1 -- and guessing on that basis is exactly what
// port/tools/stategen.py refuses to do at the arity step, correctly.  If a
// dispatch through a computed address ever reaches one of them, the framework's
// own UNHANDLED line names the address and the run is the evidence.  That is a
// better outcome than a switch arm that cannot be wrong because nothing calls
// it.
//
//     0x020f58d0   src/actors/dScMgMemory2_c.cpp   void (char *c, int i)
//     0x020f5744   src/actors/dScMgMemory2_c.cpp   void (char *base, int i)
//     0x020f5740   src/actors/dScMgMemory2_c.cpp   void (void), 4 bytes, bx lr
//
// ---- 4. ONE STATE HAS A src TU AND NO DELINK BLOCK ------------------------
//
// 0x020f6904 is slot 5 of data_ov006_02142440.  config/arm9/overlays/ov006/
// delinks.txt has a hole there -- the block before it ends at 0x020f6904 and
// the next starts at 0x020f6a00 -- so port/tools/stategen.py cannot emit a case
// for it and this file's case is hand-written.  src/actors/dScMgMemory2_c.cpp
// EXISTS and is bannered
//
//     NONMATCHING: different op / idiom (div=29). Logic verified correct vs
//     ROM; not byte-matchable from C at mwccarm 1.2/sp2p3.
//
// NONMATCHING TUs are routinely sliced and built by this port, so the state is
// reachable and the cost is one slice line plus one switch arm.  It is NOT a
// floor: port/mg_fanout_costs.txt's own correction to itself says so, and the
// contrast is curling's func_ov006_020e1854, which has no src in either
// extension.  THIS CLASS HAS NO HARD STATE FLOOR AT ALL -- all twenty-nine
// addresses reach a real src body, and the count that would say otherwise is
// reported by hal/scene_mg_memory2.cpp on every run so a regression is loud.
//
// ---- 5. WHY THIS FILE HAS ITS OWN ENTRY POINTS ----------------------------
//
// unmatched/MgBase_StateDispatch.cpp owns port_mg_call0 and port_mg_call1 and
// chains them to exactly one per-class pair, port_mg_try_ov006_0 and _1, which
// unmatched/MgCurling_StateDispatch.cpp defines.  A second class cannot define
// those names, so this file calls port_mg_memory2_call0 / _call1, which try
// THIS class's switch and hand everything else to the framework unchanged --
// the shape MgCoin_StateDispatch.cpp, MgLuigi_StateDispatch.cpp and
// MgPachinko_StateDispatch.cpp all use.  The framework therefore remains the
// single place that decides what a null code word means, what a nonzero
// adjustment means and how an unhandled address is reported.

#include <cstdio>
#include <cstdlib>   /* std::abort, for the boot installer below (lane PMFB4) */

/* The eight-byte mwcc member pointer, in the only spelling that is true on both
   machines: two words, no member-pointer type anywhere. */
struct MgPmf { unsigned code; int adj; };

extern "C" {

/* the framework's entry points; see MgBase_StateDispatch.cpp */
void port_mg_call0(void *self, unsigned code, int adj);
void port_mg_call1(void *self, unsigned code, int adj, int a);

/* ---- the twenty-six routed state bodies, in address order ----------------
   Each is declared with the parameter list ITS OWN src TU defines, so a
   ride-through is called the way the ROM calls it rather than the way the
   slot's arity would suggest.  The two (void) ones in the arity-1 table are
   four-byte `bx lr` bodies -- there is nothing for an ignored argument to be
   wrong about, which is the MgCoin_StateDispatch.cpp ruling for the same
   shape.  0x020f7234 and 0x020f71c8 are host copies further down this file. */
void  _ZN14dScMgMemory2_c11CardFlyAwayEi(void *ctx, int idx);
void  _ZN14dScMgMemory2_c12CardFlipDownEi(char *c, int i);
void  _ZN14dScMgMemory2_c8CardWaitEi(void);              /* one-argument slot, bx lr body */
void  _ZN14dScMgMemory2_c10CardFlipUpEi(char *base, int idx);
void  _ZN14dScMgMemory2_c10CardSelectEi(char *self, int idx);
void  _ZN14dScMgMemory2_c8CardIdleEi(void);              /* one-argument slot, bx lr body */
void  _ZN14dScMgMemory2_c8CardMoveEi(char *self, int i);
void  _ZN14dScMgMemory2_c12ResultFinishEv(char *p);
void  _ZN14dScMgMemory2_c15ResultTurnCardsEv(char *c);
void  _ZN14dScMgMemory2_c12ResultRewardEv(char *c);
void  _ZN14dScMgMemory2_c10ResultWaitEv(char *c);
void  _ZN14dScMgMemory2_c11RoundRevealEv(char *c);
void *_ZN14dScMgMemory2_c13RoundWaitDealEv(char *c);
void  _ZN14dScMgMemory2_c15RoundReadyCardsEv(char *c);
void  _ZN14dScMgMemory2_c14RoundHideCardsEv(char *o);
void  _ZN14dScMgMemory2_c14RoundShowCardsEv(char *c);           /* NONMATCHING src, section 4 */
void  _ZN14dScMgMemory2_c15RoundDealFourthEv(char *thiz);
void  _ZN14dScMgMemory2_c13RoundDealHardEv(char *c);
void  _ZN14dScMgMemory2_c15RoundDealNormalEv(char *thiz);
void  _ZN14dScMgMemory2_c13RoundDealEasyEv(char *thiz);
void  _ZN14dScMgMemory2_c10RoundStartEv(char *c);
void  _ZN14dScMgMemory2_c9StateExitEv(char *self);
void  _ZN14dScMgMemory2_c10StateJudgeEv(void *c);
void  _ZN14dScMgMemory2_c10StateSetupEv(char *c);

/* the four mount tables this file dispatches, re-typed to the ROM's eight-byte
   pair.  The ov006 mount defines the storage; __sinit_ov006_021314e4 fills it
   at minigame scene load.  data_ov006_021423a8 is deliberately absent -- see
   section 3; nothing here reads it. */
extern MgPmf data_ov006_021423c0[];
extern MgPmf data_ov006_021423e0[];
extern MgPmf data_ov006_02142408[];
extern MgPmf data_ov006_02142440[];

/* the ordinary callees the host copies below keep, each spelled as its own src
   TU spells it */
void func_ov004_020b65e4(void);
void func_ov006_020c19d0(void *c);

/* host-copied further down this file, and called from above their own
   definitions -- 020f7234 and 020f71c8 are STATE BODIES as well as dispatchers,
   and 020f5c40 is 020f7234's tail call. */
void _ZN14dScMgMemory2_c9StatePlayEv(void *c);
void _ZN14dScMgMemory2_c11StateResultEv(void *c);
void _ZN14dScMgMemory2_c11UpdateCardsEv(void *c);

/* the boot installer at the end of this file; hal/scene_mg.cpp calls it after
   the ov006 constructors have filled the tables. */
void port_mg_memory2_states_seat(void);

}  /* extern "C" */

// ---- the class's address switch --------------------------------------------

static unsigned g_mem2_state_hits;
/* THE BODILESS-STATE COUNTER, KEPT AND STRUCTURALLY ZERO. Every address in the
   two switches below reaches a real symbol, so nothing increments this. It is
   kept rather than deleted so hal/scene_mg_memory2.cpp's census field keeps its
   meaning: if a later lane ever adds a state this class cannot reach, this is
   where it is counted, and a nonzero reading is a regression rather than a new
   field. */
static unsigned g_mem2_floor_hits;
/* How often the NONMATCHING state of section 4 ran, counted on its own so the
   run can say the hole is closed rather than merely unreported. */
static unsigned g_mem2_nonmatching_calls;

static int mem2_try_0(void *self, unsigned code)
{
    char *c = (char *)self;
    switch (code) {
    /* data_ov006_021423e0, dispatched by vtable slot 6 */
    case 0x020f7280u: _ZN14dScMgMemory2_c10StateSetupEv(c); return 1;
    case 0x020f7234u: _ZN14dScMgMemory2_c9StatePlayEv(c); return 1;   /* host copy below */
    case 0x020f7210u: _ZN14dScMgMemory2_c10StateJudgeEv(c); return 1;
    case 0x020f71c8u: _ZN14dScMgMemory2_c11StateResultEv(c); return 1;   /* host copy below */
    case 0x020f7190u: _ZN14dScMgMemory2_c9StateExitEv(c); return 1;
    /* data_ov006_02142440, dispatched by _ZN14dScMgMemory2_c9StatePlayEv */
    case 0x020f6bf0u: _ZN14dScMgMemory2_c10RoundStartEv(c); return 1;
    case 0x020f6b78u: _ZN14dScMgMemory2_c13RoundDealEasyEv(c); return 1;
    case 0x020f6b00u: _ZN14dScMgMemory2_c15RoundDealNormalEv(c); return 1;
    case 0x020f6a78u: _ZN14dScMgMemory2_c13RoundDealHardEv(c); return 1;
    case 0x020f6a00u: _ZN14dScMgMemory2_c15RoundDealFourthEv(c); return 1;
    case 0x020f6904u: ++g_mem2_nonmatching_calls;
                      _ZN14dScMgMemory2_c14RoundShowCardsEv(c); return 1;   /* section 4 */
    case 0x020f6830u: _ZN14dScMgMemory2_c14RoundHideCardsEv(c); return 1;
    case 0x020f67a0u: _ZN14dScMgMemory2_c15RoundReadyCardsEv(c); return 1;
    case 0x020f670cu: _ZN14dScMgMemory2_c13RoundWaitDealEv(c); return 1;
    case 0x020f6678u: _ZN14dScMgMemory2_c11RoundRevealEv(c); return 1;
    /* data_ov006_021423c0, dispatched by _ZN14dScMgMemory2_c11StateResultEv */
    case 0x020f6538u: _ZN14dScMgMemory2_c10ResultWaitEv(c); return 1;
    case 0x020f6488u: _ZN14dScMgMemory2_c12ResultRewardEv(c); return 1;
    case 0x020f639cu: _ZN14dScMgMemory2_c15ResultTurnCardsEv(c); return 1;
    case 0x020f6230u: _ZN14dScMgMemory2_c12ResultFinishEv(c); return 1;
    default:                                  return 0;
    }
}

static int mem2_try_1(void *self, unsigned code, int a)
{
    char *c = (char *)self;
    switch (code) {
    /* data_ov006_02142408, dispatched by _ZN14dScMgMemory2_c11UpdateCardsEv.  `c` is the
       CLASS BASE and `a` is the loop counter, in that order; section 2 has the
       disassembly that says so. */
    case 0x020f6088u: _ZN14dScMgMemory2_c8CardMoveEi(c, a);  return 1;
    case 0x020f6084u: _ZN14dScMgMemory2_c8CardIdleEi();      return 1;  /* bx lr body */
    case 0x020f5f0cu: _ZN14dScMgMemory2_c10CardSelectEi(c, a);  return 1;
    case 0x020f5e74u: _ZN14dScMgMemory2_c10CardFlipUpEi(c, a);  return 1;
    case 0x020f5e70u: _ZN14dScMgMemory2_c8CardWaitEi();      return 1;  /* bx lr body */
    case 0x020f5de0u: _ZN14dScMgMemory2_c12CardFlipDownEi(c, a);  return 1;
    case 0x020f5cb4u: _ZN14dScMgMemory2_c11CardFlyAwayEi(c, a);  return 1;
    default:                                      return 0;
    }
}

/* The two entry points the host copies below use.  Everything this switch does
   not own goes to the framework unchanged, so the null-code guard, the
   nonzero-adjustment refusal and the UNHANDLED report all still live in exactly
   one place. */
extern "C" void port_mg_memory2_call0(void *self, unsigned code, int adj)
{
    if (code != 0 && adj == 0 && mem2_try_0(self, code)) {
        ++g_mem2_state_hits;
        return;
    }
    port_mg_call0(self, code, adj);
}

extern "C" void port_mg_memory2_call1(void *self, unsigned code, int adj, int a)
{
    if (code != 0 && adj == 0 && mem2_try_1(self, code, a)) {
        ++g_mem2_state_hits;
        return;
    }
    port_mg_call1(self, code, adj, a);
}

extern "C" unsigned port_mg_memory2_state_hits(void)     { return g_mem2_state_hits; }
extern "C" unsigned port_mg_memory2_floor_hits(void)     { return g_mem2_floor_hits; }
extern "C" unsigned port_mg_memory2_nonmatching(void)    { return g_mem2_nonmatching_calls; }

// ---- the four host copies --------------------------------------------------
//
// Each is its src TU verbatim except for the table declaration (MgPmf rather
// than a member-pointer type) and the dispatch site (port_mg_memory2_callN
// rather than `(c->*table[i].pmf)()`).  Where anything else moved it is stated
// on the line.

/* src/_ZN14dScMgMemory2_c9StatePlayEv -- RETIRED, run link100 lane SEAT4. Its table is
   seated in port/hal/pmf_seat4.cpp and the matched TU is on
   port/slice_seat4.txt, so the host copy that stood in for it is gone and
   the declaration above is what the faces in this file reach. */

/* src/_ZN14dScMgMemory2_c11StateResultEv -- RETIRED, run link100 lane SEAT4. Its table is
   seated in port/hal/pmf_seat4.cpp and the matched TU is on
   port/slice_seat4.txt, so the host copy that stood in for it is gone and
   the declaration above is what the faces in this file reach. */

// ---- TWO TABLES SEATED, AND TWELVE FACES -----------------------------------
//
// Run link100 lane PMFB4. Two of dScMgMemory2_c's tables now hold HOST
// addresses, written at boot by port_mg_memory2_states_seat below after every
// cell has been compared against the ROM's own code word and a zero adjustment
// word, so two of the four host copies are gone:
//
//   _ZN14dScMgMemory2_c8BehaviorEv  data_ov006_021423e0   5 slots  arity 0  (vtable slot 6)
//   _ZN14dScMgMemory2_c11UpdateCardsEv  data_ov006_02142408   7 slots  arity 1
//
// THE TWO HOST COPIES THAT STAY are _ZN14dScMgMemory2_c9StatePlayEv and _ZN14dScMgMemory2_c11StateResultEv,
// STATE BODIES of data_ov006_021423e0 rather than dispatchers of their own
// table; the installer writes faces that call them. The switch stays live for
// the tables this lane did not seat, including the arm that counts the
// NONMATCHING body at 0x020f6904, which is in neither seated table and is
// untouched.
//
// THE STRIDE, BOTH SIDES (runs/link100/out/PMFB4/rom_gate3.txt, emit_gate3.txt):
//   020f7458  ROM add r3,r1,r0,lsl #3 at 020f746c, pool 020f74ac = 021423e0
//   020f5c40  ROM add r3,r4,r0,lsl #3 at 020f5c6c, pool 020f5cb0 = 02142408
// emitted [eax*8] and [eax*8+4] in both listings; /Zp4 a no-op on both.
//
// THE TWELVE SOURCE PAIRS all read {code, 0} in overlay_0006.bin at the
// addresses src/__sinit_ov006_021314e4.c copies each slot from. THAT SINIT USES
// A THIRD WHOLE-PAIR SPELLING and it is worth naming, because a reader that only
// knows the other two counts zero pairs here and calls the table unfillable:
// `data_ov006_TAB.p[N] = data_ov006_SRC;`, an assignment of the whole eight-byte
// Pair into element N of an array member, which is the same whole-pair copy as
// `TAB[N] = SRC` and as the `.pN =` form the Bomroom and Luigi constructors use.
// A field-form fill would be `.p[N].a =`, and there is not one here:
//
//   021423e0[0] <- 0213d268 020f7280/0    02142408[0] <- 0213d308 020f6088/0
//   021423e0[1] <- 0213d250 020f7234/0    02142408[1] <- 0213d270 020f6084/0
//   021423e0[2] <- 0213d2c0 020f7210/0    02142408[2] <- 0213d2b0 020f5f0c/0
//   021423e0[3] <- 0213d300 020f71c8/0    02142408[3] <- 0213d2f0 020f5e74/0
//   021423e0[4] <- 0213d278 020f7190/0    02142408[4] <- 0213d2e8 020f5e70/0
//                                         02142408[5] <- 0213d2e0 020f5de0/0
//                                         02142408[6] <- 0213d2d8 020f5cb4/0
//
// THE DISPATCH SHAPE: 020f5c40 pushes edi at both of its two indirect call sites
// (peeled first iteration, both read), 020f7458 pushes nothing; both are
// receiver-in-ecx with callee cleanup.
//
// ONE /alternatename: ?data_ov006_02142408@@3PAP8C77@@AEXH@ZA, read off the
// object with dumpbin /symbols. src/actors/dScMgMemory2_c.cpp declares its table
// inside extern "C".
//
// ONE GUESS MARKER, ADJUDICATED: src/actors/dScMgMemory2_c.cpp, ruled REAL_DECOMP
// at port/tools/inferred_stub_adjudicated.txt:326.
//
// STALE COMMENT DISCLOSED, NOT EDITED: hal/scene_mg_memory2.cpp:188-192 still
// calls _ZN14dScMgMemory2_c8BehaviorEv "the HOST COPY".
#pragma comment(linker, "/alternatename:?data_ov006_02142408@@3PAP8C77@@AEXH@ZA=_data_ov006_02142408")

#define M2_FACE1(sym, cast)                                                   \
    static void __fastcall m2_##sym(void *self, void *dead_edx, int i)        \
    {                                                                         \
        (void)dead_edx;                                                       \
        ++g_mem2_state_hits;                                                  \
        sym(cast self, i);                                                    \
    }
#define M2_FACE1_VOID(sym)                                                    \
    static void __fastcall m2_##sym(void *self, void *dead_edx, int i)        \
    {                                                                         \
        (void)self; (void)dead_edx; (void)i;                                  \
        ++g_mem2_state_hits;                                                  \
        sym();                                                                \
    }
#define M2_FACE0(sym, cast)                                                   \
    static void __fastcall m2_##sym(void *self, void *dead_edx)               \
    {                                                                         \
        (void)dead_edx;                                                       \
        ++g_mem2_state_hits;                                                  \
        sym(cast self);                                                       \
    }

/* data_ov006_021423e0, arity 0. Slots 1 and 3 are host copies above. */
M2_FACE0(_ZN14dScMgMemory2_c10StateSetupEv, (char *))
M2_FACE0(_ZN14dScMgMemory2_c9StatePlayEv, (char *))
M2_FACE0(_ZN14dScMgMemory2_c10StateJudgeEv, (char *))
M2_FACE0(_ZN14dScMgMemory2_c11StateResultEv, (char *))
M2_FACE0(_ZN14dScMgMemory2_c9StateExitEv, (char *))
/* data_ov006_02142408, arity 1 */
M2_FACE1(_ZN14dScMgMemory2_c8CardMoveEi, (char *))
M2_FACE1_VOID(_ZN14dScMgMemory2_c8CardIdleEi)
M2_FACE1(_ZN14dScMgMemory2_c10CardSelectEi, (char *))
M2_FACE1(_ZN14dScMgMemory2_c10CardFlipUpEi, (char *))
M2_FACE1_VOID(_ZN14dScMgMemory2_c8CardWaitEi)
M2_FACE1(_ZN14dScMgMemory2_c12CardFlipDownEi, (char *))
M2_FACE1(_ZN14dScMgMemory2_c11CardFlyAwayEi, (char *))

/* run link100 lane SEAT4: this class's remaining state tables are
   seated in port/hal/pmf_seat4.cpp, from inside this installer, so the
   seat order hal/scene_mg.cpp already establishes is the one they get
   and no new call site is added anywhere. */
extern "C" void port_pmf_seat4_memory2(void);

extern "C" void port_mg_memory2_states_seat(void)
{
    static int done;
    if (done)
        return;
    done = 1;

    port_pmf_seat4_memory2();

    static const struct {
        MgPmf *table;
        const char *name;
        unsigned slot;
        unsigned rom;
        void *face;
    } seats[] = {
        {data_ov006_021423e0, "021423e0", 0, 0x020f7280u, (void *)m2__ZN14dScMgMemory2_c10StateSetupEv},
        {data_ov006_021423e0, "021423e0", 1, 0x020f7234u, (void *)m2__ZN14dScMgMemory2_c9StatePlayEv},
        {data_ov006_021423e0, "021423e0", 2, 0x020f7210u, (void *)m2__ZN14dScMgMemory2_c10StateJudgeEv},
        {data_ov006_021423e0, "021423e0", 3, 0x020f71c8u, (void *)m2__ZN14dScMgMemory2_c11StateResultEv},
        {data_ov006_021423e0, "021423e0", 4, 0x020f7190u, (void *)m2__ZN14dScMgMemory2_c9StateExitEv},

        {data_ov006_02142408, "02142408", 0, 0x020f6088u, (void *)m2__ZN14dScMgMemory2_c8CardMoveEi},
        {data_ov006_02142408, "02142408", 1, 0x020f6084u, (void *)m2__ZN14dScMgMemory2_c8CardIdleEi},
        {data_ov006_02142408, "02142408", 2, 0x020f5f0cu, (void *)m2__ZN14dScMgMemory2_c10CardSelectEi},
        {data_ov006_02142408, "02142408", 3, 0x020f5e74u, (void *)m2__ZN14dScMgMemory2_c10CardFlipUpEi},
        {data_ov006_02142408, "02142408", 4, 0x020f5e70u, (void *)m2__ZN14dScMgMemory2_c8CardWaitEi},
        {data_ov006_02142408, "02142408", 5, 0x020f5de0u, (void *)m2__ZN14dScMgMemory2_c12CardFlipDownEi},
        {data_ov006_02142408, "02142408", 6, 0x020f5cb4u, (void *)m2__ZN14dScMgMemory2_c11CardFlyAwayEi},
    };

    for (unsigned i = 0; i < sizeof seats / sizeof seats[0]; ++i) {
        MgPmf *p = &seats[i].table[seats[i].slot];
        if (p->code != seats[i].rom || p->adj != 0) {
            std::fprintf(stderr, "FATAL: dScMgMemory2_c state table %s slot %u: "
                         "the sinit left %08x/%d, the ROM's own pairs say "
                         "%08x/0 -- WRONG BYTES\n", seats[i].name,
                         seats[i].slot, p->code, p->adj, seats[i].rom);
            std::abort();
        }
        p->code = (unsigned)(size_t)seats[i].face;
    }
}
