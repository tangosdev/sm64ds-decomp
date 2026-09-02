// PORT_HOST_ABI. dScMgPanel_c's pointer-to-member state machine, host-copied.
// Run mg6, lane PPP. actor id 0x17c = scene 380.
//
// ---- WHY THIS FILE IS TWICE THE SIZE OF CURLING'S -------------------------
//
// port/mg_fanout_costs.txt section 4 costs the wall at "five dispatching TUs
// to host-copy" per minigame, measured on dScMgCurling_c. THIS CLASS HAS
// ELEVEN -- ten when run mg6 lane PPP seated it, and an eleventh the day run
// mg7 lane L380 decompiled func_ov006_02106ca4 and found it was a dispatcher
// as well as a state -- and the reason is structural rather than incidental:
// dScMgPanel_c runs a TWO-LEVEL state machine. Its Behavior (slot 6) dispatches
// data_ov006_02142888, and FIVE of that table's own eight states are
// themselves dispatchers of a second table, data_ov006_02142840. So the
// closure walk reaches the second level only THROUGH the first, and a lane
// that host-copied the Behavior alone would link, boot, and jump to a raw DS
// address on the first frame the outer state advanced.
//
//   level 1  data_ov006_02142888  8 slots  arity 0  <- slot 6, the Behavior
//   level 2  data_ov006_02142840  4 slots  arity 1  <- five level-1 states
//            data_ov006_02142820  4 slots  arity 0
//            data_ov006_02142860  5 slots  arity 0
//            data_ov006_021427ec  3 slots  arity 0
//            data_ov006_021427bc  3 slots  arity 1
//
// TWENTY-SEVEN PAIRS, ALL TWENTY-SEVEN WITH A ZERO ADJUSTMENT WORD, read out
// of extracted/overlays/overlay_0006.bin at base 0x020bfec0 one assignment at
// a time from src/__sinit_ov006_02131fa4.c -- NOT by sweeping the .data span.
// The span 0x0213dc4c..0x0213dd2c holds 57 eight-byte slots and the
// constructor names 27 of them; 0x0213dc64 inside that range is
// MgPuzzlePanelPuzzlePanic_SpawnInfo itself, which is exactly the phantom
// section 4 warns a sweep manufactures.
//
// ---- THREE OF THE ELEVEN ARE THE THIRD SHAPE, AND ONE IS THE BEHAVIOR -----
//
// port/mg_fanout_costs.txt section 4's FLW amendment names one TU in the whole
// tree that open-codes the ARM Itanium sequence in plain ints,
// func_ov006_020c3d18, and says the pair of prescribed detectors -- the link
// and a `::*` source sweep -- both read clean over it. THIS CLASS HAS THREE
// MORE OF THEM, and one is vtable slot 6:
//
//   src/func_ov006_02107358.cpp   table 02142888, arity 0, VTABLE SLOT 6
//   src/func_ov006_02106bc0.c     table 02142840, arity 1
//   src/func_ov006_02106ca4.c     table 02142840, arity 1, AND A STATE --
//                                 slot 6 of data_ov006_02142888. Run mg7 lane
//                                 L380; the header said TWO before it landed.
//
// All three declare `struct Ent { int a; int b; }` / `struct Pmf { int off;
// int adj; }`, read the pair as two ordinary ints and do their own decoding,
// so there is no member-pointer type for a source sweep to match and no
// mangled global for a link to fail on. The ROM at 0x02107358, read out of the
// shipped overlay image:
//
//     ldr   r1,[r3,#4]        the adjustment
//     add   r0,r4,r1,asr#1    this, advanced, arithmetic shift
//     ands  r1,r1,#1          the virtual bit, in the LSB
//     ldrne r2,[r0] / ldrne r1,[r3] / ldrne r1,[r2,r1]
//     ldreq r1,[r3]
//     blx   r1
//
// one for one with what the src spells. THE STRIDE IS ALREADY RIGHT in all
// three -- eight bytes on MSVC is eight bytes in the ROM, which is section 4's
// rulebook corollary -- so what is wrong is only the CODE WORD, which is a DS
// address. That is why the aliases onto the six tables stay ordinary aliases
// and only the DECODE is replaced here.
//
// ROUTE AT THE DISPATCH SITE, NEVER BY REWRITING THE STORED PAIR. The ov085 /
// ov100 shape would write host addresses over the .data source pairs before
// the constructor copies them. It is wrong for the same reason section 4 gives
// for func_ov006_020c3d18 and for one more that is this class's own: the pairs
// live in ov006 .data inside a span whose neighbours belong to other classes,
// and __sinit_ov006_02131fa4 is not the only constructor reading that region.
// The stored pair keeps the ROM's own words.
//
// ---- THE EIGHT TYPED ONES, AND WHY EVERY ONE NEEDS A COPY -----------------
//
// The other eight name a member-pointer type, so section 4's rulebook applies
// verbatim: "a pair whose consumer names a member-pointer type needs a host
// copy, whether it is called or only copied". Three spell it through a struct
// wrapper (the @@3PAUEntry@@A spelling port/tools/facegen.py does NOT refuse,
// tool finding 1) and five through a bare typedef:
//
//   src/func_ov006_02104ac4.cpp   Entry { PMF pmf[1]; }   021427bc  arity 1
//   src/func_ov006_02104c60.cpp   bare PMF               021427ec  arity 0
//   src/func_ov006_021050bc.cpp   Entry { PMF pmf; }      02142860  arity 0
//   src/func_ov006_021057f0.cpp   bare PMF               02142820  arity 0
//   src/func_ov006_02106eb8.cpp   bare PMF               02142840  arity 1
//   src/func_ov006_02106f44.cpp   bare PMF               02142840  arity 1
//   src/func_ov006_02106fdc.c     bare PMF (//cpp)       02142840  arity 1
//   src/func_ov006_0210709c.cpp   bare PMF               02142840  arity 1
//
// NONE OF THE ELEVEN HOLDS A MEMBER POINTER AS AN OBJECT FIELD -- all six
// tables are external -- so the four-byte-versus-eight-byte layout shift that
// makes section 4's ov004 group read the wrong field does not apply, and the
// src offsets and the ROM's agree. EVERY OFFSET BELOW WAS STILL READ OFF THE
// ROM DISASSEMBLY rather than off the src struct, and each body's comment
// states the instructions it came from so the check is visible, not asserted.
//
// ---- WHAT THIS CLASS CANNOT REACH -----------------------------------------
//
// ONE HARD FLOOR IS LEFT, and it used to be two. Run mg7 lane L380 decompiled
// 0x02106ca4 -- slot 6 of data_ov006_02142888, the round-end state that a
// 1200-frame mg6 run asked for 951 times -- and src/func_ov006_02106ca4.c
// byte-matches at mwccarm 1.2/sp2p3. What remains:
//
//   0x021053a8   slot 2 of data_ov006_02142820
//
// port/tools/stategen.py still reports it under REFUSALS as "HAS NO DECOMPILED
// BODY". It is emitted below as a REPORTING case and never as a call, and no
// symbol is invented for it. That is curling's func_ov006_020e1854 shape
// before lane CT1 transcribed it; this lane does NOT transcribe, and says so.
//
// DECOMPILING 02106ca4 DID NOT PRODUCE A SLICE LINE, IT PRODUCED AN ELEVENTH
// HOST COPY. The matched body dispatches data_ov006_02142840 at arity 1 with
// the open-coded Itanium decode -- the same third shape as 02107358 and
// 02106bc0 -- so the src TU links and both prescribed detectors read clean
// over it while its first dispatch would jump to a raw DS address. It is
// carried below, and it is NOT a line in port/slice_ppp.txt.
//
// TWO MORE WERE STATEGEN JOIN MISSES AND ARE NOT FLOORS: 0x02106aa8 and
// 0x02106fdc are outside the delinks join but src/func_ov006_02106aa8.c and
// src/func_ov006_02106fdc.c both exist, so both are reachable. 02106aa8 joins
// port/slice_ppp.txt as an ordinary line; 02106fdc is one of the eleven host
// copies below, because it is a dispatcher as well as a state.

#include <cstdio>

/* The eight-byte mwcc member pointer, in the only spelling that is true on
   both machines: two words, no member-pointer type anywhere. */
struct MgPmf { unsigned code; int adj; };

extern "C" {

/* the framework's entry points; see unmatched/MgBase_StateDispatch.cpp */
void port_mg_call0(void *self, unsigned code, int adj);
void port_mg_call1(void *self, unsigned code, int adj, int a);

/* ---- the six state tables, re-typed --------------------------------------
   The ov006 mount defines the storage and __sinit_ov006_02131fa4 fills it.
   Each is spelled here as an array of two plain ints, which is eight bytes on
   MSVC and eight bytes in the ROM -- section 4's rulebook corollary, and the
   reason the mount's tables have been correct all along while only the
   consumers were wrong. */
extern MgPmf data_ov006_021427bc[];
extern MgPmf data_ov006_021427ec[];
extern MgPmf data_ov006_02142820[];
extern MgPmf data_ov006_02142840[];
extern MgPmf data_ov006_02142860[];
extern MgPmf data_ov006_02142888[];

/* ---- the twenty-five reachable state bodies ------------------------------
   Reached ONLY through the switches below: the pair words are mounted DATA
   holding DS addresses, so nothing else in the build names them and /OPT:REF
   would drop every one of them without this file. The eighteen that are not
   defined in this file join port/slice_ppp.txt in the same commit. */

/* arity 0 */
void func_ov006_021071d4(char *c);
void func_ov006_0210713c(char *c);
void func_ov006_02105730(char *c);
void func_ov006_02105670(char *c);
void func_ov006_021051dc(char *c);
void func_ov006_0210508c(char *c);
void func_ov006_0210500c(char *c);
void func_ov006_02104fb4(unsigned char *c);
void func_ov006_02104ecc(char *c);
void func_ov006_02104ec8(void);          /* empty body, no argument */
void func_ov006_02104c08(char *c);
void func_ov006_02104bb0(char *c);
void func_ov006_02104bac(void);          /* empty body, no argument */

/* arity 1 */
void func_ov006_02106bac(char *c, int i);
void func_ov006_02106aa8(char *c, int i);
void func_ov006_02106a08(char *c, int i);
void func_ov006_02106910(char *c, int i);
void func_ov006_02104ac0(void);          /* empty body, no argument */
void func_ov006_02104a10(char *c, int i);
void func_ov006_02104920(char *c, int i);

/* the non-dispatching callees the eleven host copies below reach */
void func_ov006_02104c60(void *c);
void func_ov006_021050bc(void *c);
void func_ov006_021057f0(void *c);
void func_ov006_02105134(void *c);
void func_ov006_02105854(void *c);
void func_ov006_02105c88(void *c);
void func_ov006_02105d20(void *c);
void func_ov006_02105de4(void *c);
void func_ov006_02104354(void *c);
void func_ov006_02104ac4(void *c);
void func_ov006_02104ea8(char *c);
void func_ov004_020b0a54(int c);
/* the round-end state's own callees, run mg7 lane L380 */
void func_ov006_02104580(char *c);
void func_ov006_02104870(char *c);
void func_ov006_021068d8(char *c);
void func_ov004_020adb1c(int n);
extern unsigned char data_020a0e40;
extern unsigned char data_020a0de8[];
extern unsigned char data_020a0de9[];
extern char *data_ov004_020beb68;

/* the eleven host copies this file DEFINES, so the seat can name them */
int  func_ov006_02107358(char *c);
void func_ov006_02106ca4(char *c);
void func_ov006_02106bc0(char *c);
void func_ov006_02106eb8(char *c);
void func_ov006_02106f44(char *c);
void func_ov006_02106fdc(void *c);
void func_ov006_0210709c(void *c);

void port_mg_panel_counts(unsigned *hits, unsigned *floor, unsigned *unknown);

}  /* extern "C" */

/* THE WITNESS, AND THE THREE COUNTS ARE DISJOINT ON PURPOSE. A hit is a state
   this class ROUTED TO A REAL BODY; a floor entry is one of the two addresses
   with no decompiled body; unknown is a code word neither switch knows, which
   is the number that convicts a missed dispatcher. The first version of this
   counted a floor entry as a hit as well, so a 1200-frame run read "2508
   routed" when 951 of those had gone nowhere -- the switches return -1 for a
   floor now, and only +1 counts as a hit. */
static unsigned g_panel_hits;
static unsigned g_panel_floor_2820;
static unsigned g_panel_unknown;

static int panel_try_0(void *self, unsigned code)
{
    char *c = (char *)self;
    switch (code) {
    /* data_ov006_02142888, the Behavior's own eight */
    case 0x021071d4u: func_ov006_021071d4(c);       return 1;
    case 0x0210713cu: func_ov006_0210713c(c);       return 1;
    case 0x0210709cu: func_ov006_0210709c(c);       return 1;
    case 0x02106fdcu: func_ov006_02106fdc(c);       return 1;
    case 0x02106f44u: func_ov006_02106f44(c);       return 1;
    case 0x02106eb8u: func_ov006_02106eb8(c);       return 1;
    case 0x02106ca4u: func_ov006_02106ca4(c);       return 1;
    case 0x02106bc0u: func_ov006_02106bc0(c);       return 1;
    /* data_ov006_02142820 */
    case 0x02105730u: func_ov006_02105730(c);       return 1;
    case 0x02105670u: func_ov006_02105670(c);       return 1;
    case 0x021051dcu: func_ov006_021051dc(c);       return 1;
    /* data_ov006_02142860 */
    case 0x0210508cu: func_ov006_0210508c(c);       return 1;
    case 0x0210500cu: func_ov006_0210500c(c);       return 1;
    case 0x02104fb4u: func_ov006_02104fb4((unsigned char *)c); return 1;
    case 0x02104eccu: func_ov006_02104ecc(c);       return 1;
    case 0x02104ec8u: func_ov006_02104ec8();        return 1;
    /* data_ov006_021427ec */
    case 0x02104c08u: func_ov006_02104c08(c);       return 1;
    case 0x02104bb0u: func_ov006_02104bb0(c);       return 1;
    case 0x02104bacu: func_ov006_02104bac();        return 1;

    /* ---- THE ONE REMAINING FLOOR, REPORTED AND NEVER CALLED -------------
     *
     * REPORTED ONCE, not once per entry. 0x021053a8 is a SUB-state: a class
     * that asks for it loses one tick and carries on, so a per-entry line
     * would put identical rows in the shipped playlog and say nothing the
     * count does not. The count is the witness; this is the name.
     *
     * ITS SIBLING IS GONE. The 951-ask entry this switch used to carry for
     * 0x02106ca4 was the other half of that pair, and it is retired: run mg7
     * lane L380 decompiled the body, and it is a real case above. */
    case 0x021053a8u:
        if (++g_panel_floor_2820 == 1)
            std::fprintf(stderr, "  [scene] dScMgPanel_c FLOOR: state "
                         "0x021053a8 (slot 2 of data_ov006_02142820) has no "
                         "delink block and no src file. The state is not "
                         "entered. (Reported once; the count is in the run "
                         "report.)\n");
        return -1;
    default:
        return 0;
    }
}

static int panel_try_1(void *self, unsigned code, int a)
{
    char *c = (char *)self;
    switch (code) {
    /* data_ov006_02142840, dispatched at arity 1 by five level-1 states */
    case 0x02106bacu: func_ov006_02106bac(c, a); return 1;
    case 0x02106aa8u: func_ov006_02106aa8(c, a); return 1;
    case 0x02106a08u: func_ov006_02106a08(c, a); return 1;
    case 0x02106910u: func_ov006_02106910(c, a); return 1;
    /* data_ov006_021427bc */
    case 0x02104ac0u: func_ov006_02104ac0();     return 1;
    case 0x02104a10u: func_ov006_02104a10(c, a); return 1;
    case 0x02104920u: func_ov006_02104920(c, a); return 1;
    default:
        return 0;
    }
}

/* THE TWO ENTRY POINTS the host copies below call. A hit is this class's; a
   miss falls through to the framework, which owns the guards and the report,
   and is counted here as well because a nonzero unknown count is the number
   that says a dispatcher was missed. */
extern "C" void port_mg_panel_call0(void *self, unsigned code, int adj)
{
    if (code != 0 && adj == 0) {
        const int r = panel_try_0(self, code);
        if (r > 0) { ++g_panel_hits; return; }
        if (r < 0) { return; }            /* a named floor, already counted */
    }
    if (code != 0)
        ++g_panel_unknown;
    port_mg_call0(self, code, adj);
}

extern "C" void port_mg_panel_call1(void *self, unsigned code, int adj, int a)
{
    if (code != 0 && adj == 0) {
        const int r = panel_try_1(self, code, a);
        if (r > 0) { ++g_panel_hits; return; }
        if (r < 0) { return; }
    }
    if (code != 0)
        ++g_panel_unknown;
    port_mg_call1(self, code, adj, a);
}

extern "C" void port_mg_panel_counts(unsigned *hits, unsigned *floor,
                                     unsigned *unknown)
{
    if (hits)    *hits    = g_panel_hits;
    if (floor)   *floor   = g_panel_floor_2820;
    if (unknown) *unknown = g_panel_unknown;
}

// ---- the eleven host copies ------------------------------------------------
//
// Each is its src TU with the pair declaration replaced by MgPmf and the
// dispatch replaced by port_mg_panel_call0/1. Everything else is verbatim.

/* src/func_ov006_02107358.cpp -- VTABLE SLOT 6, the Behavior. Table 02142888,
   arity 0. THE THIRD SHAPE: the src reads the pair as two ints and open-codes
   the decode, so neither a link nor a `::*` sweep sees it.
   ROM 0x02107358: add r0,r4,#0x4000; ldr r0,[r0,#0xca8] (the state index, a
   WORD at +0x4ca8); pool 0x021073ac = 02142888; add r3,r1,r0,lsl#3 -- an
   EIGHT-byte stride; then the five-instruction Itanium sequence; then
   func_ov006_02104ac4 and func_ov006_02104354 off r4; mov r0,#1; return. */
/* PORT_HOST_ABI: mwcc pointer-to-member dispatch (dScMgPanel_c two-level state machine); the 8-byte {code,adj} pair is host-copied as an address switch, MSVC's 4-byte member pointer cannot express it */
extern "C" int func_ov006_02107358(char *c)
{
    const unsigned j = (unsigned)*(int *)(c + 0x4ca8);
    const MgPmf *e = &data_ov006_02142888[j];
    port_mg_panel_call0(c, e->code, e->adj);
    func_ov006_02104ac4(c);
    func_ov006_02104354(c);
    return 1;
}

/* src/func_ov006_02106ca4.c, table 02142840, arity 1, AND A STATE AS WELL --
   slot 6 of data_ov006_02142888, the ROUND-END state. Run mg7 lane L380.
   THE THIRD THIRD-SHAPE TU, and the first one this class gained by decompiling
   a floor rather than by inheriting a src file. ROM 0x02106ca4, 0x214 bytes:
   bl 0x021050bc; sl = self+0x4000; count = [sl,#0xcb8]; the per-panel index
   byte at self+i+0x4efa held in r5 ACROSS the dispatch and re-read at
   0x02106d04, which is what the `busy` counter below is; mov r1,r6 -- the
   loop counter is the argument.
   The tail is the round-over decision and it is transcribed one branch at a
   time: the u16 at +0x4ec0 is the hold-off timer; the byte at +0x4fe6 says
   whether this was the LAST round; 0x020a0de8[touch*4] and 0x020a0de9[touch*4]
   are the stylus record's held/edge bytes, and a touch cancels the hold-off.
   On the last round it plays the finish (0x02104870), clears the two flags and
   bumps the score at +0xb4 of the ov004 singleton under a 9999 clamp, keeping
   +0xb8 as its high-water mark; otherwise it re-arms for 0x10 frames, drops
   the outer state to 7 and re-seeds every panel through 0x021068d8. */
/* PORT_HOST_ABI: mwcc pointer-to-member dispatch (dScMgPanel_c two-level state machine); the 8-byte {code,adj} pair is host-copied as an address switch, MSVC's 4-byte member pointer cannot express it */
extern "C" void func_ov006_02106ca4(char *c)
{
    int busy = 0;
    int i;
    char *g;

    func_ov006_021050bc(c);
    for (i = 0; i < *(int *)(c + 0x4cb8); i++) {
        unsigned char *slot = (unsigned char *)(c + i + 0x4efa);
        const MgPmf *e = &data_ov006_02142840[*slot];
        port_mg_panel_call1(c, e->code, e->adj, i);
        if (*slot != 0)
            busy++;
    }
    if (busy != 0)
        return;
    if (*(unsigned short *)(c + 0x4ec0) == 0) {
        func_ov006_02104580(c);
        *(unsigned short *)(c + 0x4ec6) += 1;
        return;
    }
    *(unsigned short *)(c + 0x4ec0) -= 1;
    if (*(unsigned char *)(c + 0x4fe6) == 0) {
        if (data_020a0de8[data_020a0e40 * 4] != 0
            && data_020a0de9[data_020a0e40 * 4] != 0)
            *(unsigned short *)(c + 0x4ec0) = 0;
    }
    if (*(short *)(c + 0x4ec0) > 0)
        return;
    *(unsigned short *)(c + 0x4ec0) = 0;
    if (*(unsigned char *)(c + 0x4fe6) == 0) {
        *(unsigned short *)(c + 0x4ec0) = 0x10;
        *(int *)(c + 0x4ca8) = 7;
        func_ov006_021068d8(c);
        return;
    }
    func_ov006_02104870(c);
    func_ov004_020b0a54(0);
    func_ov006_02104ea8(c);
    *(unsigned char *)(c + 0x4fe3) = 0;
    *(unsigned char *)(c + 0xc3) = 0;
    g = data_ov004_020beb68;
    if (g != 0) {
        if (*(int *)(g + 0xb4) < 9999)
            *(int *)(g + 0xb4) += 1;
        if (*(int *)(g + 0xb4) > *(int *)(g + 0xb8))
            *(int *)(g + 0xb8) = *(int *)(g + 0xb4);
    }
    func_ov004_020adb1c(data_ov004_020beb68 != 0
                            ? *(int *)(data_ov004_020beb68 + 0xb4)
                            : 0);
}

/* src/func_ov006_02106bc0.c, table 02142840, arity 1. THE SECOND THIRD-SHAPE
   TU. ROM 0x02106bc0: r7 = self+0x4000; count = [r7,#0xcb8]; loop index r5;
   idx = ldrb [self+i+0x4000, #0xefa]; add r3,r4,r0,lsl#3; the Itanium
   sequence; mov r1,r5 -- the loop counter is the argument; then the +0x4ec0
   halfword countdown, func_ov004_020b0a54(0x12), the two flag bytes and
   func_ov006_02104ea8. */
/* PORT_HOST_ABI: mwcc pointer-to-member dispatch (dScMgPanel_c two-level state machine); the 8-byte {code,adj} pair is host-copied as an address switch, MSVC's 4-byte member pointer cannot express it */
extern "C" void func_ov006_02106bc0(char *c)
{
    int i;
    for (i = 0; i < *(int *)(c + 0x4cb8); i++) {
        const unsigned char idx = *(unsigned char *)(c + i + 0x4efa);
        const MgPmf *e = &data_ov006_02142840[idx];
        port_mg_panel_call1(c, e->code, e->adj, i);
    }
    if (*(unsigned short *)(c + 0x4ec0) == 0)
        return;
    *(unsigned short *)(c + 0x4ec0) -= 1;
    if (*(short *)(c + 0x4ec0) > 0)
        return;
    *(unsigned short *)(c + 0x4ec0) = 0;
    func_ov004_020b0a54(0x12);
    *(unsigned char *)(c + 0xc3) = 0;
    *(unsigned char *)(c + 0x4fe3) = 0;
    func_ov006_02104ea8(c);
}

/* src/func_ov006_02106eb8.cpp, table 02142840, arity 1.
   ROM 0x02106eb8: func_ov006_021050bc(self), func_ov006_021057f0(self), then
   the same count/index loop as 02106bc0 -- [r7,#0xcb8] and ldrb +0xefa -- with
   mov r1,r5. */
/* PORT_HOST_ABI: mwcc pointer-to-member dispatch (dScMgPanel_c two-level state machine); the 8-byte {code,adj} pair is host-copied as an address switch, MSVC's 4-byte member pointer cannot express it */
extern "C" void func_ov006_02106eb8(char *c)
{
    int i;
    func_ov006_021050bc(c);
    func_ov006_021057f0(c);
    for (i = 0; i < *(int *)(c + 0x4cb8); i++) {
        const unsigned char idx = *(unsigned char *)(c + i + 0x4efa);
        const MgPmf *e = &data_ov006_02142840[idx];
        port_mg_panel_call1(c, e->code, e->adj, i);
    }
}

/* src/func_ov006_02106f44.cpp, table 02142840, arity 1. Same loop, with the
   +0x4fe9 flag set to 1 and func_ov006_02105854 between the two calls. */
/* PORT_HOST_ABI: mwcc pointer-to-member dispatch (dScMgPanel_c two-level state machine); the 8-byte {code,adj} pair is host-copied as an address switch, MSVC's 4-byte member pointer cannot express it */
extern "C" void func_ov006_02106f44(char *c)
{
    int i;
    func_ov006_021050bc(c);
    *(unsigned char *)(c + 0x4fe9) = 1;
    func_ov006_02105854(c);
    for (i = 0; i < *(int *)(c + 0x4cb8); i++) {
        const unsigned char idx = *(unsigned char *)(c + i + 0x4efa);
        const MgPmf *e = &data_ov006_02142840[idx];
        port_mg_panel_call1(c, e->code, e->adj, i);
    }
}

/* src/func_ov006_02106fdc.c, table 02142840, arity 1. BANNERED NONMATCHING
   (different op / idiom, div=28) and therefore decompiled rather than matched,
   which is why the ordinary slice line is not enough and why stategen reports
   it as a delinks-join miss rather than a floor. The `found` counter is the
   part the banner is about: the ROM counts nonzero indices and, if none was
   seen, drops the outer state to 4 and sets the two flags. */
/* PORT_HOST_ABI: mwcc pointer-to-member dispatch (dScMgPanel_c two-level state machine); the 8-byte {code,adj} pair is host-copied as an address switch, MSVC's 4-byte member pointer cannot express it */
extern "C" void func_ov006_02106fdc(void *p)
{
    char *c = (char *)p;
    int found = 0;
    int i = 0;
    func_ov006_021050bc(c);
    *(unsigned char *)(c + 0x4fe9) = 1;
    if (*(int *)(c + 0x4cb8) > 0) {
        do {
            const unsigned char k = *(unsigned char *)(c + i + 0x4efa);
            const MgPmf *e = &data_ov006_02142840[k];
            port_mg_panel_call1(c, e->code, e->adj, i);
            i++;
            if (k != 0)
                found++;
        } while (i < *(int *)(c + 0x4cb8));
    }
    if (found != 0)
        return;
    *(int *)(c + 0x4ca8) = 4;
    *(unsigned char *)(c + 0x4fdf) = 1;
    *(short *)(c + 0x4ec4) = 0x40;
}

/* src/func_ov006_0210709c.cpp, table 02142840, arity 1. */
/* PORT_HOST_ABI: mwcc pointer-to-member dispatch (dScMgPanel_c two-level state machine); the 8-byte {code,adj} pair is host-copied as an address switch, MSVC's 4-byte member pointer cannot express it */
extern "C" void func_ov006_0210709c(void *p)
{
    char *c = (char *)p;
    int i;
    func_ov006_02105de4(c);
    func_ov006_021050bc(c);
    *(unsigned char *)(c + 0x4fe9) = 0;
    for (i = 0; i < *(int *)(c + 0x4cb8); i++) {
        const unsigned char idx = *(unsigned char *)(c + i + 0x4efa);
        const MgPmf *e = &data_ov006_02142840[idx];
        port_mg_panel_call1(c, e->code, e->adj, i);
    }
    func_ov006_02105d20(c);
    func_ov006_02105c88(c);
    func_ov006_02105134(c);
}

/* src/func_ov006_02104ac4.cpp, table 021427bc, arity 1 WITH A CONSTANT.
   ROM 0x02104ac4: add r1,r0,#0x4000; ldrb r2,[r1,#0x692] the guard; ldrb
   r1,[r1,#0x693] the index; pool 0x02104b20 = 021427BC; and `mov r1,#0`
   between the two arms of the Itanium sequence, so the argument is the
   LITERAL ZERO and not a loop counter. */
/* PORT_HOST_ABI: mwcc pointer-to-member dispatch (dScMgPanel_c two-level state machine); the 8-byte {code,adj} pair is host-copied as an address switch, MSVC's 4-byte member pointer cannot express it */
extern "C" void func_ov006_02104ac4(void *p)
{
    char *c = (char *)p;
    if (*(unsigned char *)(c + 0x4692) == 0)
        return;
    const unsigned j = *(unsigned char *)(c + 0x4693);
    const MgPmf *e = &data_ov006_021427bc[j];
    port_mg_panel_call1(c, e->code, e->adj, 0);
}

/* src/func_ov006_02104c60.cpp, table 021427ec, arity 0.
   ROM 0x02104c60: guard byte at +0x4684; the pooled literal 0x00004680 added
   to self is the halfword decremented; the s16 at +0x4680 tested <= 0 zeroes
   that halfword and the bytes at +0x4684 and +0x4685 and returns; otherwise
   the index is the byte at +0x4686 and the pool at 0x02104cf8 is 021427EC. */
/* PORT_HOST_ABI: mwcc pointer-to-member dispatch (dScMgPanel_c two-level state machine); the 8-byte {code,adj} pair is host-copied as an address switch, MSVC's 4-byte member pointer cannot express it */
extern "C" void func_ov006_02104c60(void *p)
{
    char *c = (char *)p;
    if (*(unsigned char *)(c + 0x4684) == 0)
        return;
    *(unsigned short *)(c + 0x4680) -= 1;
    if (*(short *)(c + 0x4680) <= 0) {
        *(short *)(c + 0x4680) = 0;
        *(unsigned char *)(c + 0x4684) = 0;
        *(unsigned char *)(c + 0x4685) = 0;
        return;
    }
    const unsigned j = *(unsigned char *)(c + 0x4686);
    const MgPmf *e = &data_ov006_021427ec[j];
    port_mg_panel_call0(c, e->code, e->adj);
}

/* src/func_ov006_021050bc.cpp, table 02142860, arity 0.
   ROM 0x021050bc: add r1,r0,#0x4000; ldrb r2,[r1,#0x674] guard; ldrb
   r1,[r1,#0x675] index; pool 0x02105114 = 02142860. */
/* PORT_HOST_ABI: mwcc pointer-to-member dispatch (dScMgPanel_c two-level state machine); the 8-byte {code,adj} pair is host-copied as an address switch, MSVC's 4-byte member pointer cannot express it */
extern "C" void func_ov006_021050bc(void *p)
{
    char *c = (char *)p;
    if (*(unsigned char *)(c + 0x4674) == 0)
        return;
    const unsigned j = *(unsigned char *)(c + 0x4675);
    const MgPmf *e = &data_ov006_02142860[j];
    port_mg_panel_call0(c, e->code, e->adj);
}

/* src/func_ov006_021057f0.cpp, table 02142820, arity 0.
   ROM 0x021057f0: ldr r1,[r0]; ldr r1,[r1,#0x8c]; blx r1 -- a VIRTUAL CALL ON
   SLOT 35 (0x8c/4), which is dScMgBase_c's func_ov004_020ad660 and reads
   `(this[2] & 0xff) != 0`. A nonzero answer returns without dispatching. The
   index is the byte at +0x4fe0 and the pool at 0x02105850 is 02142820; the
   tail is func_ov006_02104c60(self).
   THE VIRTUAL IS CALLED THROUGH THE SEATED TABLE, not re-spelled: by the time
   any state runs, port_scene_fill_panel has replaced slot 35's raw DS word
   with hal/scene_mg.cpp's mb_v35 thunk, so this dispatch reaches the ROM body
   through the same path every other slot does. */
/* PORT_HOST_ABI: mwcc pointer-to-member dispatch (dScMgPanel_c two-level state machine); the 8-byte {code,adj} pair is host-copied as an address switch, MSVC's 4-byte member pointer cannot express it */
extern "C" void func_ov006_021057f0(void *p)
{
    char *c = (char *)p;
    void **vt = *(void ***)c;
    int (__fastcall *m)(void *, void *) =
        (int (__fastcall *)(void *, void *))vt[35];
    if (m(c, 0) != 0)
        return;
    const unsigned j = *(unsigned char *)(c + 0x4fe0);
    const MgPmf *e = &data_ov006_02142820[j];
    port_mg_panel_call0(c, e->code, e->adj);
    func_ov006_02104c60(c);
}
