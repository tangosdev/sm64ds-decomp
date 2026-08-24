// PORT_HOST_ABI. The mwcc POINTER-TO-MEMBER WALL for dScMgJump_c, actor id
// 0x174, scene 372 -- the "Bounce and Pounce" minigame. Run mg11, lane BNP.
//
// Read port/slice_bnp.txt for the identity derivation, the five width checks,
// the fourteen ROM adjudications and the closure. Read
// unmatched/MgBase_StateDispatch.cpp's header first for the ROM's dispatch
// sequence and for port_mg_call0, which every chain in this family ends at.
// This file is the whole of this class's dispatch half.
//
// ---- 1. ONE DISPATCHER, AND IT IS VTABLE SLOT 6 ---------------------------
//
// This class has NO state table and NO overlay constructor, the
// dScMgHanachan_c architecture rather than the curling one. That is measured
// rather than assumed, and it is measured on BOTH classes that share this
// .data neighbourhood, because id 0x175 (dScMgJump2_c) was being seated
// concurrently and address adjacency is exactly how run mg9's lane LKY nearly
// host-copied a neighbour's dispatchers:
//
//   * NOT ONE relocation in config/arm9/overlays/ov006/relocs.txt whose source
//     lies in ov006's .init range (0x0212f4c4..0x0213356c) lands anywhere in
//     0x0213cb48..0x0213cc74, which is the whole of dScMgJump_c's .data;
//   * nor anywhere in 0x0213cc74..0x0213cd8c, which is the whole of
//     dScMgJump2_c's. Neither class has a constructor among the thirty-five,
//     so there is no shared sinit region for either lane to over-attribute and
//     the MgMemoryMatch/MgMemoryMaster trap cannot arise here at all.
//
// What the class has instead is SEVEN INDIVIDUALLY SYMBOLISED {code, adj}
// PAIRS in .data, every adjustment word zero, each with exactly ONE load
// relocation reaching it and that one from a literal pool inside this class's
// own code block. Read out of extracted/overlays/overlay_0006.bin at base
// 0x020bfec0, with the loading body named from the relocation's own source
// address:
//
//   pair        code       loaded by (the body that INSTALLS that state)
//   ----------  ---------  --------------------------------------------------
//   0213cb4c    020ee3ec   func_ov006_020ee44c   pool word 0x020ee4dc
//   0213cb54    020ee5b8   func_ov006_020ee658   pool word 0x020ee68c
//   0213cb5c    020ee44c   func_ov006_020ee4e0   pool word 0x020ee504
//   0213cb6c    020ee2c4   func_ov006_020ee3bc   pool word 0x020ee3e8
//   0213cb74    020ee2c0   func_ov006_020ee3ec   pool word 0x020ee448
//   0213cb7c    020ee2c0   func_ov006_020ee2c4   pool word 0x020ee3b8
//   0213cb84    020ee508   func_ov006_020ee598   pool word 0x020ee5b4
//
// SIX DISTINCT CODE WORDS across seven pairs (0x020ee2c0 is installed by two
// different bodies), and all six are routed below.
//
// THE SPAWNINFO SITS INSIDE THE PAIR RUN, which is lane WIG's sweep hazard in
// its purest form and the reason the run above is anchored on the RELOCATION
// SET rather than on an address range. 0x0213cb64 is not a pair: it is
// {0x020eeafc, 0x01740174} -- the factory word and the doubled actor id -- and
// it sits between the pair at 0x0213cb5c and the pair at 0x0213cb6c. A scan
// that started after it would have found four pairs and missed three.
//
// AND ONE WORD IN THE RUN IS NOT A PAIR EITHER. data_ov006_0213cb48, the word
// immediately before the first pair, holds 0xffffffff. It is a scratch int:
// the trapped slot-18 body loads its ADDRESS at 0x020eea5c and reads it as the
// last-rolled random value, rerolling while the new draw equals it. Counting
// it as the adjustment half of a pair would have produced an eighth state at
// code 0xffffffff that nothing installs.
//
// THE FIELD IS +0x5004 AND SLOT 6 IS THE ONLY READER. The ROM body at
// 0x020ee27c is the ARM Itanium member-pointer sequence over self+0x5004
// (pool word 0x020ee2bc = 0x00005004):
//
//     020ee284  ldr    r1, [pc, #0x30]      ; 0x00005004
//     020ee288  add    r3, r0, r1
//     020ee28c  ldr    r1, [r3, #4]         ; the adjustment/virtual word
//     020ee290  add    r0, r0, r1, asr #1
//     020ee294  ands   r1, r1, #1
//     020ee298  ldrne  r2, [r0]             ; virtual: load the vtable
//     020ee29c  ldrne  r1, [r3]
//     020ee2a0  ldrne  r1, [r2, r1]
//     020ee2a4  ldreq  r1, [r3]             ; direct: the code word
//     020ee2a8  blx    r1
//
// which is src/func_ov006_020ee27c.cpp's `(c->*c->m)()` to the instruction.
// The rulebook corollary applies: a member-pointer TYPE in the src puts the
// body on the host-copy side, so the seat wires the copy below rather than the
// src TU, and port/slice_bnp.txt does not list that TU.
//
// A `::*` / `->*` SWEEP OVER ALL 41 TUs OF THIS CLASS AND ITS SHARED BASE
// RETURNS EXACTLY ONE FILE, src/func_ov006_020ee27c.cpp. The open-coded third
// shape lane LKY's encoding detector exists for is absent here: decoding every
// word of 0x020edec0..0x020eebe8 (this class's code block) and
// 0x020e6c28..0x020e740c (dScMgD3DBase_c's) for `add Rd,Rn,Rm,asr #1`
// immediately followed by `ands Rm,Rm,#1` finds ONE site, 0x020ee290, and it
// is the slot-6 body above. The detector's own count is in port/slice_bnp.txt.
//
// ---- 2. THE STORED PAIR KEEPS THE ROM'S OWN WORDS -------------------------
//
// The state installers -- src/func_ov006_020ee658.c and the five like it --
// copy the pair out of mounted .data as two plain ints, so what lands in
// self+0x5004 is the DS code address the ROM wrote. This file decodes that
// address; it does not rewrite the mounted pairs. Writing host addresses over
// them is the ov085/ov100 shape and it is wrong here for the reason section 4
// of port/mg_fanout_costs.txt gives: the mounted words are also what a save
// state and a re-boot read, and an installer that copies a rewritten pair
// would make the class's identity depend on which fill ran first.
//
// ---- 3. WHAT IS NOT ROUTED, AND WHY THE COUNT CAN LEGITIMATELY BE ZERO ----
//
// dScMgJump_c's vtable slot 18 is func_ov006_020ee994, and it is this class's
// ONE nosrc floor: a config symbol with no delink block and no src file in
// either extension. hal/scene_mg_jump.cpp traps it, and the trap is the reason
// a default boot of scene 372 routes NOTHING through the switch below.
//
// THE FLOOR IS THE ONLY BODY THAT STARTS THE MACHINE. src/func_ov006_020ee690
// .cpp (slot 0, InitResources) ends with `((Base *)base)->m48(-1)` -- vtable
// offset 0x48, which is slot 18 -- and the ROM's slot-18 body tail-calls
// func_ov006_020ee658, whose only job is to install the pair at 0x0213cb54.
// Nothing else in ov006 writes self+0x5004 before slot 6 first reads it. So
// with the floor trapped the field stays zero, port_mg_call0's own null-pmf
// guard fires on every tick, and the honest reading of a zero here is "the
// state machine was never started", not "the class does not dispatch".
//
// hal/scene_mg_jump.cpp's SM64DS_BNP_START_STATE diagnostic is the only thing
// that changes that, it is OFF unless the variable is set, and what it does is
// call the ROM's OWN state installer -- func_ov006_020ee658, a decompiled,
// sliced, byte-matched TU -- and nothing else the floor would have done. That
// file states the boundary in full.

#include <cstdio>
#include <cstdlib>

struct MgPmf { unsigned code; int adj; };

extern "C" {

/* the family chain's end: the null-code guard, the nonzero-adjustment refusal
   and the UNHANDLED report all live in exactly one place */
void port_mg_call0(void *self, unsigned code, int adj);

/* ---- the six state bodies, all of them `void f(char *)` in the ROM ------ */
void func_ov006_020ee2c0(void);          /* 4 bytes in the ROM, `bx lr` */
void func_ov006_020ee2c4(char *c);
void func_ov006_020ee3ec(char *c);
void func_ov006_020ee44c(char *c);
void func_ov006_020ee508(char *c);
void func_ov006_020ee5b8(char *c);

/* the host copy of vtable slot 6, defined below */
int func_ov006_020ee27c(void *c);

/* this class's own witnesses, read by hal/scene_mg_jump.cpp */
void     port_mg_jump_counts(unsigned *calls, unsigned *hits, unsigned *nullpmf);
unsigned port_mg_jump_state_count(void);
unsigned port_mg_jump_state_addr(unsigned i);
unsigned port_mg_jump_state_hit(unsigned i);
unsigned port_mg_jump_state_last(void);

}  /* extern "C" */

// ---- the address switch ----------------------------------------------------

static unsigned g_bnp_calls;     /* every dispatch that reached this file */
static unsigned g_bnp_hits;      /* dispatches this file routed itself */
static unsigned g_bnp_nullpmf;   /* dispatches with code 0 -- see section 3 */
static unsigned g_bnp_last;      /* the last code word slot 6 saw */

/* A PER-ADDRESS CENSUS, for the reason unmatched/MgWiggler_StateDispatch.cpp
   gives: a routed total plus a zero UNHANDLED says every address the class
   asked for was one this file owns, and neither says WHICH of the six ran. On
   a class whose "state" is an address rather than an index that is the whole
   difference between "the machine moved" and an aggregate. Taken at the
   dispatch site, so the census survives the floor above it being retired. */
static const unsigned kBnpStates[] = {
    0x020ee2c0u,   /* the idle terminal, `bx lr`; installed by TWO pairs */
    0x020ee2c4u,   /* the fall/score state */
    0x020ee3ecu,
    0x020ee44cu,
    0x020ee508u,   /* the main play state */
    0x020ee5b8u,   /* the opening countdown -- the state slot 18 installs */
};
enum { kBnpStateCount = sizeof kBnpStates / sizeof kBnpStates[0] };
static unsigned g_bnp_state_hits[kBnpStateCount];

static void bnp_note(unsigned code)
{
    for (unsigned i = 0; i < (unsigned)kBnpStateCount; ++i)
        if (kBnpStates[i] == code) { ++g_bnp_state_hits[i]; return; }
}

static int bnp_try(void *self, unsigned code)
{
    char *c = (char *)self;
    bnp_note(code);
    switch (code) {
    case 0x020ee2c0u: func_ov006_020ee2c0();  return 1;
    case 0x020ee2c4u: func_ov006_020ee2c4(c); return 1;
    case 0x020ee3ecu: func_ov006_020ee3ec(c); return 1;
    case 0x020ee44cu: func_ov006_020ee44c(c); return 1;
    case 0x020ee508u: func_ov006_020ee508(c); return 1;
    case 0x020ee5b8u: func_ov006_020ee5b8(c); return 1;
    default:                                  return 0;
    }
}

extern "C" unsigned port_mg_jump_state_count(void)
{ return (unsigned)kBnpStateCount; }

extern "C" unsigned port_mg_jump_state_addr(unsigned i)
{ return i < (unsigned)kBnpStateCount ? kBnpStates[i] : 0u; }

extern "C" unsigned port_mg_jump_state_hit(unsigned i)
{ return i < (unsigned)kBnpStateCount ? g_bnp_state_hits[i] : 0u; }

extern "C" unsigned port_mg_jump_state_last(void)
{ return g_bnp_last; }

/* The one entry point for this class. Everything it does not own goes to the
   framework's chain, so the null guard, the nonzero-adjustment refusal and the
   UNHANDLED report stay in one place. */
extern "C" void port_mg_jump_call0(void *self, unsigned code, int adj)
{
    ++g_bnp_calls;
    g_bnp_last = code;
    if (code == 0)
        ++g_bnp_nullpmf;
    if (code != 0 && adj == 0 && bnp_try(self, code)) {
        ++g_bnp_hits;
        return;
    }
    port_mg_call0(self, code, adj);
}

extern "C" void port_mg_jump_counts(unsigned *calls, unsigned *hits,
                                    unsigned *nullpmf)
{
    if (calls)   *calls   = g_bnp_calls;
    if (hits)    *hits    = g_bnp_hits;
    if (nullpmf) *nullpmf = g_bnp_nullpmf;
}

// ---- the one host copy -----------------------------------------------------

/* src/func_ov006_020ee27c.cpp -- VTABLE SLOT 6, the Behavior. Its `struct C`
   carried nothing but the padding and the member-pointer typedef, so replacing
   the dispatch removes the struct entirely and no layout moves: every access
   in the copy is a raw char* offset at the same displacement the ROM uses. The
   ROM returns a literal 1 (mov r0,#1 at 0x020ee2ac, single exit), so the copy
   does too. */
extern "C" int func_ov006_020ee27c(void *c)
{
    const MgPmf *p = (const MgPmf *)((char *)c + 0x5004);
    port_mg_jump_call0(c, p->code, p->adj);
    return 1;
}
