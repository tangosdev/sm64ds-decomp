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
#include <cstdlib>   /* std::abort, for the seat below (lane PMFB3) */

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

/* RETIRED, run link100 lane MGWRITER. port_mg_try_base_state and the four
   host-copy declarations that used to stand here are gone with the bodies that
   needed them. The setter's own forty addresses now sit in the ROM's own pair
   globals as HOST addresses, and the four self-field dispatchers below are
   src/ TUs again, so nothing hands a DS framework address to a switch any
   more. THE SEAT at the bottom of this file is what replaced them. */

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

/* mg_try_ov004_0 IS GONE, run link100 lane MGWRITER, and it is gone because
   every one of its twenty-eight arms is now UNREACHABLE rather than merely
   unused. Four (0x020adc80, _adcc8, _addcc, _adeb0) were retired by lane
   PMFB3, which installed host bodies into data_ov004_020beb88 and _020beb98.
   The other twenty-four are __sinit_ov004_020b955c's three tables, and THE
   SEAT below installs host bodies into all thirty of their code-bearing cells,
   so the two matched TUs that read them through an object field
   (src/func_ov004_020b31b4.cpp and src/func_ov004_020b321c.cpp, whose fields
   src/func_ov004_020b3278.cpp writes) and 020b3278's own direct dispatch on
   data_ov004_020bf490 all reach a host address with no switch in the path. A
   switch left standing beside a seated table is not a safety net: it is a
   second opinion about an address that can no longer arrive. */

/* THE HOST-WORD REFUSAL, run link100 lane MGWRITER. Everything below this line
   routes a DS address through a switch. Once a source table has been seated the
   words that come out of it are HOST addresses, and a host address handed to a
   DS switch is the one failure this lane exists to make impossible: the switch
   would not match it, mg_unhandled would report an address that reads like
   nonsense, and NOTHING WOULD BE CALLED -- which for a state machine is the
   freeze the setter's own trap used to cause. So a code word outside the DS
   address space is refused HERE, by name, rather than falling through to a
   report about an unknown DS address. It cannot fire on any path this tree has
   after the seat: every ov004 word the framework dispatches is seated and
   dispatched directly, and every remaining caller of these two entry points
   reads an UNSEATED table or field whose words are still the cartridge's own.
   If it ever does fire, the line names the seat that has to grow. */
static int mg_is_ds_code(unsigned code, const char *who)
{
    if (code >= 0x02000000u && code < 0x02400000u)
        return 1;
    std::fprintf(stderr, "  [scene] %s WAS HANDED A HOST ADDRESS 0x%08x. A "
                 "seated table's word reached the DS address switch, which "
                 "cannot match it, so nothing was called. The seat that owns "
                 "that word has to dispatch it directly. "
                 "port/unmatched/MgBase_StateDispatch.cpp\n", who, code);
    std::fflush(stderr);
    return 0;
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
    if (!mg_is_ds_code(code, "port_mg_call0"))
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
    if (!mg_is_ds_code(code, "port_mg_call1"))
        return;
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

/* run link100 lane PMFB3: the boot installer that lets the ROM's own two
   framework dispatchers read these tables directly. hal/scene_mg.cpp calls it
   after the four ov004 constructors run. */
void port_mg_framework_states_seat(void);
}

/* ---- THE TWO ALIASES THE MATCHED TUs ASK FOR (lane PMFB3) ----------------
   src/func_ov004_020add88.cpp and src/func_ov004_020adf2c.cpp declare their
   table at namespace scope rather than inside an extern "C" block, so MSVC
   spells the reference with the C++ decoration below -- read off each TU's own
   /FAsc listing (runs/link100/out/PMFB3/listings/), not guessed -- while the
   ov004 mount defines the same address and the same bytes under C linkage.
   src/ is byte-locked against mwccarm, so the sources cannot be given the
   extern "C" instead. Safe under port/tools/alternatename_guard.py for
   hal/pmfc_aliases.cpp's reason: nothing in the tree DEFINES either LHS, so
   neither can acquire a real definition and be silently defeated. */
#pragma comment(linker, "/alternatename:?data_ov004_020beb88@@3PAP8C@@AEXXZA=_data_ov004_020beb88")
#pragma comment(linker, "/alternatename:?data_ov004_020beb98@@3PAP8C@@AEXXZA=_data_ov004_020beb98")

/* ---- THE FRAMEWORK BOOT INSTALLER, run link100 lane PMFB3 ---------------
   WHAT THIS REPLACES. func_ov004_020add88 and func_ov004_020adf2c were host
   copies here, each reading its table's pair and handing the code word to
   port_mg_call0's address switch. That shape cannot retire: taking the host
   copy away takes the switch with it and the matched TU would call a DS
   address. Turning it inside out fixes it once -- install the HOST bodies into
   the four cells at boot, and the ROM's own two dispatchers reach them with no
   switch at all. Both are now src/func_ov004_020add88.cpp and
   src/func_ov004_020adf2c.cpp on port/slice_pmfb3.txt.

   NO FACE, AND THAT IS MEASURED. Both matched TUs compile the dispatch to a
   TAIL JUMP with the frame fully restored:

       mov   eax, DWORD PTR _c$[ebp]
       movsx edx, WORD PTR [eax+30]                 the index, a short at +0x1e
       mov   ecx, ?data_ov004_020beb98@@...[edx*8+4]   the adjustment word
       add   ecx, eax                                  this
       mov   eax, ?data_ov004_020beb98@@...[edx*8]     the code word
       pop   ebp
       jmp   eax

   A tail jump hands the callee the forwarder's OWN cdecl frame, so the callee
   finds the receiver at [esp+4] where its one declared argument lives, and
   returns straight to the forwarder's caller, which cleans it. That is exactly
   why lane PMFB1 could seat its thirteen FREE rows with no face, and it is why
   the four bodies below go in raw. The receiver in ecx is ignored by a cdecl
   body, and it is `this` unchanged in any case: all four adjustment words read
   zero.

   THE STRIDE, BOTH SIDES: the ROM strides eight (`add r3, r2, r1, lsl #3` at
   0x020add98 and 0x020adf3c, on the pool words 0x020addc8 = 020beb98 and
   0x020adf6c = 020beb88), and both listings emit [edx*8] and [edx*8+4]. No
   /Zp4: these tables are arrays of the bare pointer to member.

   THE FOUR SOURCE PAIRS, read out of extracted/overlays/overlay_0004.bin at
   the addresses src/__sinit_ov004_020b948c.c assigns from -- whole-pair copies,
   every one, no field-form fill (runs/link100/out/PMFB3/rom_framework.txt):
       020beb88[0] <- 020bbf4c   code=020adeb0  adjust=0
       020beb88[1] <- 020bbf5c   code=020addcc  adjust=0
       020beb98[0] <- 020bbf54   code=020adcc8  adjust=0
       020beb98[1] <- 020bbf64   code=020adc80  adjust=0
   which are the same four addresses mg_try_ov004_0 above has always routed:
   "all thirty minigames dispatch these same four addresses out of the same two
   tables". Those four cases in the switch are now unreachable -- nothing else
   in the closure carries these code words -- and are left standing because the
   switch is still the path for the other twenty-six.

   THE CHECK IS THE POINT, as in every other seat in this tree: each cell is
   compared with the ROM's own code word and a zero adjustment before anything
   is written, and either mismatch is a loud abort rather than a silent wrong
   dispatch. */
extern "C" void port_mg_framework_states_seat(void)
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
        void (*host)(void *);
    } seats[] = {
        {data_ov004_020beb88, "020beb88", 0, 0x020adeb0u,
                                        (void (*)(void *))func_ov004_020adeb0},
        {data_ov004_020beb88, "020beb88", 1, 0x020addccu,
                                        (void (*)(void *))func_ov004_020addcc},
        {data_ov004_020beb98, "020beb98", 0, 0x020adcc8u,
                                        (void (*)(void *))func_ov004_020adcc8},
        {data_ov004_020beb98, "020beb98", 1, 0x020adc80u,
                                        (void (*)(void *))func_ov004_020adc80},
    };

    for (unsigned i = 0; i < sizeof seats / sizeof seats[0]; ++i) {
        MgPmf *p = &seats[i].table[seats[i].slot];
        if (p->code != seats[i].rom || p->adj != 0) {
            std::fprintf(stderr, "FATAL: dScMgBase_c framework table %s slot "
                         "%u: the sinit left %08x/%d, the ROM's own pairs say "
                         "%08x/0 -- WRONG BYTES\n", seats[i].name,
                         seats[i].slot, p->code, p->adj, seats[i].rom);
            std::abort();
        }
        p->code = (unsigned)(size_t)seats[i].host;
    }
}

// ---- THE SEAT, run link100 lane MGWRITER -----------------------------------
//
// WHAT THIS REPLACES. Five host copies stood here: the four self-field
// dispatchers (func_ov004_020b31b4, _020b321c, _020b8714, _020b8778) and the
// writer func_ov004_020b3278. All five are src/ TUs again, on
// port/slice_mgwriter.txt. What made that possible is not a new idea about
// pointers to member; it is that the STORAGE THEY READ now holds host words.
//
// ---- 1. THE FIVE ROWS, RE-DERIVED AT THEIR OWN ROM ADDRESSES --------------
//
// Read out of extracted/overlays/overlay_0004.bin at ov004 base 0x020ad660
// (runs/link100/out/MGWRITER/rom_gate1.txt). Every one is a ZERO-ARGUMENT
// dispatch with `this` in r0, past an adjustment word whose LSB the ROM tests
// and whose value it shifts right by one -- and every measured adjustment in
// this family is zero, so `this` is the object unchanged:
//
//   func_ov004_020b31b4  ldr r1,[r0,#0x20] cmp #0x1d | ldr r1,[r0,#8] cmp #0
//                        add r3,r0,#8 | ldr r1,[r3,#4] | ldreq r1,[r3] | blx r1
//   func_ov004_020b321c  ldr r1,[r0,#0x20] cmp #0x1d | ldr r2,[r0] cmp #0
//                        ldr r1,[r0,#4] | add r3,r0,r1,asr #1 | mov r0,r3 | blx
//   func_ov004_020b8714  ldr r2,[r0,#0x18] cmp mvn #0 | ldr r1,[r0,#0x10] cmp 0
//                        add r3,r0,#0x10 | ldr r1,[r3,#4] | ldreq r1,[r3] | blx
//   func_ov004_020b8778  ldr r1,[r4,#0x18] cmp mvn #0 | bl ApproachLinear on
//                        r4+0x1c | ldr r0,[r4,#8] cmp #0 | add r3,r4,#8 | blx
//   func_ov004_020b3278  ldr r0,[r1,r2,lsl #3] cmp #0 | add r0,r8,r1,asr #1 |
//                        ldreq r1,[r3] | blx r1 -- the ONE table dispatch, on
//                        data_ov004_020bf490, stride EIGHT
//
// THE EMITTED SIDE, off each TU's own /FAsc listing under the port's own flags
// (runs/link100/out/MGWRITER/listings/, emit_gate1.txt):
//
//   the four dispatchers   mov ecx,[eax+12] / [eax+4] / [eax+20] / [esi+12]
//                          add ecx,<this> ; pop ebp ; JMP edx or eax
//   func_ov004_020b3278    mov eax,?data_ov004_020bf490@@...[ecx*8]
//                          test eax,eax ; mov ecx,...[ecx*8+4] ; add ecx,esi
//                          CALL eax, with NO push and NO add esp after it
//
// so the four are TAIL JUMPS with the frame fully restored -- the callee finds
// the receiver at [esp+4] where its one declared cdecl argument lives -- and
// 020b3278 is a __thiscall indirect with ZERO stack arguments. The emitted
// field offsets (+0x0c, +0x04, +0x14, +0x0c) agree with the ROM's own offsets
// word for word, which is what says the src structs are right at the EIGHT-byte
// member pointer /vmg /vmm gives this target (block R8 of port/CMakeLists.txt).
// So the tables below get PLAIN CDECL bodies and data_ov004_020bf490 gets
// __fastcall FACES, and that difference is measured per row rather than assumed
// per family.
//
// /Zp4 IS A MEASURED NO-OP on all five: each TU was compiled both ways and the
// only differing line in the two listings is the TITLE, which is the output
// filename. It is not claimed on any of them.
//
// ---- 2. WHY A SEAT AND NOT A SWITCH --------------------------------------
//
// An object-field row has no table for a boot installer to rewrite: the pair is
// a FIELD, written at play time. What a seat can rewrite is the source the
// writer copies FROM, and then every field written afterwards carries a host
// word for free. This file seats the thirty code-bearing cells of the three
// .bss tables __sinit_ov004_020b955c fills; port/unmatched/MgBase_StateSetter
// .cpp seats the fifty-six .data pair globals the state bodies copy into the
// object. Together they are every storage location in ov004 that can hand one
// of the eighty-four routed code words to anything, and that is a SWEEP rather
// than a hope: runs/link100/out/MGWRITER/sweep.txt takes every `load`
// relocation row in config/arm9/overlays/ov004/relocs.txt whose destination is
// one of the eighty-four, keeps the ones whose FOLLOWING word reads zero, and
// finds NINETY-NINE pairs. They account exactly: 4 + 30 + 7 + 9 + 49, which is
// the four lane PMFB3 already seated in data_ov004_020beb88 and _020beb98, the
// thirty this constructor copies FROM (dead once the destination is seated),
// the seven this lane seats for the 020b3278 object, the nine that belong to
// dScMgMemory2_c and are NOT this gate's, and the forty-nine of the setter's
// own object. Nothing else in the overlay holds one. That sweep is an
// ADDRESS-TARGETED one rather than the blind pair sweep over a range that
// port/mg_fanout_costs.txt section 4 refuses: the needles are the eighty-four
// code words the port's own two switches already routed, each with a matched
// src TU and a delink block, so a false positive would have to be a word that
// equals one of those addresses, carries the overlay's own load relocation to
// it, and is followed by a zero. One row was reported and NOT written on
// exactly that test: 0x020b7468, the literal pool word of the 0x020b7460 tail
// jump veneer, whose following word is the next function's `stmdb`.
//
// ---- 3. THE THIRTY CELLS, AND WHY THE DESTINATION AND NOT THE SOURCE -----
//
// data_ov004_020bf428, _020bf4f8 and _020bf490 are ov004 .bss past the end of
// the overlay image, filled by __sinit_ov004_020b955c out of thirty named .data
// pairs. The seat runs AFTER that constructor (hal/scene_mg.cpp calls it beside
// the other twelve), so the destination already holds the cartridge's own words
// and the destination is what is rewritten. The sources are left alone on
// purpose: lane PMFB3's framework seat checks data_ov004_020beb88 and _020beb98
// against the ROM's own DS words at that same point in the boot, and seating a
// source ahead of a sinit would make that check refuse.
//
// Nine slots hold the arm9 NULL pair {0,0} rather than a code word
// (data_ov004_020bf428.p0 and .p12, and _020bf4f8.p0 through .p4 and .p11 --
// .p0 field-form, the rest whole-pair copies of the same local). They are not
// seated because there is no cartridge word to compare against, and the ROM's
// own null-code guard is what reads them.
//
// THE CHECK IS THE POINT, as in every other seat in this tree: each cell is
// compared with the ROM's own code word and a zero adjustment before anything
// is written, and either mismatch is a loud abort rather than a silent wrong
// dispatch.

static unsigned g_fwk_field_hits;
static unsigned g_fwk_490_hits;
static unsigned g_fwk_seated;

/* ---- THE ALIAS src/func_ov004_020b3278.cpp ASKS FOR ---------------------
   That TU declares `extern PMF data_ov004_020bf490[]` at namespace scope with
   PMF a pointer to member, so MSVC encodes the member-pointer type into the
   reference and spells it as below -- read off the TU's own /FAsc listing
   (runs/link100/out/MGWRITER/listings/func_ov004_020b3278_nozp.asm), not
   guessed -- while the ov004 mount defines the same address under C linkage.
   Its seven other data references (_020beb68, _020bf3e8, _020bf428, _020bf4f8,
   _020bf560, _020bf5d4, _020bc27c) already have their aliases in
   hal/scene_mg_faces_gen.cpp and hal/scene_mg_faces.cpp. Safe under
   port/tools/alternatename_guard.py for hal/pmfc_aliases.cpp's reason: nothing
   in the tree DEFINES this LHS, so it cannot acquire a real definition and be
   silently defeated. */
#pragma comment(linker, "/alternatename:?data_ov004_020bf490@@3PAP8Base@@AEXXZA=_data_ov004_020bf490")

extern "C" {
extern MgPmf data_ov004_020bf428[];
extern MgPmf data_ov004_020bf4f8[];
extern MgPmf data_ov004_020bf490[];
}

/* The counting wrappers for the two field tables. A wrapper rather than the
   bare body because the census in hal/scene_mg.cpp has always printed how many
   framework dispatches actually happened, and once the ROM's own dispatchers
   read the table directly there is nothing left in this file for a counter to
   sit on. Same convention in and out, same body called, one increment: the
   dispatcher tail-jumps into the wrapper's own frame, the wrapper reads the
   receiver at [esp+4] exactly as the body would, and cleans nothing because the
   frame belongs to the dispatcher's caller. */
static void fw_020b35d8(void *c) { ++g_fwk_field_hits; func_ov004_020b35d8(c); }
static void fw_020b3834(void *c) { ++g_fwk_field_hits; func_ov004_020b3834(c); }
static void fw_020b3b38(void *c) { ++g_fwk_field_hits; func_ov004_020b3b38(c); }
static void fw_020b3c58(void *c) { ++g_fwk_field_hits; func_ov004_020b3c58(c); }
static void fw_020b3cb8(void *c) { ++g_fwk_field_hits; func_ov004_020b3cb8(c); }
static void fw_020b3e9c(void *c) { ++g_fwk_field_hits; func_ov004_020b3e9c(c); }
static void fw_020b4080(void *c) { ++g_fwk_field_hits; func_ov004_020b4080(c); }
static void fw_020b4360(void *c) { ++g_fwk_field_hits; func_ov004_020b4360(c); }
static void fw_020b45c0(void *c) { ++g_fwk_field_hits; func_ov004_020b45c0(c); }
static void fw_020b4820(void *c) { ++g_fwk_field_hits; func_ov004_020b4820(c); }
static void fw_020b49b8(void *c) { ++g_fwk_field_hits; func_ov004_020b49b8(c); }
static void fw_020b49f0(void *c) { ++g_fwk_field_hits; func_ov004_020b49f0(c); }
static void fw_020b4a28(void *c) { ++g_fwk_field_hits; func_ov004_020b4a28(c); }
static void fw_020b4a4c(void *c) { ++g_fwk_field_hits; func_ov004_020b4a4c(c); }

/* The faces for data_ov004_020bf490. src/func_ov004_020b3278.cpp compiles its
   dispatch to `call eax` with the receiver in ecx and NOTHING pushed, so these
   twelve cells hold ten one-cell __fastcall faces (0x020b484c and 0x020b40ac
   each fill two slots). func_ov004_020b3698 is four bytes of `bx lr` in the ROM
   and its src TU takes no argument, so its face drops the receiver, which is
   exact rather than a dropped pointer. */
static void __fastcall f490_020b3698(void *self, void *) { ++g_fwk_490_hits; func_ov004_020b3698(); }
static void __fastcall f490_020b3888(void *self, void *) { ++g_fwk_490_hits; func_ov004_020b3888(self); }
static void __fastcall f490_020b3c9c(void *self, void *) { ++g_fwk_490_hits; func_ov004_020b3c9c(self); }
static void __fastcall f490_020b40ac(void *self, void *) { ++g_fwk_490_hits; func_ov004_020b40ac(self); }
static void __fastcall f490_020b433c(void *self, void *) { ++g_fwk_490_hits; func_ov004_020b433c(self); }
static void __fastcall f490_020b484c(void *self, void *) { ++g_fwk_490_hits; func_ov004_020b484c(self); }
static void __fastcall f490_020b49e4(void *self, void *) { ++g_fwk_490_hits; func_ov004_020b49e4(self); }
static void __fastcall f490_020b4a1c(void *self, void *) { ++g_fwk_490_hits; func_ov004_020b4a1c(self); }
static void __fastcall f490_020b4a40(void *self, void *) { ++g_fwk_490_hits; func_ov004_020b4a40(self); }
static void __fastcall f490_020b4a64(void *self, void *) { ++g_fwk_490_hits; func_ov004_020b4a64(self); }

typedef void (*SeatFn)(void *);

extern "C" void port_mg_framework_tables_seat(void)
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
        SeatFn host;
    } cells[] = {
    { data_ov004_020bf428, "020bf428",  1, 0x020b4a4cu, (SeatFn)fw_020b4a4c },   /* sinit copies data_ov004_020bc22c */
    { data_ov004_020bf428, "020bf428",  2, 0x020b4a28u, (SeatFn)fw_020b4a28 },   /* sinit copies data_ov004_020bc19c */
    { data_ov004_020bf428, "020bf428",  3, 0x020b49f0u, (SeatFn)fw_020b49f0 },   /* sinit copies data_ov004_020bc184 */
    { data_ov004_020bf428, "020bf428",  4, 0x020b49b8u, (SeatFn)fw_020b49b8 },   /* sinit copies data_ov004_020bc204 */
    { data_ov004_020bf428, "020bf428",  5, 0x020b4820u, (SeatFn)fw_020b4820 },   /* sinit copies data_ov004_020bc18c */
    { data_ov004_020bf428, "020bf428",  6, 0x020b4080u, (SeatFn)fw_020b4080 },   /* sinit copies data_ov004_020bc1fc */
    { data_ov004_020bf428, "020bf428",  7, 0x020b4820u, (SeatFn)fw_020b4820 },   /* sinit copies data_ov004_020bc1f4 */
    { data_ov004_020bf428, "020bf428",  8, 0x020b4820u, (SeatFn)fw_020b4820 },   /* sinit copies data_ov004_020bc154 */
    { data_ov004_020bf428, "020bf428",  9, 0x020b4080u, (SeatFn)fw_020b4080 },   /* sinit copies data_ov004_020bc1dc */
    { data_ov004_020bf428, "020bf428", 10, 0x020b3c58u, (SeatFn)fw_020b3c58 },   /* sinit copies data_ov004_020bc1e4 */
    { data_ov004_020bf428, "020bf428", 11, 0x020b3834u, (SeatFn)fw_020b3834 },   /* sinit copies data_ov004_020bc214 */
    { data_ov004_020bf4f8, "020bf4f8",  5, 0x020b45c0u, (SeatFn)fw_020b45c0 },   /* sinit copies data_ov004_020bc1a4 */
    { data_ov004_020bf4f8, "020bf4f8",  6, 0x020b3e9cu, (SeatFn)fw_020b3e9c },   /* sinit copies data_ov004_020bc24c */
    { data_ov004_020bf4f8, "020bf4f8",  7, 0x020b45c0u, (SeatFn)fw_020b45c0 },   /* sinit copies data_ov004_020bc21c */
    { data_ov004_020bf4f8, "020bf4f8",  8, 0x020b4360u, (SeatFn)fw_020b4360 },   /* sinit copies data_ov004_020bc26c */
    { data_ov004_020bf4f8, "020bf4f8",  9, 0x020b3cb8u, (SeatFn)fw_020b3cb8 },   /* sinit copies data_ov004_020bc1cc */
    { data_ov004_020bf4f8, "020bf4f8", 10, 0x020b3b38u, (SeatFn)fw_020b3b38 },   /* sinit copies data_ov004_020bc174 */
    { data_ov004_020bf4f8, "020bf4f8", 12, 0x020b35d8u, (SeatFn)fw_020b35d8 },   /* sinit copies data_ov004_020bc194 */
    { data_ov004_020bf490, "020bf490",  1, 0x020b4a64u, (SeatFn)f490_020b4a64 },   /* sinit copies data_ov004_020bc234 */
    { data_ov004_020bf490, "020bf490",  2, 0x020b4a40u, (SeatFn)f490_020b4a40 },   /* sinit copies data_ov004_020bc23c */
    { data_ov004_020bf490, "020bf490",  3, 0x020b4a1cu, (SeatFn)f490_020b4a1c },   /* sinit copies data_ov004_020bc244 */
    { data_ov004_020bf490, "020bf490",  4, 0x020b49e4u, (SeatFn)f490_020b49e4 },   /* sinit copies data_ov004_020bc16c */
    { data_ov004_020bf490, "020bf490",  5, 0x020b484cu, (SeatFn)f490_020b484c },   /* sinit copies data_ov004_020bc1bc */
    { data_ov004_020bf490, "020bf490",  6, 0x020b40acu, (SeatFn)f490_020b40ac },   /* sinit copies data_ov004_020bc1ac */
    { data_ov004_020bf490, "020bf490",  7, 0x020b433cu, (SeatFn)f490_020b433c },   /* sinit copies data_ov004_020bc25c */
    { data_ov004_020bf490, "020bf490",  8, 0x020b484cu, (SeatFn)f490_020b484c },   /* sinit copies data_ov004_020bc264 */
    { data_ov004_020bf490, "020bf490",  9, 0x020b40acu, (SeatFn)f490_020b40ac },   /* sinit copies data_ov004_020bc1c4 */
    { data_ov004_020bf490, "020bf490", 10, 0x020b3c9cu, (SeatFn)f490_020b3c9c },   /* sinit copies data_ov004_020bc164 */
    { data_ov004_020bf490, "020bf490", 11, 0x020b3888u, (SeatFn)f490_020b3888 },   /* sinit copies data_ov004_020bc15c */
    { data_ov004_020bf490, "020bf490", 12, 0x020b3698u, (SeatFn)f490_020b3698 },   /* sinit copies data_ov004_020bc1d4 */
    };

    for (unsigned i = 0; i < sizeof cells / sizeof cells[0]; ++i) {
        MgPmf *p = &cells[i].table[cells[i].slot];
        if (p->code != cells[i].rom || p->adj != 0) {
            std::fprintf(stderr, "FATAL: dScMgBase_c framework table "
                         "data_ov004_%s slot %u: __sinit_ov004_020b955c left "
                         "%08x/%d, the ROM's own source pair says %08x/0 -- "
                         "WRONG BYTES\n", cells[i].name, cells[i].slot,
                         p->code, p->adj, cells[i].rom);
            std::abort();
        }
        p->code = (unsigned)(size_t)cells[i].host;
        ++g_fwk_seated;
    }
}

extern "C" void port_mg_framework_seat_counts(unsigned *seated, unsigned *field,
                                              unsigned *table)
{
    if (seated) *seated = g_fwk_seated;
    if (field)  *field  = g_fwk_field_hits;
    if (table)  *table  = g_fwk_490_hits;
}
