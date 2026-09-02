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

/* host-copied further down this file, and called from above their own
   definitions -- by the address switch (020d8f98 and 020d7c00 are STATE
   BODIES as well as dispatchers) and by each other. */
void func_ov006_020d5fec(void *self);
void func_ov006_020d6278(void *self);
void func_ov006_020d65c8(void *self);
void func_ov006_020d7c00(char *c, int i);
void func_ov006_020d836c(char *c);
void func_ov006_020d8f98(unsigned char *c);
int  func_ov006_020d91b0(char *c);

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

extern "C" unsigned port_mg_bomroom_opencoded_calls(void)
{
    return g_bomroom_opencoded_calls;
}

// ---- the seven host copies -------------------------------------------------
//
// Each is its src TU verbatim except for the table declaration (MgPmf rather
// than a member-pointer type) and the dispatch site (port_mg_bomroom_callN
// rather than the member-pointer call). Nothing else moves. Every object
// offset below was re-read off the ROM disassembly rather than off the src
// struct, because a struct is where the four-byte member pointer shifts a
// layout and the disassembly is where it cannot.

/* src/func_ov006_020d91b0.cpp -- dScMgBomroom_c::Behavior, vtable SLOT 6. The
   ROM at 0x020d91b0: the halfword at +0x62f0 tested nonzero takes the timer
   arm (decrement, then a SIGNED re-read clamped at 0); zero takes the dispatch
   arm, over data_ov006_021416e0 at index *(int *)(c + 0x62d0), which is the
   index slot 0 sets to 1 and slot 18 zeroes. */
/* PORT_HOST_ABI: mwcc pointer-to-member dispatch (dScMgBomroom_c state table); the 8-byte {code,adj} pair is host-copied as an address switch, MSVC's 4-byte member pointer cannot express it */
extern "C" int func_ov006_020d91b0(char *c)
{
    if (*(unsigned short *)(c + 0x6200 + 0xf0) != 0) {
        unsigned short *t = (unsigned short *)(c + 0x62f0);
        *t = (unsigned short)(*t - 1);
        if (*(short *)(c + 0x6200 + 0xf0) <= 0)
            *(short *)(c + 0x6200 + 0xf0) = 0;
    } else {
        const MgPmf *e = &data_ov006_021416e0[*(int *)(c + 0x6000 + 0x2d0)];
        port_mg_bomroom_call0(c, e->code, e->adj);
        func_ov006_020d5d08(c);
        func_ov006_020d5b10(c);
    }
    return 1;
}

/* src/func_ov006_020d8f98.cpp -- THE OPEN-CODED ONE, and the whole reason this
   class needed a run and not only a link. Its src decodes the pair as two
   plain ints and calls through the result, which is the ROM's five
   instructions transcribed faithfully and is fatal on the host. The index is
   at +0x62d4 -- a DIFFERENT word from slot 6's +0x62d0 -- and the arity is 0:
   the ROM sets no argument register before its blx. It is also STATE SLOT 3 of
   data_ov006_021416e0, so the switch above reaches this copy and not the src.
   Its three tail calls are the other three dispatchers, in the ROM's order. */
/* PORT_HOST_ABI: mwcc pointer-to-member dispatch (dScMgBomroom_c state table); the 8-byte {code,adj} pair is host-copied as an address switch, MSVC's 4-byte member pointer cannot express it */
extern "C" void func_ov006_020d8f98(unsigned char *c)
{
    char *self = (char *)c;
    const MgPmf *e = &data_ov006_021416a0[*(int *)(self + 0x6000 + 0x2d4)];
    ++g_bomroom_opencoded_calls;
    port_mg_bomroom_call0(self, e->code, e->adj);
    func_ov006_020d65c8(self);
    func_ov006_020d6278(self);
    func_ov006_020d5fec(self);
}

/* src/func_ov006_020d836c.cpp -- 0x70 elements of stride 0x40, the live flag
   at +0x4698 of each and the state byte at +0x4697, with the loop counter
   passed as the argument. The head is the ROM's own guard: if the byte
   data_020a0de8[data_020a0e40] is zero, +0x62f6 takes 0xff. `this` is the
   CLASS BASE at every iteration -- the element cursor is only used to fetch
   the two bytes -- which the ROM says with `add r0,r6,r1,asr#1` off r6.

   ITS RECEIVER IS THE ONE THIS CLASS ALREADY GOT WRONG ONCE, run mg8 lane
   SBN, and the note is here rather than only in the seat because this is the
   definition every caller has to agree with. Five bodies call it -- 020d8ff4,
   020d8d84, 020d8cc4, 020d8af8, 020d89c4 -- and src/func_ov006_020d8cc4.cpp
   declared it `(void)` and called it with nothing. On ARM that costs no
   instruction, because the ROM's `mov r5,r0` leaves r0 holding the receiver
   across the bl, so the byte gate was green over it for two runs. On MSVC the
   call pushes nothing and the receiver arrives as the caller's saved edi.
   This body is the per-bomb tick and during top-level state 3 it has exactly
   one caller -- 020d8cc4 -- so the whole bin-full sweep waited forever for
   bombs nothing was ticking. hal/scene_mg_bomroom.cpp section 9 carries the
   measurement. Any new caller spells the receiver.

   THE HANG WAS THE LUCKY HALF OF IT. Read the body below with a garbage `c`:
   the head writes 0xff at c+0x62f6 whenever the stylus is up, which on a
   headless run is EVERY FRAME, and the loop then reads 0x70 bytes at
   c+0x4698 with stride 0x40. Every frame of the wedge did one wild write and
   a hundred and twelve wild reads into whatever the caller's saved edi
   pointed at. On the build that shipped they landed somewhere harmless and
   the symptom was a freeze; nothing about the defect guaranteed that, and the
   same source would have been a fault on a different frame layout. */
/* PORT_HOST_ABI: mwcc pointer-to-member dispatch (dScMgBomroom_c state table); the 8-byte {code,adj} pair is host-copied as an address switch, MSVC's 4-byte member pointer cannot express it */
extern "C" void func_ov006_020d836c(char *c)
{
    if (*(unsigned char *)(data_020a0de8 + data_020a0e40) == 0)
        *(unsigned char *)(c + 0x62f6) = 0xff;
    for (int i = 0; i < 0x70; i++) {
        unsigned char *o = (unsigned char *)c + i * 0x40 + 0x4000;
        if (o[0x698] != 0) {
            const MgPmf *e = &data_ov006_02141730[o[0x697]];
            port_mg_bomroom_call1(c, e->code, e->adj, i);
            func_ov006_020d69b8(c, i);
        }
    }
}

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

/* src/func_ov006_020d65c8.cpp -- two elements of stride 0x10, the live flag at
   +0x626d of each and the state byte at +0x626c, loop counter as argument. */
/* PORT_HOST_ABI: mwcc pointer-to-member dispatch (dScMgBomroom_c state table); the 8-byte {code,adj} pair is host-copied as an address switch, MSVC's 4-byte member pointer cannot express it */
extern "C" void func_ov006_020d65c8(void *self)
{
    char *c = (char *)self;
    for (int i = 0; i < 2; i++) {
        unsigned char *o = (unsigned char *)c + i * 0x10 + 0x6000;
        if (o[0x26d] != 0) {
            const MgPmf *p = &data_ov006_02141680[o[0x26c]];
            port_mg_bomroom_call1(c, p->code, p->adj, i);
        }
    }
}

/* src/func_ov006_020d6278.cpp -- the same shape as 020d65c8 with the pair of
   bytes 0x20 further on: flag at +0x628d, state at +0x628c. */
/* PORT_HOST_ABI: mwcc pointer-to-member dispatch (dScMgBomroom_c state table); the 8-byte {code,adj} pair is host-copied as an address switch, MSVC's 4-byte member pointer cannot express it */
extern "C" void func_ov006_020d6278(void *self)
{
    char *c = (char *)self;
    for (int i = 0; i < 2; i++) {
        unsigned char *o = (unsigned char *)c + i * 0x10 + 0x6000;
        if (o[0x28d] != 0) {
            const MgPmf *p = &data_ov006_021416c0[o[0x28c]];
            port_mg_bomroom_call1(c, p->code, p->adj, i);
        }
    }
}

/* src/func_ov006_020d5fec.cpp -- no loop and no element cursor: one guard byte
   at +0x62ad, one index byte at +0x62ac, and a ONE-argument slot the ROM calls
   with a constant zero (`mov r1, #0` immediately before the blx). */
/* PORT_HOST_ABI: mwcc pointer-to-member dispatch (dScMgBomroom_c state table); the 8-byte {code,adj} pair is host-copied as an address switch, MSVC's 4-byte member pointer cannot express it */
extern "C" void func_ov006_020d5fec(void *self)
{
    char *c = (char *)self;
    if (*(unsigned char *)(c + 0x6000 + 0x2ad) == 0)
        return;
    const MgPmf *p = &data_ov006_02141660[*(unsigned char *)(c + 0x6000 + 0x2ac)];
    port_mg_bomroom_call1(c, p->code, p->adj, 0);
}
