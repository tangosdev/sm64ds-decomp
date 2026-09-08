// PORT_HOST_ABI. The mwcc POINTER-TO-MEMBER WALL, per-class half:
// dScMgBomroom_c's SEVEN dispatching TUs and its THIRTY-THREE state addresses.
// Run mg6, lane SOS. Actor id 0x172, scene 370, "Sort or 'Splode".
//
// Read unmatched/MgBase_StateDispatch.cpp's header first, then
// unmatched/MgCurling_StateDispatch.cpp and unmatched/MgCoin_StateDispatch.cpp.
// The first carries the ROM disassembly of the mwcc dispatch sequence and the
// finding that the overlay constructors already copy the pairs at the right
// stride; the second is the template; the third is the two-entry-point shape
// this file follows. None of it is repeated here.
//
// ---- THE THIRTY-THREE ADDRESSES, AND WHERE THEY COME FROM ------------------
//
// port/tools/stategen.py --sinit src/__sinit_ov006_0212fd48.c --overlay 6.
// That constructor is dScMgBomroom_c's own, and it was identified by the test
// port/mg_fanout_costs.txt section 3 prescribes rather than by name: the pairs
// it copies run 0x0213b9ec..0x0213baf4 and the class's SpawnInfo (0x0213badc)
// sits INSIDE that span. SEVEN tables, THIRTY-THREE slots, THIRTY-THREE
// DISTINCT addresses, no slot repeated, and every adjustment word zero --
// re-read out of extracted/overlays/overlay_0006.bin at base 0x020bfec0, pair
// by pair, rather than taken from the tool's summary line:
//
//   table                n  arity  dispatched by
//   -------------------  -  -----  ----------------------------------------
//   data_ov006_02141660  4    1    func_ov006_020d5fec   (constant 0 arg)
//   data_ov006_02141680  4    1    func_ov006_020d65c8
//   data_ov006_021416a0  4    0    func_ov006_020d8f98   THE OPEN-CODED ONE
//   data_ov006_021416c0  4    1    func_ov006_020d6278
//   data_ov006_021416e0  5    0    func_ov006_020d91b0   (vtable slot 6)
//   data_ov006_02141708  5    1    func_ov006_020d7c00
//   data_ov006_02141730  7    1    func_ov006_020d836c
//
// STATEGEN REFUSED THIS CONSTRUCTOR AND THE REFUSAL WAS CORRECT. It cannot
// derive an arity for data_ov006_021416a0, because arity comes from the
// dispatching TU's member-pointer typedef and 021416a0's dispatcher declares
// none -- it is the open-coded one below. The tool's own remedy, --arity N,
// applies ONE arity to every table, and six of the seven here are not that
// arity. So it was run with --arity 0 for the ADDRESS CENSUS ONLY (the slot
// order, the delink join and the refusal list are arity-independent) and the
// seven arities in the table above were read off the seven dispatchers' ROM
// disassembly one at a time. That is the "read the constructor by hand and
// document why" branch, and this paragraph is the documentation.
//
//   arity 1 is `mov r1, r5` (the loop counter) or `mov r1, #0` immediately
//   before the blx; arity 0 is a blx with r1 destroyed by the sequence itself
//   and never reloaded. Every one of the seven was checked at its own blx.
//
// ---- THE STATE THAT HAD NO BODY NOW HAS ONE (run mg7, lane L370) -----------
//
// 0x020d7c4c, slot 5 of data_ov006_02141730, is decompiled and BYTE-MATCHED --
// src/func_ov006_020d7c4c.c, 0x230, matching mwccarm 1.2/base, 1.2/sp2 and
// 1.2/sp2p3 with strict relocs. The reporting case that stood here (it counted
// how often the ROM's own state machine ASKED for a body this build did not
// have) is retired, and the slot calls the real body.
//
// IT IS THE BOMB'S FREE-FLIGHT STATE: advance the position along the heading
// through data_02082214, then bounce off the walls of whichever of the two
// boxes the +0x696 flag selects (right box x in [0xc0,0x100], left box x in
// [0,0x40], both y in [0x40,0x80], radius 12). That is why it was never wanted
// on an mg6 run: the machine parked at index 2 and no bomb ever reached flight.
//
// THE COUNTER IS KEPT AND ITS MEANING IS INVERTED. It used to say "the game
// wanted a body that is missing"; it now says "the newly landed body RAN".
// A zero on a scene-370 run is no longer a decomp gap, it is the statement
// that no bomb was in flight, and hal/scene_mg_bomroom.cpp's census prints it
// either way so a zero still says which zero it is.
//
// ---- WHY SEVEN HOST COPIES, AND WHY ONLY SIX OF THEM ARE FINDABLE ----------
//
// A LINK IS NOT A COMPLETE DETECTOR FOR THIS WALL AND NEITHER IS A SOURCE
// SWEEP. port/mg_fanout_costs.txt section 4 states both halves of that; this
// class is the first seat that carries all three shapes at once, so it is
// worth having the split in one place:
//
//   NAMED BY A LINK -- the table is declared at C++ linkage, so MSVC encodes
//   the member-pointer type into the symbol and nothing can satisfy it
//     func_ov006_020d91b0   PMF data_ov006_021416e0[]
//     func_ov006_020d836c   PMF data_ov006_02141730[]
//     func_ov006_020d65c8   PMF data_ov006_02141680[]
//     func_ov006_020d6278   PMF data_ov006_021416c0[]
//     func_ov006_020d7c00   Entry{PMF} data_ov006_02141708[]  (the @@3PAU
//                           spelling facegen's WALL test misses)
//     func_ov006_020d5fec   Entry{PMF} data_ov006_02141660[]  (likewise)
//
//   FOUND ONLY BY READING THE SOURCE, and convicted only by a run
//     func_ov006_020d8f98   `extern int data_ov006_021416a0[];` inside
//                           extern "C", and the ARM Itanium sequence
//                           open-coded in plain ints
//
// The seventh is lane FLW's third shape (src/func_ov006_020c3d18.cpp is the
// other known instance). It declares no member-pointer type, so no symbol is
// unresolvable; it spells no `::*`, so no sweep matches it. Its src is
// FAITHFUL -- it is the ROM's five instructions transcribed one for one -- and
// that is exactly what makes it dangerous: it compiles, it links, and the two
// words it decodes are the MOUNTED ROM's, so the blx lands on a raw DS address
// and eip is the tell. The ROM at 0x020d8f98:
//
//     ldr   r1,[r3,#4]            the adjustment word
//     add   r0,r4,r1,asr#1        this, advanced, arithmetic shift
//     ands  r1,r1,#1              the virtual bit, in the LSB
//     ldrne r2,[r0] / ldrne r1,[r3] / ldrne r1,[r2,r1]
//     ldreq r1,[r3]
//     blx   r1
//
// ROUTED AT THE DISPATCH SITE, NOT BY REWRITING THE STORED PAIR, which is the
// ruling lane FLW established: the pairs keep the ROM's own words, because
// this family's idle test compares a stored pair BY VALUE against a sentinel
// and a host address never equals a DS one. The host copy replaces the DECODE.
//
// NONE OF THE SEVEN PUTS A MEMBER POINTER IN AN OBJECT FIELD. That is the
// worse ov004 defect class (four bytes where the ROM has eight moves every
// field after it, so the body reads the wrong field before any dispatch
// happens). All seven dispatch through a table, so their struct layouts are
// correct and only the stride and the call sequence are wrong.
//
// ---- FOUR RIDE-THROUGHS, AND ALL FOUR ARE EMPTY ----------------------------
//
// func_ov006_020d7958, _020d7778, _020d64c4 and _020d5f28 sit in ONE-argument
// slots and take (void). Every one is FOUR BYTES in the ROM -- the whole
// function is `bx lr` -- so there is nothing for an ignored argument to be
// wrong about, and calling them with no arguments is the faithful host form.
// This is deliberately not the defect class of hal/scene_actor_faces.cpp's
// veneers, where a (void) callee stood between a caller and a body that DID
// want arguments; these four have no body at all.
//
// ---- WHY THIS FILE HAS ITS OWN ENTRY POINTS --------------------------------
//
// unmatched/MgBase_StateDispatch.cpp owns port_mg_call0 and port_mg_call1 and
// chains them to one per-class pair that curling defines. A second class
// cannot define those names, and widening the chain means editing MG2's file.
// So the host copies below call port_mg_bomroom_call0 / _call1, which try THIS
// class's switch and hand everything else to the framework unchanged. The
// framework therefore remains the single place that decides what a null code
// word means, what a nonzero adjustment means, and how an unhandled address is
// reported. This is the shape lane CCN established for the same reason.

#include <cstdio>
#include <cstdlib>   /* std::abort, for the boot installer below (lane PMFB4) */

/* The eight-byte mwcc member pointer, in the only spelling that is true on
   both machines: two words, no member-pointer type anywhere. */
struct MgPmf { unsigned code; int adj; };

extern "C" {

/* the framework's entry points; see MgBase_StateDispatch.cpp */
void port_mg_call0(void *self, unsigned code, int adj);
void port_mg_call1(void *self, unsigned code, int adj, int a);

/* ---- the thirty-two matched state bodies, in address order ----------------
   Each is declared with the parameter list ITS OWN src TU defines, so a
   ride-through is called the way the ROM calls it rather than the way the
   slot's arity would suggest. The four (void) ones are the empty bodies the
   header names. Two parameter names differ from src's because src spells them
   `this`, which is a keyword in the language this file compiles as; a
   declaration's parameter names are not part of its type. */
void func_ov006_020d5eb8(unsigned char *base, int idx);
void func_ov006_020d5f28(void);              /* one-argument slot, bx lr body */
void func_ov006_020d5f2c(char *c, int i);
void func_ov006_020d5fd8(int c, int i);
void func_ov006_020d6100(unsigned char *base, int idx);
void func_ov006_020d6170(char *c, int idx);
void func_ov006_020d61dc(char *c, int index);
void func_ov006_020d6264(int c, int i);
void func_ov006_020d6454(unsigned char *base, int idx);
void func_ov006_020d64c4(void);              /* one-argument slot, bx lr body */
void func_ov006_020d64c8(char *o, int i);
void func_ov006_020d65b4(int c, int i);
void func_ov006_020d7778(void);              /* one-argument slot, bx lr body */
void func_ov006_020d777c(char *self, int idx);
void func_ov006_020d7c4c(char *c, int i);    /* run mg7: was the missing state */
void func_ov006_020d7958(void);              /* one-argument slot, bx lr body */
void func_ov006_020d795c(char *o, int i);
void func_ov006_020d7a84(char *c, int i);
void func_ov006_020d7e7c(char *c, int i);
void func_ov006_020d7edc(unsigned char *c, int idx);
void func_ov006_020d7f5c(char *self, int idx);
void func_ov006_020d816c(char *self, int idx);
void func_ov006_020d8324(int c, int i);
void func_ov006_020d89c4(char *self);
void func_ov006_020d8af8(char *self);
void func_ov006_020d8cc4(char *r5);
void func_ov006_020d8d84(char *self);
void func_ov006_020d8f34(char *c);
int  func_ov006_020d8ff4(void *c);
void func_ov006_020d9020(void *c);
void func_ov006_020d904c(void *c);

/* the seven mount tables, re-typed to the ROM's eight-byte pair. The mount
   defines the storage; __sinit_ov006_0212fd48 fills it at scene load. */
extern MgPmf data_ov006_02141660[];
extern MgPmf data_ov006_02141680[];
extern MgPmf data_ov006_021416a0[];
extern MgPmf data_ov006_021416c0[];
extern MgPmf data_ov006_021416e0[];
extern MgPmf data_ov006_02141708[];
extern MgPmf data_ov006_02141730[];

/* the ordinary callees the host copies below keep, each spelled as its own src
   TU spells it */
void func_ov006_020d5b10(char *c);
void func_ov006_020d5d08(char *c);
void func_ov006_020d69b8(char *a, int i);

/* the two arm9 globals src/func_ov006_020d836c.cpp reads, in its spelling.
   data_020a0e40 is read as a BYTE by the ROM (`ldrb r1,[r1]`) and indexes
   data_020a0de8 by word (`ldrb r0,[r2,r1,lsl #2]`); the host mount defines
   both under these plain C names. */
extern unsigned char data_020a0e40;
extern int data_020a0de8[];

/* THE TWO DISPATCHERS THAT ARE ALSO STATE BODIES, and are still host-side.
   020d7c00 is host-copied further down this file; 020d8f98 compiles from src
   with hostgen's MG_PMF_CALL swap. Both are called from above their own
   definitions by the address switch, and both are now also reached from a face
   the boot installer writes into a table cell.
   Run link100 lane PMFB4: the other five declarations that stood here --
   020d5fec, 020d6278, 020d65c8, 020d836c and 020d91b0 -- are gone with the host
   copies they named. Those five symbols are their own matched TUs now and
   nothing in this file calls them. */
void func_ov006_020d7c00(char *c, int i);
void func_ov006_020d8f98(unsigned char *c);

/* the boot installer at the end of this file; hal/scene_mg.cpp calls it after
   the ov006 constructors have filled the tables. */
void port_mg_bomroom_states_seat(void);

}  /* extern "C" */

// ---- the class's address switch --------------------------------------------

static unsigned g_bomroom_state_hits;
/* THE FREE-FLIGHT-STATE COUNTER. 0x020d7c4c used to have no decompiled body
   and this counted how often the ROM's own state machine ASKED for one. Run
   mg7 landed the body byte-matched, so it now counts how often that state
   RAN. */
static unsigned g_bomroom_flight_calls;
/* HOW OFTEN THE OPEN-CODED DISPATCHER RAN, counted separately because it is
   the one shape neither the link nor a source sweep can find. On a build that
   still compiled src/func_ov006_020d8f98.cpp this count would be zero AND the
   run would fault on a DS address; a nonzero count here with a clean run is
   the positive evidence that the seventh host copy is doing its job. */
static unsigned g_bomroom_opencoded_calls;

static int bomroom_try_0(void *self, unsigned code)
{
    char *c = (char *)self;
    switch (code) {
    /* data_ov006_021416e0, the table vtable slot 6 dispatches */
    case 0x020d904cu: func_ov006_020d904c(c);            return 1;
    case 0x020d9020u: func_ov006_020d9020(c);            return 1;
    case 0x020d8ff4u: func_ov006_020d8ff4(c);            return 1;
    case 0x020d8f98u: func_ov006_020d8f98((unsigned char *)c);
                                                         return 1; /* host copy */
    case 0x020d8f34u: func_ov006_020d8f34(c);            return 1;
    /* data_ov006_021416a0, the open-coded dispatcher's own table */
    case 0x020d8d84u: func_ov006_020d8d84(c);            return 1;
    case 0x020d8cc4u: func_ov006_020d8cc4(c);            return 1;
    case 0x020d8af8u: func_ov006_020d8af8(c);            return 1;
    case 0x020d89c4u: func_ov006_020d89c4(c);            return 1;
    default:                                             return 0;
    }
}

static int bomroom_try_1(void *self, unsigned code, int a)
{
    char *c = (char *)self;
    switch (code) {
    /* data_ov006_02141730, seven slots, dispatched by func_ov006_020d836c */
    case 0x020d8324u: func_ov006_020d8324((int)(size_t)c, a); return 1;
    case 0x020d816cu: func_ov006_020d816c(c, a);              return 1;
    case 0x020d7f5cu: func_ov006_020d7f5c(c, a);              return 1;
    case 0x020d7edcu: func_ov006_020d7edc((unsigned char *)c, a); return 1;
    case 0x020d7e7cu: func_ov006_020d7e7c(c, a);              return 1;
    /* Slot 5 of this table. Was the floor; run mg7 decompiled and byte-matched
       it, so this dispatches the real body and the count says it ran. */
    case 0x020d7c4cu: ++g_bomroom_flight_calls;
                      func_ov006_020d7c4c(c, a);              return 1;
    case 0x020d7c00u: func_ov006_020d7c00(c, a);              return 1; /* host */
    /* data_ov006_02141708, five slots, dispatched by func_ov006_020d7c00 */
    case 0x020d7a84u: func_ov006_020d7a84(c, a);              return 1;
    case 0x020d795cu: func_ov006_020d795c(c, a);              return 1;
    case 0x020d7958u: func_ov006_020d7958();                  return 1; /* void */
    case 0x020d777cu: func_ov006_020d777c(c, a);              return 1;
    case 0x020d7778u: func_ov006_020d7778();                  return 1; /* void */
    /* data_ov006_02141680, four slots, dispatched by func_ov006_020d65c8 */
    case 0x020d65b4u: func_ov006_020d65b4((int)(size_t)c, a); return 1;
    case 0x020d64c8u: func_ov006_020d64c8(c, a);              return 1;
    case 0x020d64c4u: func_ov006_020d64c4();                  return 1; /* void */
    case 0x020d6454u: func_ov006_020d6454((unsigned char *)c, a); return 1;
    /* data_ov006_021416c0, four slots, dispatched by func_ov006_020d6278 */
    case 0x020d6264u: func_ov006_020d6264((int)(size_t)c, a); return 1;
    case 0x020d61dcu: func_ov006_020d61dc(c, a);              return 1;
    case 0x020d6170u: func_ov006_020d6170(c, a);              return 1;
    case 0x020d6100u: func_ov006_020d6100((unsigned char *)c, a); return 1;
    /* data_ov006_02141660, four slots, dispatched by func_ov006_020d5fec */
    case 0x020d5fd8u: func_ov006_020d5fd8((int)(size_t)c, a); return 1;
    case 0x020d5f2cu: func_ov006_020d5f2c(c, a);              return 1;
    case 0x020d5f28u: func_ov006_020d5f28();                  return 1; /* void */
    case 0x020d5eb8u: func_ov006_020d5eb8((unsigned char *)c, a); return 1;
    default:                                                  return 0;
    }
}

/* The two entry points the host copies below use. Everything this switch does
   not own goes to the framework unchanged, so the null-code guard, the
   nonzero-adjustment refusal and the UNHANDLED report all still live in
   exactly one place. */
extern "C" void port_mg_bomroom_call0(void *self, unsigned code, int adj)
{
    if (code != 0 && adj == 0 && bomroom_try_0(self, code)) {
        ++g_bomroom_state_hits;
        return;
    }
    port_mg_call0(self, code, adj);
}

extern "C" void port_mg_bomroom_call1(void *self, unsigned code, int adj, int a)
{
    if (code != 0 && adj == 0 && bomroom_try_1(self, code, a)) {
        ++g_bomroom_state_hits;
        return;
    }
    port_mg_call1(self, code, adj, a);
}

extern "C" unsigned port_mg_bomroom_state_hits(void)
{
    return g_bomroom_state_hits;
}

extern "C" unsigned port_mg_bomroom_flight_calls(void)
{
    return g_bomroom_flight_calls;
}

/* Lane LINKMG: the entry func_ov006_020d8f98 reaches now that it compiles
   FROM src. Its src TU decodes the pair as two plain ints, which MSVC
   compiles as it stands; only the call through the resulting DS code address
   is impossible, and tools/hostgen.py's MG_PMF_CALL table swaps that one line
   for a call here. This wrapper exists so the census line in
   hal/scene_mg_bomroom.cpp keeps counting the open-coded dispatcher
   separately, which is the only evidence a run has that the shape is served. */
extern "C" void port_mg_bomroom_opencoded_call0(void *self, unsigned code, int adj)
{
    ++g_bomroom_opencoded_calls;
    port_mg_bomroom_call0(self, code, adj);
}

extern "C" unsigned port_mg_bomroom_opencoded_calls(void)
{
    return g_bomroom_opencoded_calls;
}

// ---- ONE HOST COPY IS LEFT, AND FIVE ARE RETIRED ---------------------------
//
// Run link100 lane PMFB4 seated FIVE of this class's seven tables, so the five
// dispatchers that read them now compile from their own matched TUs
// (port/slice_pmfb4.txt) and the host copies of them are gone:
//
//   func_ov006_020d5fec  data_ov006_02141660   src/func_ov006_020d5fec.cpp
//   func_ov006_020d65c8  data_ov006_02141680   src/func_ov006_020d65c8.cpp
//   func_ov006_020d6278  data_ov006_021416c0   src/func_ov006_020d6278.cpp
//   func_ov006_020d91b0  data_ov006_021416e0   src/func_ov006_020d91b0.cpp
//   func_ov006_020d836c  data_ov006_02141730   src/func_ov006_020d836c.cpp
//
// THE TWO THAT STAY, and why the switch above stays with them:
//
//   func_ov006_020d7c00  reads data_ov006_02141708, which is NOT seated. Its
//     copy is below, unchanged, and it still hands a DS code word to
//     port_mg_bomroom_call1. It is also state slot 6 of data_ov006_02141730,
//     so the seat installs a face that calls THIS copy.
//   func_ov006_020d8f98  reads data_ov006_021416a0, also not seated. It
//     compiles from src with hostgen's MG_PMF_CALL swap and is state slot 3 of
//     data_ov006_021416e0, so the seat installs a face that calls it too.
//
// So bomroom_try_0 keeps 021416a0's four cases live and bomroom_try_1 keeps
// 02141708's five; the cases belonging to the five seated tables are now
// unreachable and are left in place rather than deleted, because deleting them
// would remove the only written record of which address belongs to which table
// and the storage they name is still real.

/* src/func_ov006_020d7c00.cpp -- one element, no loop: the caller hands it the
   index. Stride 0x40 (the ROM's `add r2,r0,r1,lsl #6`), state byte at
   +0x469b. `this` is the class base, the argument is the same index it was
   given. It is ALSO state slot 6 of data_ov006_02141730, which is why the
   switch above routes to this copy. */
/* PORT_HOST_ABI: mwcc pointer-to-member dispatch (dScMgBomroom_c state table); the 8-byte {code,adj} pair is host-copied as an address switch, MSVC's 4-byte member pointer cannot express it */
extern "C" void func_ov006_020d7c00(char *c, int i)
{
    unsigned char *e = (unsigned char *)c + i * 0x40 + 0x4000;
    const MgPmf *p = &data_ov006_02141708[e[0x69b]];
    port_mg_bomroom_call1(c, p->code, p->adj, i);
}

// ---- THE TWENTY-FOUR FACES AND THE BOOT INSTALLER --------------------------
//
// Run link100 lane PMFB4. Five of dScMgBomroom_c's seven state tables now hold
// HOST addresses, written at boot by port_mg_bomroom_states_seat below after
// every cell has been compared against the ROM's own code word and a zero
// adjustment word. The five matched dispatchers then dispatch through the table
// directly and no longer route through this file's address switch.
//
// THE STRIDE, BOTH SIDES, PER ROW -- measured on this tree, not carried
// (runs/link100/out/PMFB4/rom_gate1.txt and emit_gate1.txt):
//
//   dispatcher            table                ROM                     emitted
//   --------------------  -------------------  ----------------------  --------
//   func_ov006_020d5fec   data_ov006_02141660  add r3,r4,r0,lsl #3     [eax*8]
//                           pool 020d6048 = 02141660                   [eax*8+4]
//   func_ov006_020d65c8   data_ov006_02141680  add r3,r4,r0,lsl #3     [eax*8]
//                           pool 020d662c = 02141680                   [eax*8+4]
//   func_ov006_020d6278   data_ov006_021416c0  add r3,r4,r0,lsl #3     [eax*8]
//                           pool 020d62dc = 021416c0                   [eax*8+4]
//   func_ov006_020d91b0   data_ov006_021416e0  add r3,r1,r0,lsl #3     [eax*8]
//                           pool 020d9240 = 021416e0                   [eax*8+4]
//   func_ov006_020d836c   data_ov006_02141730  add r3,r4,r0,lsl #3     [eax*8]
//                           pool 020d8404 = 02141730                   [eax*8+4]
//
// so ROM 8 == emitted 8 on all five. The `add r0,r6,r5,lsl #4` in 020d65c8 and
// 020d6278 and the `add r0,r6,r5,lsl #6` in 020d836c are the ENTITY cursor off
// `this`, not the table: the record stride is the add whose base register is
// the one the literal pool loaded with the table address, and that is the one
// quoted above.
//
// NO /Zp4 ON ANY OF THE FIVE, measured rather than assumed. Lane PMFB1's lever
// is that MSVC aligns any struct CONTAINING a pointer-to-member to eight, which
// silently rounds a twenty-byte record to twenty-four. Each of these five TUs
// was compiled under the port's own flags with and without the option for this
// lane and the two /FAsc listings came back IDENTICAL except for the TITLE line
// naming the .obj -- zero real diff lines -- so the option is not claimed.
//
// THE TWENTY-FOUR SOURCE PAIRS ALL READ {code, 0} in the overlay image at the
// addresses src/__sinit_ov006_0212fd48.c copies each slot from, and every one is
// a WHOLE-PAIR copy (`data_ov006_TAB.pN = data_ov006_SRC;`, a struct assignment
// of the whole eight-byte Pair). There is not one field-form fill, which is the
// condition that lets this installer prove what it writes:
//
//   02141660[0] <- 0213bab4 020d5fd8/0    02141730[0] <- 0213ba14 020d8324/0
//   02141660[1] <- 0213bacc 020d5f2c/0    02141730[1] <- 0213baac 020d816c/0
//   02141660[2] <- 0213b9ec 020d5f28/0    02141730[2] <- 0213ba1c 020d7f5c/0
//   02141660[3] <- 0213b9fc 020d5eb8/0    02141730[3] <- 0213baa4 020d7edc/0
//   02141680[0] <- 0213ba44 020d65b4/0    02141730[4] <- 0213ba9c 020d7e7c/0
//   02141680[1] <- 0213b9f4 020d64c8/0    02141730[5] <- 0213ba94 020d7c4c/0
//   02141680[2] <- 0213ba34 020d64c4/0    02141730[6] <- 0213ba5c 020d7c00/0
//   02141680[3] <- 0213bae4 020d6454/0    021416e0[0] <- 0213ba0c 020d904c/0
//   021416c0[0] <- 0213ba4c 020d6264/0    021416e0[1] <- 0213ba6c 020d9020/0
//   021416c0[1] <- 0213ba54 020d61dc/0    021416e0[2] <- 0213ba3c 020d8ff4/0
//   021416c0[2] <- 0213ba64 020d6170/0    021416e0[3] <- 0213baf4 020d8f98/0
//   021416c0[3] <- 0213ba7c 020d6100/0    021416e0[4] <- 0213baec 020d8f34/0
//
// A FACE GOES IN EVERY CELL, and the arity comes from each row's OWN listing
// rather than from the census column (PMFC2's note column says "0 stack
// argument(s)" for rows this lane measured pushing one, which is the same class
// of error lane PMFB1 recorded against its width_ev column). All five matched
// TUs emit
//
//     push  <arg>                              only on the arity-1 tables
//     mov   ecx, tab[eax*8+4]                  the adjustment word
//     mov   eax, tab[eax*8]                    the code word
//     add   ecx, <this>
//     call  eax                                and NO add esp,N after it
//
// -- a __thiscall indirect: receiver in ecx, callee cleanup. A plain cdecl body
// reads its receiver off the stack and cleans nothing, so the code word cannot
// hold one. __fastcall takes the receiver in ecx, ignores edx (which still holds
// the same object, the adjustment word being zero) and cleans the same bytes.
//
//   arity 1 (one pushed argument): 02141660, 02141680, 021416c0, 02141730
//   arity 0 (nothing pushed):      021416e0
//
// ONE /alternatename. src/func_ov006_020d836c.cpp declares its table at
// namespace scope, so MSVC spells the reference
// ?data_ov006_02141730@@3PAP8C@@AEXH@ZA -- read off the object with dumpbin
// /symbols, not guessed -- while the ov006 mount defines the plain C name. The
// other four TUs declare their table inside their own extern "C" block and come
// in as _data_ov006_XXXXXXXX.
//
// SAFE UNDER port/tools/alternatename_guard.py for hal/pmfc_aliases.cpp's
// reason: the LHS is a C++ decoration only this matched TU ever spells, nothing
// in the tree defines it, and the only definition of the address is the mount's
// C-linkage symbol on the right.
#pragma comment(linker, "/alternatename:?data_ov006_02141730@@3PAP8C@@AEXH@ZA=_data_ov006_02141730")

/* EVERY FACE COUNTS, and two of them carry a witness this file already prints.
   A face is reachable only from a table word this file's own seat wrote, so
   g_bomroom_state_hits counts exactly the dispatches that happened -- the same
   number port_mg_bomroom_callN used to count when the host copies routed
   through the switch. g_bomroom_flight_calls has to move with slot 5 of
   02141730 for the same reason: it is hal/scene_mg_bomroom.cpp's witness that
   the free-flight state RAN, and leaving it in the switch would have made it
   read zero on every run with nothing being wrong. */
#define BR_FACE1(sym, cast)                                                   \
    static void __fastcall br_##sym(void *self, void *dead_edx, int i)        \
    {                                                                         \
        (void)dead_edx;                                                       \
        ++g_bomroom_state_hits;                                               \
        sym(cast self, i);                                                    \
    }

/* the two one-argument slots whose ROM body is four bytes of `bx lr` and whose
   src TU takes (void); declared and called with no argument, which is the shape
   port/tools/aritycheck.py checks. */
#define BR_FACE1_VOID(sym)                                                    \
    static void __fastcall br_##sym(void *self, void *dead_edx, int i)        \
    {                                                                         \
        (void)self; (void)dead_edx; (void)i;                                  \
        ++g_bomroom_state_hits;                                               \
        sym();                                                                \
    }

/* data_ov006_021416e0 pushes nothing: receiver in ecx, callee cleans zero. */
#define BR_FACE0(sym, cast)                                                   \
    static void __fastcall br_##sym(void *self, void *dead_edx)               \
    {                                                                         \
        (void)dead_edx;                                                       \
        ++g_bomroom_state_hits;                                               \
        sym(cast self);                                                       \
    }

/* data_ov006_02141660, arity 1 */
BR_FACE1(func_ov006_020d5fd8, (int)(size_t))
BR_FACE1(func_ov006_020d5f2c, (char *))
BR_FACE1_VOID(func_ov006_020d5f28)
BR_FACE1(func_ov006_020d5eb8, (unsigned char *))
/* data_ov006_02141680, arity 1 */
BR_FACE1(func_ov006_020d65b4, (int)(size_t))
BR_FACE1(func_ov006_020d64c8, (char *))
BR_FACE1_VOID(func_ov006_020d64c4)
BR_FACE1(func_ov006_020d6454, (unsigned char *))
/* data_ov006_021416c0, arity 1 */
BR_FACE1(func_ov006_020d6264, (int)(size_t))
BR_FACE1(func_ov006_020d61dc, (char *))
BR_FACE1(func_ov006_020d6170, (char *))
BR_FACE1(func_ov006_020d6100, (unsigned char *))
/* data_ov006_021416e0, arity 0 */
BR_FACE0(func_ov006_020d904c, (char *))
BR_FACE0(func_ov006_020d9020, (char *))
BR_FACE0(func_ov006_020d8ff4, (char *))
BR_FACE0(func_ov006_020d8f98, (unsigned char *))
BR_FACE0(func_ov006_020d8f34, (char *))
/* data_ov006_02141730, arity 1 */
BR_FACE1(func_ov006_020d8324, (int)(size_t))
BR_FACE1(func_ov006_020d816c, (char *))
BR_FACE1(func_ov006_020d7f5c, (char *))
BR_FACE1(func_ov006_020d7edc, (unsigned char *))
BR_FACE1(func_ov006_020d7e7c, (char *))
BR_FACE1(func_ov006_020d7c00, (char *))

/* Slot 5 of data_ov006_02141730, the bomb's free-flight state. Written out
   rather than macro-generated because it carries the census counter
   hal/scene_mg_bomroom.cpp prints. */
static void __fastcall br_func_ov006_020d7c4c(void *self, void *dead_edx, int i)
{
    (void)dead_edx;
    ++g_bomroom_state_hits;
    ++g_bomroom_flight_calls;
    func_ov006_020d7c4c((char *)self, i);
}

extern "C" void port_mg_bomroom_states_seat(void)
{
    static int done;
    if (done)
        return;
    done = 1;

    static const struct {
        MgPmf *table;
        const char *name;
        unsigned slot;
        unsigned rom;
        void *face;
    } seats[] = {
        {data_ov006_02141660, "02141660", 0, 0x020d5fd8u, (void *)br_func_ov006_020d5fd8},
        {data_ov006_02141660, "02141660", 1, 0x020d5f2cu, (void *)br_func_ov006_020d5f2c},
        {data_ov006_02141660, "02141660", 2, 0x020d5f28u, (void *)br_func_ov006_020d5f28},
        {data_ov006_02141660, "02141660", 3, 0x020d5eb8u, (void *)br_func_ov006_020d5eb8},

        {data_ov006_02141680, "02141680", 0, 0x020d65b4u, (void *)br_func_ov006_020d65b4},
        {data_ov006_02141680, "02141680", 1, 0x020d64c8u, (void *)br_func_ov006_020d64c8},
        {data_ov006_02141680, "02141680", 2, 0x020d64c4u, (void *)br_func_ov006_020d64c4},
        {data_ov006_02141680, "02141680", 3, 0x020d6454u, (void *)br_func_ov006_020d6454},

        {data_ov006_021416c0, "021416c0", 0, 0x020d6264u, (void *)br_func_ov006_020d6264},
        {data_ov006_021416c0, "021416c0", 1, 0x020d61dcu, (void *)br_func_ov006_020d61dc},
        {data_ov006_021416c0, "021416c0", 2, 0x020d6170u, (void *)br_func_ov006_020d6170},
        {data_ov006_021416c0, "021416c0", 3, 0x020d6100u, (void *)br_func_ov006_020d6100},

        {data_ov006_021416e0, "021416e0", 0, 0x020d904cu, (void *)br_func_ov006_020d904c},
        {data_ov006_021416e0, "021416e0", 1, 0x020d9020u, (void *)br_func_ov006_020d9020},
        {data_ov006_021416e0, "021416e0", 2, 0x020d8ff4u, (void *)br_func_ov006_020d8ff4},
        {data_ov006_021416e0, "021416e0", 3, 0x020d8f98u, (void *)br_func_ov006_020d8f98},
        {data_ov006_021416e0, "021416e0", 4, 0x020d8f34u, (void *)br_func_ov006_020d8f34},

        {data_ov006_02141730, "02141730", 0, 0x020d8324u, (void *)br_func_ov006_020d8324},
        {data_ov006_02141730, "02141730", 1, 0x020d816cu, (void *)br_func_ov006_020d816c},
        {data_ov006_02141730, "02141730", 2, 0x020d7f5cu, (void *)br_func_ov006_020d7f5c},
        {data_ov006_02141730, "02141730", 3, 0x020d7edcu, (void *)br_func_ov006_020d7edc},
        {data_ov006_02141730, "02141730", 4, 0x020d7e7cu, (void *)br_func_ov006_020d7e7c},
        {data_ov006_02141730, "02141730", 5, 0x020d7c4cu, (void *)br_func_ov006_020d7c4c},
        {data_ov006_02141730, "02141730", 6, 0x020d7c00u, (void *)br_func_ov006_020d7c00},
    };

    for (unsigned i = 0; i < sizeof seats / sizeof seats[0]; ++i) {
        MgPmf *p = &seats[i].table[seats[i].slot];
        if (p->code != seats[i].rom || p->adj != 0) {
            std::fprintf(stderr, "FATAL: dScMgBomroom_c state table %s slot %u: "
                         "the sinit left %08x/%d, the ROM's own pairs say "
                         "%08x/0 -- WRONG BYTES\n", seats[i].name,
                         seats[i].slot, p->code, p->adj, seats[i].rom);
            std::abort();
        }
        p->code = (unsigned)(size_t)seats[i].face;
    }
}
