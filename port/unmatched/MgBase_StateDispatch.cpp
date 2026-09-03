// PORT_HOST_ABI. The mwcc POINTER-TO-MEMBER WALL, framework half: dScMgBase_c's
// seven dispatching TUs, host-copied against an address switch. Run link60,
// lane MG2.
//
// This is the file port/mg_fanout_costs.txt section 4 says is "worth more than
// any single minigame", and it is PAID ONCE for all thirty. The per-class half
// is unmatched/MgCurling_StateDispatch.cpp, the same shape at one class's size.
//
// ---- 1. WHAT THE ROM DOES, READ OUT OF THE ROM ----------------------------
//
// An mwcc pointer-to-member is EIGHT bytes, {code, adjustment}, and the call
// through one is five instructions. Disassembled from
// extracted/overlays/overlay_0004.bin, func_ov004_020b31b4 verbatim:
//
//     020b31dc  add   r3, r0, #8          ; &self->pmf
//     020b31e0  ldr   r1, [r3, #4]        ; adjustment
//     020b31e4  add   r0, r0, r1, asr #1  ; this += adj >> 1  (arithmetic)
//     020b31e8  ands  r1, r1, #1          ; virtual bit is the adjustment's LSB
//     020b31ec  ldrne r2, [r0]            ;   virtual: r2 = *this  (the vtable)
//     020b31f0  ldrne r1, [r3]            ;            r1 = code, a BYTE OFFSET
//     020b31f4  ldrne r1, [r2, r1]        ;            fn = vtable[code]
//     020b31f8  ldreq r1, [r3]            ;   direct:  fn = code, an ADDRESS
//     020b31fc  blx   r1
//
// MSVC's single-inheritance member pointer is FOUR bytes and its call is its
// own incompatible shape, so none of that survives a recompile. Three separate
// failures, any one fatal: the stride, the content (the word is a DS code
// address), and the dispatch sequence.
//
// EVERY ADJUSTMENT WORD IN THIS SEAT'S CLOSURE READS ZERO. Verified word for
// word out of the two ROM images: dScMgBase_c's four framework pairs at
// 0x020bbf4c / 0x020bbf54 / 0x020bbf5c / 0x020bbf64, and the twenty-five pairs
// __sinit_ov006_021304ac names for dScMgCurling_c. Both sets were taken from
// the constructors that copy them and not from an address range -- the ov006
// twenty-five run from 0x0213c1e4 to 0x0213c2bc, but that span holds
// twenty-eight slots and three of them (MgShuffleShell_SpawnInfo at
// 0x0213c214, plus 0x0213c264 and 0x0213c2ac) are not pairs and DO carry
// nonzero second words. So every dispatch this seat can actually reach is the
// DIRECT case: no this-adjustment, no vtable indirection.
// port_mg_call0 and port_mg_call1 below implement exactly that case and REPORT
// any other, rather than implementing a shape no measurement supports. A run
// that prints the report is a measurement; a run that guesses is a wild jump.
//
// ---- 2. THE DATA IS ALREADY RIGHT, AND ONLY THE CONSUMERS ARE WRONG --------
//
// This is the finding that makes the fix small, and it was not known when
// port/mg_fanout_costs.txt was written. The .bss dispatch tables are filled by
// the overlay constructors, and EVERY ONE OF THOSE CONSTRUCTORS IS A PLAIN .c
// FILE THAT SPELLS THE PAIR AS TWO INTS. src/__sinit_ov004_020b948c.c:
//
//     struct B8 { int a, b; };  struct B16 { struct B8 p, q; };
//     data_ov004_020beb88.p = data_ov004_020bbf4c;
//
// Eight bytes on MSVC, eight in the ROM, C linkage, and the mount already
// defines the storage. So the constructors copy the ROM's pairs into the
// mount's tables BYTE-FAITHFULLY AT THE RIGHT STRIDE, and they have been doing
// it correctly all along. The only broken thing in the chain is the consumer
// that re-declares the same table as an array of MSVC member pointers and
// strides it by four. That is why these host copies change ONE declaration and
// ONE call site each and leave everything else verbatim.
//
// THE COROLLARY THE FAN-OUT NEEDS, AND IT KEYS ON THE SPELLING RATHER THAN ON
// THE USE: A PAIR WHOSE CONSUMER SPELLS IT AS TWO INTS IS SAFE AS AN ALIAS. A
// PAIR WHOSE CONSUMER NAMES A MEMBER-POINTER TYPE NEEDS A HOST COPY, WHETHER IT
// IS CALLED OR ONLY COPIED.
//
// An earlier version of this note drew the line at copied-versus-called, and
// that line is wrong in a way that would authorise a bad alias. A consumer that
// only COPIES, but copies through a struct containing a real MSVC member
// pointer, moves four bytes where the ROM moves eight and shifts every field
// after it in the same object -- no call required. What makes a copy safe is
// not that it is a copy, it is that `struct Pair { int a; int b; }` is eight
// bytes on both machines.
//
// src/func_ov004_020b7cd0.cpp and src/func_ov004_020b72d4.cpp pass the test on
// the spelling: each declares `struct Pair { int a; int b; }` and stores the
// two words into the object's own state field. They are NOT here, they are two
// ordinary aliases in hal/scene_mg_faces.cpp section 2b, and the alias is right
// because eight bytes of int are eight bytes of int.
//
// ---- 3. THE WALL IS TWELVE TUs AND THE LINK ONLY NAMES SIX -----------------
//
// READ THIS BEFORE COSTING ANY OF THE OTHER TWENTY-NINE. The first link of this
// slice named 36 unresolved externals, of which SEVEN are pointer-to-member
// globals in SIX TUs, and port/mg_fanout_costs.txt costs the fan-out on that
// number. It is half the real figure. A sweep of the slice's TUs for a
// pointer-to-member dispatch finds TWELVE:
//
//   NAMED BY THE LINK, because the PMF global is declared at C++ linkage and
//   MSVC bakes the member-pointer type into the symbol:
//     func_ov004_020add88  _020adf2c  _020b3278
//     func_ov006_020e0d84  _020e12d0  _020e3528
//
//   SILENT, and for TWO DIFFERENT REASONS rather than one. Lumping them
//   together is the easy mistake here, and it hides which half is worse:
//
//     the PMF global is declared inside extern "C", so it mangles as the plain
//     C name the mount already defines and the linker is satisfied:
//       func_ov006_020e1214  _020e3078
//
//     THERE IS NO PMF GLOBAL AT ALL. The member pointer is a FIELD OF THE
//     OBJECT, so these four emit no external symbol for it in any linkage and
//     there was never anything for a link to name:
//       func_ov004_020b31b4  _020b321c  _020b8714  _020b8778
//
// So A LINK IS NOT A COMPLETE DETECTOR FOR THIS WALL, and the conclusion is
// STRONGER for the four than for the two. The two could in principle be caught
// by a lane that re-declared the table at C++ linkage and re-linked; the four
// cannot be caught by any link, ever, because there is no symbol to resolve.
// Only reading the source finds them.
//
// The four are also the worse defect. They dispatch the OBJECT'S OWN pmf field
// rather than a table, so MSVC's four-byte member pointer moves every field
// after it: src/func_ov004_020b31b4.cpp puts `state` at 0x1c where the ROM
// reads [r0,#0x20]. That is a wrong READ before any dispatch happens, where
// the two table cases are a wrong stride within a correct object. Their
// layouts are re-derived from the disassembly below, one offset at a time,
// rather than from the src structs.
//
// ---- 4. WHAT IS NOT HERE, AND IT IS NOT A TRAP ANY MORE --------------------
//
// func_ov004_020b87e0, dScMgBase_c's state SETTER, is a different problem from
// these seven: it does not dispatch a table the mount holds, it BUILDS a
// twenty-entry static table out of twenty ov004 globals whose MSVC symbol names
// carry the member-pointer type, so there is nothing for an alias or a stride
// fix to attach to. It needs its twenty addresses routed the way these are, and
// that was a lane of its own.
//
// AMENDED, run mg5 lane BASESET. THAT LANE RAN. The setter is host-copied in
// port/unmatched/MgBase_StateSetter.cpp, its trap is gone from
// hal/scene_mg_faces.cpp, and port_mg_try_base_state below carries its forty
// addresses: the twenty its own table holds and the twenty its state bodies
// install as per-frame ticks. The second twenty are this file's business too,
// because they are dispatched by func_ov004_020b8714 and func_ov004_020b8778
// further down, both of which return on their first line while the message
// object's +0x18 reads -1. The setter is the only writer of that field, so
// while it was a trap NEITHER of those two host copies ever reached its call.
// That is why this file measured 0 UNHANDLED addresses on runs that were in
// fact dispatching nothing at all, and it is worth reading twice before
// treating a zero here as coverage.
//
// ---- 5. AMENDED, run mg5 lane FWK. A PREMISE FURTHER DOWN HAS EXPIRED ------
//
// THE CORRECTION FIRST, because it is the thing that hid this hole. The
// func_ov004_020b3278 block at the bottom of this file says
// data_ov004_020bf490 "reads zero" on the port, and it argues that from this
// premise:
//
//     "no sinit in src/ writes it: the constructor that would is
//      __sinit_ov004_020b955c, one of the two with a config symbol, no delink
//      block and no source."
//
// THAT PREMISE IS NO LONGER TRUE, and it was re-derived here rather than taken
// from the sentence. src/__sinit_ov004_020b955c.c EXISTS and is a full body;
// config/arm9/overlays/ov004/delinks.txt carries its block
// (`src/__sinit_ov004_020b955c.c: complete / .init start:0x020b955c
// end:0x020b9ad0`); port/slice_mg1.txt already lists it; and a scene boot
// prints `ov004 4/4` overlay constructors run, which is the runtime witness
// that it fired. So all THREE of the tables that constructor fills are live
// data on the port now, not zeros:
//
//   data_ov004_020bf490  13 pairs  dispatched DIRECTLY by func_ov004_020b3278
//   data_ov004_020bf428  13 pairs  copied to self+0x00/+0x04, dispatched by
//                                  func_ov004_020b321c
//   data_ov004_020bf4f8  13 pairs  copied to self+0x08/+0x0c, dispatched by
//                                  func_ov004_020b31b4
//
// The last two are the ones that block calls "the same shape". They are not
// dispatched by 020b3278 at all: 020b3278 STORES them into the object as the
// object's own pmf fields, and the two self-field dispatchers listed above
// call them later. That block read them as inert `Pair` copies because at the
// time the table was zero and nothing ever came back out of it.
//
// The consequence is that its "nothing is dispatched" is now the
// opposite of what happens, and the switch below was short by the whole set.
// A scene 366 boot (dScMgLuigi_c, run mg5 lane LUI) reported 174 dispatches
// hitting the mg_unhandled path at TWO addresses, both of them slot 12:
//
//   data_ov004_020bf490.p12 = data_ov004_020bc1d4 -> code 0x020b3698  adj 0
//   data_ov004_020bf4f8.p12 = data_ov004_020bc194 -> code 0x020b35d8  adj 0
//
// Both pair words were read out of extracted/overlays/overlay_0004.bin at
// (addr - 0x020ad660), and both are confirmed by the overlay's own relocation
// rows rather than by the raw image alone:
//
//   config/arm9/overlays/ov004/relocs.txt:1878
//       from:0x020bc194 kind:load to:0x020b35d8 module:overlay(4)
//   config/arm9/overlays/ov004/relocs.txt:1886
//       from:0x020bc1d4 kind:load to:0x020b3698 module:overlay(4)
//
// The constructor makes 39 slot assignments across the three tables; 30 of them
// name an ov004 .data pair by symbol and the other 9 copy the same arm9 pair
// (data_02086b58) through a local. Every one of those 30 pairs was resolved out
// of the image here and EVERY SECOND WORD READS ZERO, so section 1's ruling
// still holds and both of these go through the DIRECT case.
//
// ---- 5a. THE CALLING CONVENTION, DISASSEMBLED AND NOT ASSUMED -------------
//
// BOTH SLOTS ARE ZERO-ARGUMENT SLOTS. The two dispatch sites are byte-identical
// in shape and neither sets up an argument register:
//
//   func_ov004_020b31b4, the self+8 dispatcher
//     020b31dc  add   r3, r0, #8
//     020b31e0  ldr   r1, [r3, #4]        ; adjustment
//     020b31e4  add   r0, r0, r1, asr #1  ; adj 0, so r0 stays `this`
//     020b31f8  ldreq r1, [r3]            ; r1 is OVERWRITTEN with the code
//     020b31fc  blx   r1
//
//   func_ov004_020b3278, the direct call on data_ov004_020bf490
//     020b3540  ldr   r0, [r1, r2, lsl #3]  ; stride 8, the pair
//     020b3548  cmp   r0, #0                ; the ROM's own null-code guard
//     020b3558  add   r0, r8, r1, asr #1    ; adj 0, so r0 stays `this`
//     020b356c  ldreq r1, [r3]
//     020b3570  blx   r1
//
// r0 is `this` and r1 is destroyed by the code word itself, so nothing rides
// through in r1 and there is no argument for a callee to drop. Against that:
//
//   func_ov004_020b35d8  0xc0 bytes, and it OPENS `mov r7, r0` then reads
//     [r7,#0x34], [r7,#0x10], [r7,#0x12], [r7,#0x18], [r7,#0x1c]. It never
//     touches r1. One argument, `this`, and src/func_ov004_020b35d8.c declares
//     exactly that (`struct Obj *self` with fields at 0x10/0x12/0x18/0x1c/0x34,
//     which is the disassembly field for field). It is called with one
//     argument below and that is not a ride-through, it is the plain case.
//
//     The object it wants is the SAME object func_ov004_020b3278 writes: that
//     body sets [self+0x10], [self+0x12], [self+0x18], [self+0x1c] from its own
//     arguments and hands `self+0x34` to func_ov004_020af5e0. So the `this` the
//     dispatch delivers is the one the body's fields were filled by, and that
//     agreement is independent of the dispatch reasoning above.
//
//   func_ov004_020b3698  0x4 bytes. The whole body is `bx lr`. It reads
//     nothing, so calling it as (void) is exact. This is the benign ARM
//     ride-through unmatched/MgCurling_StateDispatch.cpp documents for
//     func_ov006_020e2eb8 and NOT the scene_actor_faces defect class: there is
//     no callee behind it that wanted an argument, because there is no callee.
//
// NEITHER NEEDS A HOST COPY. Both are matched src TUs with delink blocks; the
// only thing missing was three slice lines and two cases. src/ is untouched.
//
// ---- 5b. WHAT IS STILL SHORT, MEASURED AND LEFT ON THE TABLE --------------
//
// Closing slot 12 is not closing the framework. Resolving the 30 pair-bearing
// slots of __sinit_ov004_020b955c out of the overlay image gives TWENTY-FOUR
// DISTINCT CODE ADDRESSES across the three tables, and before this change the
// switch below routed NONE of them. Every one has a matched src TU and a delink
// block, and none of them was in any slice, so the whole set is wiring and not
// decomposition. Scene 366 only exposes two of them because
// func_ov004_020b3278 reaches slot 12 alone on that scene (its `case 13:` arm
// writes 0xc to [self+0x2e]); a scene that drives a different graphic id will
// meet the other twenty-one the same way this one met these two.
//
// THE OTHER TWENTY-TWO ARE STILL UNROUTED, and each one will print the same
// report the day a scene reaches it. They are left for a lane that can boot a
// scene which actually exercises them: routing an address no run touches buys a
// slice line, a closure, and no evidence that any of it is right. The full
// table-and-slot listing is this lane's scoping artifact rather than a comment,
// because it is a worklist and not a ruling.
//
// ---- 5c. READING THE CENSUS LINE AFTER THIS CHANGE ------------------------
//
// hal/scene_mg.cpp prints `calls`, `port_mg_curling_state_hits()` and
// `unknown` from port_mg_dispatch_counts. The framework-routed count is not
// printed separately and does not need to be: it is
// calls - curling_hits - unknown. On a scene 366 boot that reads 174 - 0 - 0
// after this change where it read 174 - 0 - 174 before, and the total `calls`
// is unchanged, which is the shape that says the same dispatches now land
// somewhere instead of nowhere.

#include <cstdio>

/* The eight-byte mwcc member pointer, in the only spelling that is true on
   both machines: two words, no member-pointer type anywhere. */
struct MgPmf { unsigned code; int adj; };

extern "C" {

/* ---- the ov004 state bodies these tables and fields hold ---------------- */
/* Reached ONLY through the switch below. None of the four had a caller in the
   build before this file: the pair words are mounted DATA holding DS
   addresses, so /OPT:REF had dropped all four. They join port/slice_mg1.txt in
   the same commit as this file, which is what gives them one. */
void func_ov004_020adc80(int *c);
void func_ov004_020adcc8(short *obj);
void func_ov004_020addcc(char *r5);
void func_ov004_020adeb0(char *c);

/* THE TWO SLOT-12 STATES, added by run mg5 lane FWK; section 5 is the
   derivation. Both are matched src TUs with delink blocks, both are reached
   from a ZERO-ARGUMENT slot, and neither needed a host copy.
     020b35d8  src/func_ov004_020b35d8.c   0xc0 bytes, reads `this` only
     020b3698  src/func_ov004_020b3698.c   0x4 bytes, the whole body is `bx lr`
   Their three slice lines (these two plus src/func_ov004_020b1aec.c, the one
   callee of 020b35d8 the slice did not already carry) join port/slice_mg1.txt
   in the same commit as these cases, which is what gives them a caller. */
void func_ov004_020b35d8(void *self);
void func_ov004_020b3698(void);

/* THE OTHER TWENTY-TWO, added by run mg5 lane BASESET. Section 5b left these on
   the table with the note that "each one will print the same report the day a
   scene reaches it", and it was right: seating the framework state setter made
   a scene 378 sweep reach SEVEN of them at once (0x020b4360, 0x020b4820,
   0x020b484c, 0x020b49b8, 0x020b49e4, 0x020b49f0, 0x020b4a1c), 2561 times
   between them, on the first run after the seat.

   THE WHOLE TWENTY-TWO ARE ROUTED RATHER THAN THE SEVEN A RUN WITNESSED, and
   that is a deliberate departure from 5b's rule. 5b's argument was that routing
   an address no run touches buys a slice line and no evidence, and that argument
   holds when the addresses are a heterogeneous set. These are not: all
   twenty-four come from ONE constructor, __sinit_ov004_020b955c, whose thirty
   pair-bearing assignments were re-resolved here out of
   extracted/overlays/overlay_0004.bin one assignment at a time (NOT swept over
   an address range, per mg_fanout_costs section 4) and EVERY SECOND WORD READS
   ZERO. All three destination tables are dispatched by the three sites section
   5a disassembles, all zero-argument with `this` in r0. Every one of the
   twenty-four has a matched src TU and a delink block, a `::*` and `->*` sweep
   over all twenty-two new files is clean, and the only zero-argument extern any
   of them declares is NullDestructor_0203d47c, which is one `bx lr` in the ROM.
   So the evidence here is per-FAMILY and the seven that ran are its witness.

   The cost of the other rule is what changed the call: with the setter seated,
   the framework runs in a player's hands, and an unrouted address is a dead end
   a player meets rather than a line a lane owes.

   Their twenty-two slice lines join port/slice_mg1.txt in the same commit as
   these cases, which is what gives them a caller. Each takes one pointer, its
   src TU's own spelling, and none needed a host copy. */
void func_ov004_020b3834(void *self);
void func_ov004_020b3888(void *self);
void func_ov004_020b3b38(void *self);
void func_ov004_020b3c58(void *self);
void func_ov004_020b3c9c(void *self);
void func_ov004_020b3cb8(void *self);
void func_ov004_020b3e9c(void *self);
void func_ov004_020b4080(void *self);
void func_ov004_020b40ac(void *self);
void func_ov004_020b433c(void *self);
void func_ov004_020b4360(void *self);
void func_ov004_020b45c0(void *self);
void func_ov004_020b4820(void *self);
void func_ov004_020b484c(void *self);
void func_ov004_020b49b8(void *self);
void func_ov004_020b49e4(void *self);
void func_ov004_020b49f0(void *self);
void func_ov004_020b4a1c(void *self);
void func_ov004_020b4a28(void *self);
void func_ov004_020b4a40(void *self);
void func_ov004_020b4a4c(void *self);
void func_ov004_020b4a64(void *self);

/* the per-class half's switch, tried after this one; the header of
   unmatched/MgCurling_StateDispatch.cpp says why the chain runs this way */
int port_mg_try_ov006_0(void *self, unsigned code);
int port_mg_try_ov006_1(void *self, unsigned code, int a);

/* Run mg5 lane BASESET. The framework's STATE SETTER half, forty more ov004
   addresses: the twenty src/func_ov004_020b87e0.cpp's own table holds and the
   twenty its state bodies install as per-frame ticks. It joins the chain here
   rather than growing mg_try_ov004_0, because the two sets are derived from
   different ROM structures and a reader should be able to tell which file ruled
   which address. port/unmatched/MgBase_StateSetter.cpp. */
int port_mg_try_base_state(void *self, unsigned code);

/* the bodies the host copies below call, unchanged from their src */
void DecompressLZ16(int src, int dst);
int  func_ov004_020af5e0(int a, void *b, int c, int d);
void func_ov004_020b42c0(char *self);   /* derefs [this+0x34/0x10/0x12/0x1c/0x18/0x20] */
void _Z14ApproachLinearRiii(int &, int, int);

}  /* extern "C" */

// ---- the address switch ----------------------------------------------------

static unsigned g_mg_dispatch_calls;
static unsigned g_mg_dispatch_unknown;

/* One line per distinct unhandled address, so a per-frame loop cannot flood
   the log and a single occurrence cannot hide in one. */
static void mg_unhandled(const char *what, unsigned code, int adj)
{
    static unsigned said[16];
    static int nsaid;
    ++g_mg_dispatch_unknown;
    for (int i = 0; i < nsaid; ++i)
        if (said[i] == code)
            return;
    if (nsaid < 16)
        said[nsaid++] = code;
    std::fprintf(stderr, "  [scene] MINIGAME STATE DISPATCH %s: DS address "
                 "0x%08x (adjustment 0x%08x). Nothing was called. "
                 "port/unmatched/MgBase_StateDispatch.cpp\n", what, code,
                 (unsigned)adj);
    std::fflush(stderr);
}

/* dScMgBase_c's own four. The framework half of the switch, and the reason
   this file is paid once: all thirty minigames dispatch these same four
   addresses out of the same two tables. */
static int mg_try_ov004_0(void *self, unsigned code)
{
    switch (code) {
    case 0x020adc80u: func_ov004_020adc80((int *)self);   return 1;
    case 0x020adcc8u: func_ov004_020adcc8((short *)self); return 1;
    case 0x020addccu: func_ov004_020addcc((char *)self);  return 1;
    case 0x020adeb0u: func_ov004_020adeb0((char *)self);  return 1;
    /* run mg5 lane FWK. data_ov004_020bf4f8.p12, copied to self+0x08 by
       func_ov004_020b3278 and dispatched by func_ov004_020b31b4. One argument,
       `this`, which is the same object 020b3278 filled the fields of. */
    case 0x020b35d8u: func_ov004_020b35d8(self);          return 1;
    /* run mg5 lane FWK. data_ov004_020bf490.p12, dispatched directly by
       func_ov004_020b3278. The ROM body is one `bx lr`, so (void) is exact. */
    case 0x020b3698u: func_ov004_020b3698();              return 1;

    /* run mg5 lane BASESET: the other twenty-two of __sinit_ov004_020b955c's
       twenty-four. The comment above their declarations is the derivation. The
       slot each one arrives from is named so a reader can go back to the
       constructor without re-resolving anything; * marks the seven a scene 378
       sweep reached on the first run after the state setter was seated. */
    case 0x020b3834u: func_ov004_020b3834(self);          return 1;  /* bf428.p11 */
    case 0x020b3888u: func_ov004_020b3888(self);          return 1;  /* bf490.p11 */
    case 0x020b3b38u: func_ov004_020b3b38(self);          return 1;  /* bf4f8.p10 */
    case 0x020b3c58u: func_ov004_020b3c58(self);          return 1;  /* bf428.p10 */
    case 0x020b3c9cu: func_ov004_020b3c9c(self);          return 1;  /* bf490.p10 */
    case 0x020b3cb8u: func_ov004_020b3cb8(self);          return 1;  /* bf4f8.p9  */
    case 0x020b3e9cu: func_ov004_020b3e9c(self);          return 1;  /* bf4f8.p6  */
    case 0x020b4080u: func_ov004_020b4080(self);          return 1;  /* bf428.p6, p9 */
    case 0x020b40acu: func_ov004_020b40ac(self);          return 1;  /* bf490.p6, p9 */
    case 0x020b433cu: func_ov004_020b433c(self);          return 1;  /* bf490.p7  */
    case 0x020b4360u: func_ov004_020b4360(self);          return 1;  /* bf4f8.p8  * */
    case 0x020b45c0u: func_ov004_020b45c0(self);          return 1;  /* bf4f8.p5, p7 */
    case 0x020b4820u: func_ov004_020b4820(self);          return 1;  /* bf428.p5, p7, p8 * */
    case 0x020b484cu: func_ov004_020b484c(self);          return 1;  /* bf490.p5, p8 * */
    case 0x020b49b8u: func_ov004_020b49b8(self);          return 1;  /* bf428.p4  * */
    case 0x020b49e4u: func_ov004_020b49e4(self);          return 1;  /* bf490.p4  * */
    case 0x020b49f0u: func_ov004_020b49f0(self);          return 1;  /* bf428.p3  * */
    case 0x020b4a1cu: func_ov004_020b4a1c(self);          return 1;  /* bf490.p3  * */
    case 0x020b4a28u: func_ov004_020b4a28(self);          return 1;  /* bf428.p2  */
    case 0x020b4a40u: func_ov004_020b4a40(self);          return 1;  /* bf490.p2  */
    case 0x020b4a4cu: func_ov004_020b4a4c(self);          return 1;  /* bf428.p1  */
    case 0x020b4a64u: func_ov004_020b4a64(self);          return 1;  /* bf490.p1  */

    default:                                              return 0;
    }
}

/* THE ONE ENTRY POINT for a zero-argument state call. Both host-copy files
   route through it, so there is exactly one place that decides what an
   adjustment word means. */
extern "C" void port_mg_call0(void *self, unsigned code, int adj)
{
    ++g_mg_dispatch_calls;
    if (code == 0)
        return;                       /* the ROM's own null-pmf guard */
    if (adj != 0) {
        /* No pair in this seat's closure carries one, so there is no case to
           verify an implementation against. The this-adjustment is one line
           and the virtual branch is a vtable read at a byte offset, but a
           dispatch shape nobody has measured is exactly the plausible body
           port/tools/inferred_stub_guard exists to refuse. */
        mg_unhandled("with a NONZERO ADJUSTMENT, which no measured pair in "
                     "this closure has", code, adj);
        return;
    }
    if (mg_try_ov004_0(self, code))
        return;
    if (port_mg_try_base_state(self, code))
        return;
    if (port_mg_try_ov006_0(self, code))
        return;
    mg_unhandled("UNHANDLED", code, adj);
}

extern "C" void port_mg_call1(void *self, unsigned code, int adj, int a)
{
    ++g_mg_dispatch_calls;
    if (code == 0)
        return;
    if (adj != 0) {
        mg_unhandled("with a NONZERO ADJUSTMENT, which no measured pair in "
                     "this closure has", code, adj);
        return;
    }
    /* ov004 contributes no one-argument state table to this closure: both of
       dScMgBase_c's are zero-argument. The chain still runs through here so a
       derived class's table holding a framework address finds it. */
    if (port_mg_try_ov006_1(self, code, a))
        return;
    mg_unhandled("UNHANDLED", code, adj);
}

extern "C" void port_mg_dispatch_counts(unsigned *calls, unsigned *unknown)
{
    if (calls)   *calls   = g_mg_dispatch_calls;
    if (unknown) *unknown = g_mg_dispatch_unknown;
}

// ---- the seven host copies -------------------------------------------------
//
// Each is its src TU with the pointer-to-member declaration replaced by MgPmf
// and the dispatch replaced by port_mg_call0. Everything else is verbatim,
// INCLUDING the other extern declarations: those keep their C++ spellings so
// the nineteen generated aliases in hal/scene_mg_faces_gen.cpp still have the
// references they were generated for. The twelve src lines are commented out
// of port/slice_mg1.txt with a pointer back here.

extern "C" {
/* the two framework tables, re-typed. The mount defines the storage and
   __sinit_ov004_020b948c fills it; see section 2. */
extern MgPmf data_ov004_020beb88[];
extern MgPmf data_ov004_020beb98[];
}

/* src/func_ov004_020add88.cpp. The src's struct C { char pad[0x1e]; short idx; }
   is right and is kept: idx is a short at 0x1e and no member pointer sits
   inside this object, so nothing shifts. */
namespace { struct CIdx1e { char pad[0x1e]; short idx; }; }

/* PORT_HOST_ABI: mwcc pointer-to-member dispatch (dScMgBase_c framework); the 8-byte {code,adj} pair and five-instruction blx sequence are host-copied as an address switch, MSVC's 4-byte member pointer cannot express them */
extern "C" void func_ov004_020add88(void *p)
{
    CIdx1e *c = (CIdx1e *)p;
    const MgPmf *e = &data_ov004_020beb98[c->idx];
    port_mg_call0(c, e->code, e->adj);
}

/* src/func_ov004_020adf2c.cpp, the same shape on the other table. */
/* PORT_HOST_ABI: mwcc pointer-to-member dispatch (dScMgBase_c framework); the 8-byte {code,adj} pair and five-instruction blx sequence are host-copied as an address switch, MSVC's 4-byte member pointer cannot express them */
extern "C" void func_ov004_020adf2c(void *p)
{
    CIdx1e *c = (CIdx1e *)p;
    const MgPmf *e = &data_ov004_020beb88[c->idx];
    port_mg_call0(c, e->code, e->adj);
}

// ---- the four self-field dispatchers ---------------------------------------
//
// These read the pmf out of the OBJECT, so the four-byte MSVC member pointer
// in their src structs moves every later field. The offsets below are the
// ROM's, read off the disassembly rather than off the struct:
//
//   func_ov004_020b31b4   state [r0,#0x20]   pmf r0+0x08
//   func_ov004_020b321c   state [r0,#0x20]   pmf r0+0x00
//   func_ov004_020b8714   field [r0,#0x18]   pmf r0+0x10
//   func_ov004_020b8778   field [r4,#0x18]   pmf r4+0x08, ApproachLinear on +0x1c
//
// Two of them agree on state at 0x20 with the pmf at different offsets, which
// is two different classes and not a transcription slip: 020b31b4 opens the
// pair with `add r3, r0, #8` and 020b321c with `ldr r2, [r0]`.

static inline const MgPmf *mg_self_pmf(void *self, unsigned off)
{
    return (const MgPmf *)((char *)self + off);
}

/* src/func_ov004_020b31b4.cpp */
/* PORT_HOST_ABI: mwcc pointer-to-member dispatch (dScMgBase_c framework); the 8-byte {code,adj} pair and five-instruction blx sequence are host-copied as an address switch, MSVC's 4-byte member pointer cannot express them */
extern "C" void func_ov004_020b31b4(void *self)
{
    char *c = (char *)self;
    if (*(int *)(c + 0x20) == 0x1d)
        return;
    const MgPmf *p = mg_self_pmf(c, 8);
    if (p->code != 0) {
        port_mg_call0(c, p->code, p->adj);
        return;
    }
    func_ov004_020b42c0(c);
}

/* src/func_ov004_020b321c.cpp. The ROM moves `this` to the adjusted pointer
   before the call (mov r0, r3); with every measured adjustment zero that is
   the object itself, and port_mg_call0 refuses any nonzero one. */
/* PORT_HOST_ABI: mwcc pointer-to-member dispatch (dScMgBase_c framework); the 8-byte {code,adj} pair and five-instruction blx sequence are host-copied as an address switch, MSVC's 4-byte member pointer cannot express them */
extern "C" void func_ov004_020b321c(void *self)
{
    char *c = (char *)self;
    if (*(int *)(c + 0x20) == 0x1d)
        return;
    const MgPmf *p = mg_self_pmf(c, 0);
    if (p->code == 0)
        return;
    port_mg_call0(c, p->code, p->adj);
}

/* src/func_ov004_020b8714.cpp */
/* PORT_HOST_ABI: mwcc pointer-to-member dispatch (dScMgBase_c framework); the 8-byte {code,adj} pair and five-instruction blx sequence are host-copied as an address switch, MSVC's 4-byte member pointer cannot express them */
extern "C" void func_ov004_020b8714(void *self)
{
    char *c = (char *)self;
    if (*(int *)(c + 0x18) == -1)
        return;
    const MgPmf *p = mg_self_pmf(c, 0x10);
    if (p->code == 0)
        return;
    port_mg_call0(c, p->code, p->adj);
}

/* src/func_ov004_020b8778.cpp */
/* PORT_HOST_ABI: mwcc pointer-to-member dispatch (dScMgBase_c framework); the 8-byte {code,adj} pair and five-instruction blx sequence are host-copied as an address switch, MSVC's 4-byte member pointer cannot express them */
extern "C" void func_ov004_020b8778(void *self)
{
    char *c = (char *)self;
    if (*(int *)(c + 0x18) == -1)
        return;
    _Z14ApproachLinearRiii(*(int *)(c + 0x1c), 0, 1);
    const MgPmf *p = mg_self_pmf(c, 8);
    if (p->code == 0)
        return;
    port_mg_call0(c, p->code, p->adj);
}

// ---- func_ov004_020b3278 ---------------------------------------------------
//
// The one big body in the set, and the only one whose OTHER declarations
// matter: it names seven globals at C++ linkage and seven of the nineteen
// generated aliases exist for exactly those references. They are transcribed
// unchanged for that reason. Only `PMF data_ov004_020bf490[]` becomes MgPmf.
//
// data_ov004_020bf490 IS ov004 .bss PAST THE END OF THE OVERLAY IMAGE
// (overlay_0004.bin covers 0x020ad660..0x020beb60), so nothing in the image
// itself fills it. The constructor that fills it is __sinit_ov004_020b955c, and
// that constructor RUNS on the port: src/__sinit_ov004_020b955c.c is a full
// body, config/arm9/overlays/ov004/delinks.txt carries its block,
// port/slice_mg1.txt lists it, and a scene boot prints `ov004 4/4`. So the
// table holds live pairs, the ROM's own `if (data_ov004_020bf490[st])` guard
// passes, and the slots ARE dispatched.
//
// This block used to argue the opposite, from a premise about that constructor
// having no delink block and no source. Section 5 at the top of the file
// records the correction, the hole the stale premise hid, which of these slots
// the switch below routes today, and how data_ov004_020bf428 and _020bf4f8 two
// blocks down reach their callees, which is not through this body.

struct Obj {
    virtual int m00(); virtual int m01(); virtual int m02(); virtual int m03();
    virtual int m04(); virtual int m05(); virtual int m06(); virtual int m07();
    virtual int m08(); virtual int m09(); virtual int m10(); virtual int m11();
    virtual int m12(); virtual int m13(); virtual int m14(); virtual int m15();
    virtual int m16(); virtual int m17(); virtual int m18(); virtual int m19();
    virtual int m20(); virtual int m21(); virtual int m22(); virtual int m23();
    virtual int m24(); virtual int m25(); virtual int m26();
};

struct Pair { int w0; int w1; };
struct S3 { int v[3]; };

extern Obj *data_ov004_020beb68;
extern unsigned char data_ov004_020bf3e8[];
extern int data_ov004_020bf560[];
extern S3 data_ov004_020bc27c;
extern int data_ov004_020bf5d4[];
extern Pair data_ov004_020bf428[];
extern Pair data_ov004_020bf4f8[];
extern "C" MgPmf data_ov004_020bf490[];

/* PORT_HOST_ABI: mwcc pointer-to-member dispatch (dScMgBase_c framework); the 8-byte {code,adj} pair and five-instruction blx sequence are host-copied as an address switch, MSVC's 4-byte member pointer cannot express them */
extern "C" void func_ov004_020b3278(char *self, int arg1, short arg2, short arg3, int arg4, int arg5, short arg6)
{
    int a, b;

    if (data_ov004_020beb68->m26() == 2) {
        a = 0x6400000;
        b = 0;
    } else {
        a = 0x6600000;
        b = 0x6400000;
    }

    switch (arg1) {
    case 3: case 4: case 5: case 6:
    case 8: case 9: case 10: case 11: case 12:
    case 14: case 16: case 17: case 18: case 19: case 20: case 21:
        if (data_ov004_020bf3e8[0] != 0)
            return;
        DecompressLZ16(data_ov004_020bf560[arg1], a + 0x7000);
        if (b != 0)
            DecompressLZ16(data_ov004_020bf560[arg1], b + 0x7000);
        *(short *)(self + 0x30) = 0;
        data_ov004_020bf3e8[0] = 1;
        break;
    default:
        if (data_ov004_020bf3e8[1] == 0) {
            DecompressLZ16(data_ov004_020bf560[arg1], a + 0x6000);
            if (b != 0)
                DecompressLZ16(data_ov004_020bf560[arg1], b + 0x6000);
            *(short *)(self + 0x30) = 1;
            data_ov004_020bf3e8[1] = 1;
            break;
        }
        if (data_ov004_020bf3e8[2] == 0) {
            DecompressLZ16(data_ov004_020bf560[arg1], a + 0x6800);
            if (b != 0)
                DecompressLZ16(data_ov004_020bf560[arg1], b + 0x6800);
            *(short *)(self + 0x30) = 2;
            data_ov004_020bf3e8[2] = 1;
            break;
        }
        return;
    }

    *(int *)(self + 0x20) = arg1;
    *(short *)(self + 0x10) = arg2;
    *(short *)(self + 0x12) = arg3;
    *(short *)(self + 0x14) = *(short *)(self + 0x10);
    *(short *)(self + 0x16) = *(short *)(self + 0x12);
    *(int *)(self + 0x1c) = arg4;
    *(int *)(self + 0x18) = arg5;
    *(short *)(self + 0x32) = 0;

    if (arg6 != 0xd) {
        *(short *)(self + 0x2e) = arg6;
    } else {
        switch (arg1) {
        case 11:
            *(short *)(self + 0x2e) = 7;
            break;
        case 3: case 4: case 5: case 6: case 20: case 21:
            *(short *)(self + 0x2e) = 8;
            break;
        case 8: case 14:
            *(short *)(self + 0x2e) = 9;
            break;
        case 0:
            *(short *)(self + 0x2e) = 3;
            break;
        case 1: case 2:
            *(short *)(self + 0x2e) = 4;
            break;
        case 13:
            *(short *)(self + 0x2e) = 0xc;
            break;
        default:
            *(short *)(self + 0x2e) = 0;
            break;
        }
    }

    {
        S3 tmp = data_ov004_020bc27c;
        *(short *)(self + 0x2c) = (short)func_ov004_020af5e0(
            data_ov004_020bf5d4[*(int *)(self + 0x20)],
            self + 0x34,
            tmp.v[*(short *)(self + 0x30)],
            *(int *)(self + 0x20));
    }

    /* THE ONE CHANGED CALL. The src reads
           if (data_ov004_020bf490[st])
               (((Base *)self)->*data_ov004_020bf490[st])();
       and its null test is a test of the code word, which is what it becomes. */
    {
        int st = *(short *)(self + 0x2e);
        const MgPmf *p = &data_ov004_020bf490[st];
        if (p->code != 0)
            port_mg_call0(self, p->code, p->adj);
    }

    {
        short st;
        Pair *e;
        st = *(short *)(self + 0x2e);
        e = &data_ov004_020bf428[st];
        a = data_ov004_020bf428[st].w0;
        b = e->w1;
        *(int *)(self + 0) = b ? a : a;
        *(int *)(self + 4) = b;
        st = *(short *)(self + 0x2e);
        e = &data_ov004_020bf4f8[st];
        a = data_ov004_020bf4f8[st].w0;
        b = e->w1;
        *(int *)(self + 8) = b ? a : a;
        *(int *)(self + 0xc) = b;
    }
}
