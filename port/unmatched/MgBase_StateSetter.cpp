// PORT_HOST_ABI. The mwcc POINTER-TO-MEMBER WALL, the framework's STATE SETTER:
// src/func_ov004_020b87e0.cpp, host-copied against an address switch. Run mg5,
// lane BASESET.
//
// This is the TU port/mg_fanout_costs.txt section 4 calls "worth more than any
// single minigame", the one thing in the minigame stack that was still a
// reporting trap after the fan-out landed. It is paid once for all thirty.
//
// ---- 1. WHAT THE TRAP COST A PLAYER ---------------------------------------
//
// Reported from real play on 0.2.8.108, twice on the same day. In Wanted!
// (scene 366) the score collapse after a fifth win asks the framework to change
// state and the game froze there, with the frame loop and the music still
// running. In Coincentration (scene 378) a tap did the same thing. Both left
// one line in the playlog:
//
//   [scene] mwcc POINTER-TO-MEMBER WALL: func_ov004_020b87e0(idx=1) is the
//   state-setter for dScMgBase_c and MSVC cannot compile its TU ... No state
//   was set and nothing was dispatched.
//
// A trap that sets no state IS the freeze. The framework's message object keeps
// its old fields, its index stays -1, its per-frame tick is never installed, and
// nothing in the class can advance, so the scene runs forever on the frame that
// asked.
//
// ---- 2. WHAT THE ROM DOES, DISASSEMBLED ------------------------------------
//
// Read out of extracted/overlays/overlay_0004.bin at base 0x020ad660. The delink
// block is `.text start:0x020b87e0 end:0x020b8a70`, 0x290 bytes, of which the
// first 0x1c0 are the ONE-TIME table build and the last 0x74 are the assign and
// the dispatch. A twenty-two word literal pool follows at 0x020b8a14.
//
// The table build, gated on a .bss guard word at 0x020bfd08 - 4:
//
//     020b87e8  ldr   r2, [pc, #0x224]     ; r2 = 0x020bfd04, the guard
//     020b87ec  ldr   r3, [r2]
//     020b87f0  ands  r4, r3, #1
//     020b87f4  bne   0x020b89a0           ; built already, skip to the assign
//     020b87fc  orr   r3, r3, #1
//     020b8800  str   r3, [r2]
//     ...       forty ldr/str pairs, copying twenty EIGHT-BYTE records out of
//               twenty named .data globals into the 0xa0 static at 0x020bfd08
//
// The assign and the dispatch:
//
//     020b89a0  str   r1, [r0, #0x18]        ; self->index = idx
//     020b89a4  ldr   r4, [r0, #0x18]        ; and it is READ BACK
//     020b89b0  add   r2, r3, r4, lsl #3     ; &table[index], STRIDE EIGHT
//     020b89b4  ldr   r3, [r3, r4, lsl #3]   ;   code
//     020b89b8  ldr   r2, [r2, #4]           ;   adjustment
//     020b89bc  str   r3, [r0]               ; self->pmf0.code = code
//     020b89c0  str   r2, [r0, #4]           ; self->pmf0.adj  = adj
//     020b89c4  ldr   r2, [r1]               ; data_02086b58, the NULL pair
//     020b89c8  ldr   r1, [r1, #4]
//     020b89cc  str   r2, [r0, #0x10]        ; self->pmf2 = the null pair
//     020b89d0  str   r1, [r0, #0x14]
//     020b89d4  ldr   r2, [r0]
//     020b89d8  cmp   r2, #0                 ; the ROM's own null-CODE guard
//     020b89e4  bxeq  lr                     ;   and it tests the code word only
//     020b89e8  ldr   r1, [r0, #4]           ; adjustment
//     020b89ec  add   r3, r0, r1, asr #1     ; this += adj >> 1  (arithmetic)
//     020b89f0  ands  r1, r1, #1             ; virtual bit, the adjustment's LSB
//     020b89f4  ldrne r0, [r3]               ;   virtual: r0 = the vtable
//     020b89f8  ldrne r1, [r0, r2]           ;            fn = vtable[code]
//     020b89fc  ldreq r1, [r0]               ;   direct:  fn = code
//     020b8a00  mov   r0, r3                 ; `this`, adjusted
//     020b8a04  blx   r1
//
// So the dispatch is a ZERO-ARGUMENT call with `this` in r0, which is exactly
// what port_mg_call0 in unmatched/MgBase_StateDispatch.cpp implements, and this
// file routes through that one entry point rather than growing a second opinion
// about what an adjustment word means.
//
// ---- 3. WHY MSVC CANNOT COMPILE THE SRC TU --------------------------------
//
// Three failures, any one fatal, and section 4 of mg_fanout_costs has them:
//
//   the SIZE. mwcc's pointer-to-member is eight bytes, {code, adjustment}.
//   MSVC's single-inheritance one is four. The TU's own `struct C` puts
//   field_8 at +8 after a leading PMF, which is only true at eight bytes, so
//   the object is laid out wrong before any dispatch happens.
//   the NAME. MSVC encodes the member-pointer TYPE into the symbol, so the
//   twenty globals come out as ?data_ov004_020bc97c@@3P8C@@AEXXZQ1@ and the
//   mount's C symbol _data_ov004_020bc97c can never satisfy them. Twenty-one
//   of the first minigame link's ninety-five unresolved externals were this
//   one TU.
//   the DISPATCH. The five-instruction ARM Itanium sequence above has no MSVC
//   equivalent.
//
// The remedy is the one the port has now used four times: the src body verbatim
// in structure, the member-pointer table re-typed as {code, adjustment} int
// pairs, and the call site replaced by an address switch.
//
// ---- 4. THE TWENTY, RESOLVED OUT OF THE ROM -------------------------------
//
// Section 4 warns that an address sweep over a pair range catches non-pairs, so
// these were NOT swept. The twenty global names come from the src TU's own table
// initialiser, in its order, and each name's code word was read two ways that
// agree: the eight bytes at (addr - 0x020ad660) in overlay_0004.bin, and the
// overlay's own load relocation row. EVERY ADJUSTMENT WORD READS ZERO, so every
// dispatch this seat can reach is the DIRECT case.
//
//   slot  .data global            code word   config/arm9/overlays/ov004/relocs
//    0    data_ov004_020bc974     020b8688    line 2091
//    1    data_ov004_020bc96c     020b853c    line 2090
//    2    data_ov004_020bc964     020b83ac    line 2089
//    3    data_ov004_020bc95c     020b81f8    line 2088
//    4    data_ov004_020bc954     020b7f5c    line 2087
//    5    data_ov004_020bc94c     020b7e38    line 2086
//    6    data_ov004_020bc944     020b7b90    line 2085
//    7    data_ov004_020bc92c     020b79b0    line 2082
//    8    data_ov004_020bc934     020b798c    line 2083
//    9    data_ov004_020bc97c     020b7854    line 2092
//   10    data_ov004_020bc984     020b7744    line 2093
//   11    data_ov004_020bc99c     020b7594    line 2096
//   12    data_ov004_020bc9a4     020b7460    line 2097   THE VENEER, section 6
//   13    data_ov004_020bca3c     020b743c    line 2116
//   14    data_ov004_020bc9bc     020b724c    line 2100
//   15    data_ov004_020bc9d4     020b70b4    line 2103
//   16    data_ov004_020bc9dc     020b7020    line 2104
//   17    data_ov004_020bc9ec     020b6f14    line 2106
//   18    data_ov004_020bc9f4     020b6d6c    line 2107
//   19    data_ov004_020bca0c     020b6c10    line 2110
//
// ALL TWENTY HAVE A MATCHED src TU, A DELINK BLOCK AND A LINE ALREADY IN
// port/slice_mg1.txt. Nothing here is a decomp gap and no case in the switch
// below is a floor. The seat is wiring, which is the good news the fan-out was
// promised and did not get from any earlier lane.
//
// data_02086b58 is arm9 .data, eight bytes by span (the next config symbol is
// .p__sinit_02073a24 at 0x02086b60), and it reads {0, 0} in extracted/arm9_dec
// .bin with no relocation on either word. It is the NULL member pointer, and
// the port already hosts it that way: build/port/host-src/romdata.c defines
// `data_02086b58[8] = { 0,0,0,0,0,0,0,0 }`. So `self->pmf2 = data_02086b58` is
// a CLEAR, not a call, and the ROM's own reader (func_ov004_020b8714) skips a
// zero code word.
//
// ---- 5. THE OBJECT, AND THE HALF OF THE SEAT THE COST FILE DOES NOT NAME ---
//
// The setter's `self` is the framework's MESSAGE object, at scene + 0xcc. Its
// only caller is src/func_ov004_020b0a54.c:
//
//     func_ov004_020b87e0((char *)data_ov004_020beb68 + 0xcc, c);
//
// and the layout the disassembly above forces, offset by offset, is
//
//     +0x00  pmf0    the state, ASSIGNED AND DISPATCHED ONCE by this function
//     +0x08  pmf1    the per-frame tick, installed by the state body
//     +0x10  pmf2    a second per-frame slot, cleared here, sometimes installed
//     +0x18  index   the message id, and -1 means "no message"
//     +0x1c  timer   counted down by ApproachLinear
//
// THE SEAT IS NOT DONE AT TWENTY, AND THIS IS THE FINDING THAT MATTERS FOR
// ANYONE COSTING THE REST. +0x18 is the gate on the framework's two per-frame
// self-field dispatchers, both already host-copied in
// unmatched/MgBase_StateDispatch.cpp:
//
//     func_ov004_020b8714   if (*(int *)(c + 0x18) == -1) return;   pmf at +0x10
//     func_ov004_020b8778   if (*(int *)(c + 0x18) == -1) return;   pmf at +0x08
//
// While the setter was a trap, +0x18 was never written, so both returned on
// their first line on every frame of every minigame and NEITHER pmf slot was
// ever dispatched. The measurement says so: a 2400 frame scene 378 run on the
// trap build reports 576 framework dispatch calls and ZERO unhandled addresses,
// which is only possible because none of the tick addresses was ever reached.
//
// Seating the setter un-gates both. The twenty state bodies install twenty-three
// pair globals between them, which resolve to TWENTY DISTINCT further code
// addresses, and every one of those would print an UNHANDLED report on the first
// frame after a message opened. Worse, a state that enters and never ticks is
// still a wedge: the tick bodies are what read the timer, poll the A button and
// write +0x18 back to -1 to CLOSE the message. So the second twenty is not scope
// creep, it is the other half of the same seat, and the switch below carries
// both groups.
//
// The second twenty were resolved the same two ways as the first, out of the
// image and out of the relocation rows, and every adjustment word is zero:
//
//   installed by  pair global            code word   slot it lands in
//    020b853c     data_ov004_020bc8bc    020b841c    +0x08
//    020b83ac     data_ov004_020bc8c4    020b8284    +0x08
//    020b7594     data_ov004_020bc8cc    020b6ad8    +0x10
//    020b81f8     data_ov004_020bc8d4    020b8098    +0x08
//    020b7020     data_ov004_020bc8e4    020b6f88    +0x08
//    020b7f5c     data_ov004_020bc8ec    020b7eac    +0x08
//    020b7e38     data_ov004_020bc8f4    020b7cd0    +0x08
//    020b743c     data_ov004_020bc8fc    020b72d4    +0x08
//    020b7b90     data_ov004_020bc90c    020b7b20    +0x08
//    020b7020     data_ov004_020bc91c    020b68e8    +0x10
//    020b6d6c     data_ov004_020bc924    020b6c9c    +0x08
//    020b724c     data_ov004_020bc994    020b7124    +0x08
//    020b79b0     data_ov004_020bc9ac    020b78f4    +0x08
//    020b724c     data_ov004_020bc9c4    020b6948    +0x10
//    020b6f14     data_ov004_020bc9cc    020b6ddc    +0x08
//    020b7854     data_ov004_020bc9e4    020b77b4    +0x08
//    020b7744     data_ov004_020bc9fc    020b75e4    +0x08
//    020b70b4     data_ov004_020bca04    020b7124    +0x08   (shared with bc994)
//    020b8688     data_ov004_020bca1c    020b8560    +0x08
//    020b7594     data_ov004_020bca24    020b746c    +0x08
//    020b6f14     data_ov004_020bca2c    020b68e8    +0x10   (shared with bc91c)
//    020b798c     data_ov004_020bc9b4    020b78f4    +0x08   (shared with bc9ac)
//    020b6c10     data_ov004_020bca34    020b6b40    +0x08
//
// Twenty-three pairs, twenty distinct addresses, and all twenty have a matched
// src TU, a delink block and a line already in port/slice_mg1.txt. Not one of
// the forty cases below is a floor.
//
// ---- 6. THE ONE DEFECT THE SEAT UNCOVERED, AND IT WOULD HAVE CRASHED -------
//
// SLOT 12 IS A TAIL-JUMP VENEER AND THE src TU DROPS ITS POINTER. 0x020b7460 is
// three words in the ROM:
//
//     020b7460  ldr   ip, [pc]
//     020b7464  bx    ip
//     020b7468  .word 0x020b743c
//
// which preserves every register, so r0 arrives at 0x020b743c untouched. Its
// target writes through that pointer on its first instruction pair
// (`mov r2,#0xa; str r2,[r0,#0x1c]`). src/func_ov004_020b7460.c spells the
// veneer as
//
//     extern void func_ov004_020b743c(void);
//     void func_ov004_020b7460(void) { func_ov004_020b743c(); }
//
// with no parameter on either side, which is correct on ARM and is a dropped
// pointer on the host: MSVC's __cdecl pushes nothing and 0x020b743c reads
// whatever is on the stack, then stores 10 through it. That is a wild write,
// not a wrong value.
//
// IT IS REACHED THREE WAYS AND ALL THREE ARE OPENED BY THIS SEAT. Slot 12 of the
// table above is one. The other two are src/func_ov004_020b6ddc.c:39 and
// src/func_ov004_020b6f88.c:24, which are tick bodies of states 17 and 16, so
// they only ever run once +0x18 is written. BOTH CALLERS ALREADY PASS THE
// POINTER: each declares `extern void func_ov004_020b7460(void *c);` locally.
// Only the definition drops it, which is why nothing in the tree diagnoses it
// and why the byte gate is right to be silent (the ROM bytes and the src agree).
//
// So src/func_ov004_020b7460.c is EXCLUDED from port/slice_mg1.txt and the
// veneer is defined here instead, with the parameter its two callers already
// pass. That is the whole change: one parameter added, forwarded to the address
// the ROM's own literal names. Nothing else about the body differs, because
// there is no other body.
//
// ---- 7. WHAT IS NOT HERE ---------------------------------------------------
//
// The twenty state bodies and the twenty tick bodies are matched src TUs and are
// NOT copied here. They keep their slice lines and their credit; this file only
// gives them a caller. port/tools/stategen.py cannot generate this switch and
// says so in its own docstring: it reads a constructor filling a .bss table out
// of a .data pair block, and this TU has neither, so the seat is hand work with
// the ROM open. Its --reconstruct still covers the two artifacts it does own.
//
// ---- 8. THE UNIVERSE, CLOSED, AND THE DEFECT THAT CLOSING IT FOUND --------
//
// READ THIS BEFORE ROUTING ANOTHER ov004 ADDRESS, and read the second half of
// it before touching a vtable thunk in hal/scene_mg.cpp.
//
// THE UNIVERSE IS BOUNDED AND IT IS EIGHTY. Every ov004 src TU was read, every
// `*(...) = data_ov004_XXXX;` install of a .data word pair into an object slot
// was collected, and each pair was kept only when its FIRST word carries a
// `load` relocation in config/arm9/overlays/ov004/relocs.txt to an address
// inside the overlay. That is section 4's discipline rather than a byte sweep:
// a consumer names the global and the ROM's own relocation says it is an
// address. The answer is SIXTY distinct code words, every adjustment zero,
// every one with a matched src TU and a delink block, plus the setter's own
// twenty out of its in-function table, which no install pattern can see.
// runs/mg5/out/baseset/pmf_universe.txt is the listing.
//
// ALL EIGHTY ARE ROUTED. Fifty-six here (group A's twenty, group B's twenty,
// group C's sixteen) and twenty-four in unmatched/MgBase_StateDispatch.cpp,
// which are __sinit_ov004_020b955c's three tables: lane FWK routed two of them
// and left twenty-two with the note that each would report the day a scene
// reached it, and seating the setter reached seven of the twenty-two on the
// first run. A scene 378 sweep that reported 700 UNHANDLED dispatches with the
// set half-routed reports ZERO with it closed.
//
// WHY IT IS THE WHOLE SET AND NOT THE ONES A RUN WITNESSED. FWK's rule was that
// routing an address no run touches buys a slice line and no evidence, and that
// rule is right for a heterogeneous set. This one is not heterogeneous: it is
// one overlay's own state machine, every pair resolved from the consumer that
// names it, every adjustment word zero, every body matched and delinked, and
// every dispatch site zero-argument with `this` in r0. The evidence is
// per-family and the runs are its witness. The cost of the other rule is a dead
// end a PLAYER meets, which is the defect this lane exists to remove.
//
// ---- THE DEFECT CLOSING IT FOUND, AND IT WAS A CRASH ----------------------
//
// The first build with the closure routed FAULTED, on scene 378 at frame 2103
// and on a scene 366 drive at frame 300, both times with a wild execute at an
// address that MOVED WITH THE BUILD. It is a stack imbalance in the port's own
// vtable thunks and nothing to do with the sixteen bodies:
//
//   src/func_ov004_020b29a0.cpp is `void func_ov004_020b29a0(Base *c, void *arg)
//   { c->v18(arg); }` and the ROM agrees word for word at 0x020b29a0
//   (`ldr r2,[r0]; ldr r2,[r2,#0x48]; blx r2`). MSVC compiles that as a
//   __thiscall call WITH ONE PUSHED ARGUMENT, so the CALLEE owes four bytes:
//       0004cb53  mov  ecx, [ebp+8]        ; this
//       0004cb56  push [ebp+0xc]           ; arg
//       0004cb59  mov  eax, [ecx]
//       0004cb5b  call [eax+0x48]          ; vtable slot 18
//
//   hal/scene_mg.cpp's slot 18 thunks were declared with NO stack parameter,
//   `static int __fastcall mc_reset(void *s, void *)`, so both parameters sat
//   in ecx and edx and MSVC emitted a bare `ret`, popping nothing.
//
// FOUR BYTES LEAKED ON EVERY SLOT-18 DISPATCH, func_ov004_020b29a0 ran
// `pop ebp; ret` on a shifted stack, and a few frames later a `ret` took a
// garbage return address. Slot 19 had the same shape through mb_v19.
//
// THE FIX IS IN hal/scene_mg.cpp AND IT IS EVIDENCED, NOT GUESSED. Both slots
// were scanned out of the two overlay images word by word;
// runs/mg5/out/baseset/slot18_19_scan.txt is the listing and it REPRODUCES the
// twenty-two site slot-18 census that file's dScMgLuigi_c block already
// recorded. Offset 0x48: 22 sites, 21 set r1 first, and the one that does not
// is func_ov004_020b29a0, whose own src forwards its second parameter so the
// host pushes one there too. Offset 0x4c: 14 sites, all fourteen set r1, every
// one of them `ldr r1,[rX,#0x18]`. So the argument count is ONE at every site
// of both slots and there is no caller for which a popping thunk is wrong.
// Four slot-18 thunks and one slot-19 thunk gained the ride-through parameter;
// mgl_reset already had it, and its block had already named the other two as
// somebody else's to look at.
//
// WHAT IT CHANGED IN THE 378 SWEEP, AND IT IS NOT A REGRESSION. With the
// closure routed the class finally answers its own message, so slot 18,
// dScMgCoin_c::StateReset, is DISPATCHED FOR THE FIRST TIME IN THIS PORT'S
// HISTORY (once), and the round state index reads 1 where it read 5. Behavior
// and Render are 2399 and 2400 either way, so the scene is alive and drawing
// the whole run; what moved is that a round reset now happens instead of not
// happening.

#include <cstdio>

/* The eight-byte mwcc member pointer, in the only spelling that is true on both
   machines: two words, no member-pointer type anywhere. The same struct
   unmatched/MgBase_StateDispatch.cpp declares, redeclared rather than shared so
   neither file gains a header dependency on the other. */
namespace { struct MgPmf { unsigned code; int adj; }; }

extern "C" {

/* ---- the mount's twenty pair globals, re-typed -------------------------- */
/* build/port/host-src/ov004_syms.c defines each as `u8 data_ov004_XXXX[8]` at
   its ROM offset, so these are the mount's own bytes at C linkage. Declared one
   by one rather than as an array because the ROM names them one by one and they
   are NOT contiguous in slot order (slot 13 is 0x020bca3c, above slot 19). */
extern MgPmf data_ov004_020bc974;
extern MgPmf data_ov004_020bc96c;
extern MgPmf data_ov004_020bc964;
extern MgPmf data_ov004_020bc95c;
extern MgPmf data_ov004_020bc954;
extern MgPmf data_ov004_020bc94c;
extern MgPmf data_ov004_020bc944;
extern MgPmf data_ov004_020bc92c;
extern MgPmf data_ov004_020bc934;
extern MgPmf data_ov004_020bc97c;
extern MgPmf data_ov004_020bc984;
extern MgPmf data_ov004_020bc99c;
extern MgPmf data_ov004_020bc9a4;
extern MgPmf data_ov004_020bca3c;
extern MgPmf data_ov004_020bc9bc;
extern MgPmf data_ov004_020bc9d4;
extern MgPmf data_ov004_020bc9dc;
extern MgPmf data_ov004_020bc9ec;
extern MgPmf data_ov004_020bc9f4;
extern MgPmf data_ov004_020bca0c;

/* arm9 .data, the null pair. build/port/host-src/romdata.c defines it. */
extern MgPmf data_02086b58;

/* ---- the twenty state bodies the table holds --------------------------- */
/* Signatures are their src TUs', unchanged. Every one of them is reached ONLY
   through the switch below, so before this file none of them had a caller in
   the build and /OPT:REF had dropped all twenty despite the slice lines. */
void func_ov004_020b8688(char *c);
void func_ov004_020b853c(char *c);
void func_ov004_020b83ac(char *c);
void func_ov004_020b81f8(char *c);
void func_ov004_020b7f5c(char *c);
void func_ov004_020b7e38(char *c);
void func_ov004_020b7b90(char *c);
void func_ov004_020b79b0(char *c);
void func_ov004_020b798c(char *c);
void func_ov004_020b7854(char *c);
void func_ov004_020b7744(char *c);
void func_ov004_020b7594(char *c);
void func_ov004_020b743c(char *c);
void func_ov004_020b724c(char *c);
void func_ov004_020b70b4(char *c);
void func_ov004_020b7020(char *c);
void func_ov004_020b6f14(char *c);
void func_ov004_020b6d6c(char *c);
void func_ov004_020b6c10(char *c);

/* ---- the twenty per-frame tick bodies the state bodies install ---------- */
/* func_ov004_020b6948 and func_ov004_020b6ad8 REALLY DO TAKE NO ARGUMENT: both
   src bodies read only globals and the ROM slots that hold them are the same
   zero-argument slots as the rest, so calling them (void) is exact rather than
   a dropped pointer. This is the benign ARM ride-through
   unmatched/MgCurling_StateDispatch.cpp documents for func_ov006_020e2eb8, not
   the scene_actor_faces defect class: there is no callee behind them that
   wanted a pointer. Checked body by body against the disassembly, not assumed
   from the declaration. */
void func_ov004_020b68e8(int *c);
void func_ov004_020b6948(void);
void func_ov004_020b6ad8(void);
void func_ov004_020b6b40(void *c);
void func_ov004_020b6c9c(void *c);
void func_ov004_020b6ddc(char *c);
void func_ov004_020b6f88(char *c);
void func_ov004_020b7124(char *c);
void func_ov004_020b72d4(char *c);
void func_ov004_020b746c(char *c);
void func_ov004_020b75e4(char *c);
void func_ov004_020b77b4(char *c);
void func_ov004_020b78f4(char *c);
void func_ov004_020b7b20(char *c);
void func_ov004_020b7cd0(char *c);
void func_ov004_020b7eac(char *c);
void func_ov004_020b8098(char *c);
void func_ov004_020b8284(char *c);
void func_ov004_020b841c(char *c);
void func_ov004_020b8560(char *c);

/* ---- group C: the rest of the ov004 member-pointer universe -------------- */
/* THE CLOSURE, AND HOW IT WAS BOUNDED RATHER THAN GUESSED AT. Routing the forty
   above turned the framework on, and the framework installs further states out
   of its own bodies, so the first runs after the seat each ended with a shorter
   list of UNHANDLED addresses than the one before. Chasing that one wave at a
   time is the shape that never converges, so the set was closed instead.
   Section 8 of the header is the derivation, the bound and the artifact. These
   sixteen are the remainder of the sixty. Each takes one pointer, its src TU's
   own spelling, and none needed a host copy. */
void func_ov004_020b369c(char *c);
void func_ov004_020b37c4(char *c);
void func_ov004_020b38ac(char *c);
void func_ov004_020b3978(char *c);
void func_ov004_020b410c(char *c);
void func_ov004_020b4214(char *c);
void func_ov004_020b4aa4(char *c);
void func_ov004_020b4c30(char *c);
void func_ov004_020b4d50(char *c);
void func_ov004_020b4dfc(char *c);
void func_ov004_020b4f44(char *c);
void func_ov004_020b4ff0(char *c);
void func_ov004_020b5288(char *c);
void func_ov004_020b7a18(char *c);
void func_ov004_020b7c04(char *c);
void func_ov004_020b7fec(char *c);

/* the ONE entry point for a zero-argument state call, and the one place that
   decides what an adjustment word means. unmatched/MgBase_StateDispatch.cpp. */
void port_mg_call0(void *self, unsigned code, int adj);

}  /* extern "C" */

// ---- section 6's veneer, with the pointer its callers already pass ---------

/* HOST COPY of src/func_ov004_020b7460.c, which is off port/slice_mg1.txt for
   this file. The ROM is `ldr ip,[pc]; bx ip; .word 0x020b743c`, a tail jump that
   preserves r0. One parameter added, forwarded. */
/* PORT_HOST_ABI: ARM r0 ride-through; src spells the tail-jump veneer (void) and drops the object pointer its callers pass in r0 */
extern "C" void func_ov004_020b7460(void *c)
{
    func_ov004_020b743c((char *)c);
}

// ---- the address switch ----------------------------------------------------

static unsigned g_setter_calls;
static unsigned g_setter_dispatched;
/* Which message indices a run actually asked for, and how often. The census
   prints it because "the setter ran" and "the setter ran the state the game
   asked for" are different claims and the old line could only make the first. */
static unsigned g_setter_idx[20];
static unsigned g_base_state_hits;
static unsigned g_base_tick_hits;
static unsigned g_base_closure_hits;

/* One line per distinct out-of-range index, for the reason mg_unhandled gives in
   MgBase_StateDispatch: a per-frame loop must not be able to flood the log and a
   single occurrence must not be able to hide in one. */
static void setter_bad_index(int idx)
{
    static int said[8];
    static int nsaid;
    for (int i = 0; i < nsaid; ++i)
        if (said[i] == idx)
            return;
    if (nsaid < 8)
        said[nsaid++] = idx;
    std::fprintf(stderr, "  [scene] func_ov004_020b87e0: message index %d is "
                 "outside the ROM's twenty-entry table. The ROM has no bounds "
                 "check and would read the .bss word after the table; the host "
                 "refuses rather than reading past a 20 entry array. No state "
                 "was set. port/unmatched/MgBase_StateSetter.cpp\n", idx);
    std::fflush(stderr);
}

/* The twenty the setter's own table holds, plus the twenty the state bodies
   install as per-frame ticks. Two groups, one switch, because the two groups are
   dispatched through the same port_mg_call0 and an address is an address. */
extern "C" int port_mg_try_base_state(void *self, unsigned code)
{
    switch (code) {
    /* ---- group A: the setter's twenty, slot order ---- */
    case 0x020b8688u: ++g_base_state_hits; func_ov004_020b8688((char *)self); return 1;
    case 0x020b853cu: ++g_base_state_hits; func_ov004_020b853c((char *)self); return 1;
    case 0x020b83acu: ++g_base_state_hits; func_ov004_020b83ac((char *)self); return 1;
    case 0x020b81f8u: ++g_base_state_hits; func_ov004_020b81f8((char *)self); return 1;
    case 0x020b7f5cu: ++g_base_state_hits; func_ov004_020b7f5c((char *)self); return 1;
    case 0x020b7e38u: ++g_base_state_hits; func_ov004_020b7e38((char *)self); return 1;
    case 0x020b7b90u: ++g_base_state_hits; func_ov004_020b7b90((char *)self); return 1;
    case 0x020b79b0u: ++g_base_state_hits; func_ov004_020b79b0((char *)self); return 1;
    case 0x020b798cu: ++g_base_state_hits; func_ov004_020b798c((char *)self); return 1;
    case 0x020b7854u: ++g_base_state_hits; func_ov004_020b7854((char *)self); return 1;
    case 0x020b7744u: ++g_base_state_hits; func_ov004_020b7744((char *)self); return 1;
    case 0x020b7594u: ++g_base_state_hits; func_ov004_020b7594((char *)self); return 1;
    /* slot 12, the veneer. Routed to the host copy above, which is the only
       definition of the symbol in the build. */
    case 0x020b7460u: ++g_base_state_hits; func_ov004_020b7460(self);         return 1;
    case 0x020b743cu: ++g_base_state_hits; func_ov004_020b743c((char *)self); return 1;
    case 0x020b724cu: ++g_base_state_hits; func_ov004_020b724c((char *)self); return 1;
    case 0x020b70b4u: ++g_base_state_hits; func_ov004_020b70b4((char *)self); return 1;
    case 0x020b7020u: ++g_base_state_hits; func_ov004_020b7020((char *)self); return 1;
    case 0x020b6f14u: ++g_base_state_hits; func_ov004_020b6f14((char *)self); return 1;
    case 0x020b6d6cu: ++g_base_state_hits; func_ov004_020b6d6c((char *)self); return 1;
    case 0x020b6c10u: ++g_base_state_hits; func_ov004_020b6c10((char *)self); return 1;

    /* ---- group B: the twenty per-frame ticks, address order ---- */
    case 0x020b68e8u: ++g_base_tick_hits; func_ov004_020b68e8((int *)self);  return 1;
    case 0x020b6948u: ++g_base_tick_hits; func_ov004_020b6948();             return 1;
    case 0x020b6ad8u: ++g_base_tick_hits; func_ov004_020b6ad8();             return 1;
    case 0x020b6b40u: ++g_base_tick_hits; func_ov004_020b6b40(self);         return 1;
    case 0x020b6c9cu: ++g_base_tick_hits; func_ov004_020b6c9c(self);         return 1;
    case 0x020b6ddcu: ++g_base_tick_hits; func_ov004_020b6ddc((char *)self); return 1;
    case 0x020b6f88u: ++g_base_tick_hits; func_ov004_020b6f88((char *)self); return 1;
    case 0x020b7124u: ++g_base_tick_hits; func_ov004_020b7124((char *)self); return 1;
    case 0x020b72d4u: ++g_base_tick_hits; func_ov004_020b72d4((char *)self); return 1;
    case 0x020b746cu: ++g_base_tick_hits; func_ov004_020b746c((char *)self); return 1;
    case 0x020b75e4u: ++g_base_tick_hits; func_ov004_020b75e4((char *)self); return 1;
    case 0x020b77b4u: ++g_base_tick_hits; func_ov004_020b77b4((char *)self); return 1;
    case 0x020b78f4u: ++g_base_tick_hits; func_ov004_020b78f4((char *)self); return 1;
    case 0x020b7b20u: ++g_base_tick_hits; func_ov004_020b7b20((char *)self); return 1;
    case 0x020b7cd0u: ++g_base_tick_hits; func_ov004_020b7cd0((char *)self); return 1;
    case 0x020b7eacu: ++g_base_tick_hits; func_ov004_020b7eac((char *)self); return 1;
    case 0x020b8098u: ++g_base_tick_hits; func_ov004_020b8098((char *)self); return 1;
    case 0x020b8284u: ++g_base_tick_hits; func_ov004_020b8284((char *)self); return 1;
    case 0x020b841cu: ++g_base_tick_hits; func_ov004_020b841c((char *)self); return 1;
    case 0x020b8560u: ++g_base_tick_hits; func_ov004_020b8560((char *)self); return 1;


    /* ---- group C: the closure, address order. Section 8 of the header is the
       derivation and the bound. ---- */
    case 0x020b369cu: ++g_base_closure_hits; func_ov004_020b369c((char *)self); return 1;
    case 0x020b37c4u: ++g_base_closure_hits; func_ov004_020b37c4((char *)self); return 1;
    case 0x020b38acu: ++g_base_closure_hits; func_ov004_020b38ac((char *)self); return 1;
    case 0x020b3978u: ++g_base_closure_hits; func_ov004_020b3978((char *)self); return 1;
    case 0x020b410cu: ++g_base_closure_hits; func_ov004_020b410c((char *)self); return 1;
    case 0x020b4214u: ++g_base_closure_hits; func_ov004_020b4214((char *)self); return 1;
    case 0x020b4aa4u: ++g_base_closure_hits; func_ov004_020b4aa4((char *)self); return 1;
    case 0x020b4c30u: ++g_base_closure_hits; func_ov004_020b4c30((char *)self); return 1;
    case 0x020b4d50u: ++g_base_closure_hits; func_ov004_020b4d50((char *)self); return 1;
    case 0x020b4dfcu: ++g_base_closure_hits; func_ov004_020b4dfc((char *)self); return 1;
    case 0x020b4f44u: ++g_base_closure_hits; func_ov004_020b4f44((char *)self); return 1;
    case 0x020b4ff0u: ++g_base_closure_hits; func_ov004_020b4ff0((char *)self); return 1;
    case 0x020b5288u: ++g_base_closure_hits; func_ov004_020b5288((char *)self); return 1;
    case 0x020b7a18u: ++g_base_closure_hits; func_ov004_020b7a18((char *)self); return 1;
    case 0x020b7c04u: ++g_base_closure_hits; func_ov004_020b7c04((char *)self); return 1;
    case 0x020b7fecu: ++g_base_closure_hits; func_ov004_020b7fec((char *)self); return 1;

    default: return 0;
    }
}

// ---- the host copy ---------------------------------------------------------
//
// src/func_ov004_020b87e0.cpp, statement for statement, with the twenty-entry
// PMF table re-typed and the one dispatch replaced. Everything else, including
// the read-back of self->index and the null-CODE guard, is the src's and the
// ROM's.

/* The object the ROM's own offsets force. See section 5. */
namespace {
struct SetterObj {
    MgPmf pmf0;     /* +0x00 */
    int   field_8;  /* +0x08, the low word of the tick pmf */
    int   field_c;  /* +0x0c, its adjustment */
    MgPmf pmf2;     /* +0x10 */
    int   index;    /* +0x18 */
};
}  /* namespace */

/* The ROM's function-local static, in the ROM's own shape: one guard word and a
   0xa0 table, built on the first call and never again. The twenty sources are
   mount .data and are constant for the life of the process, so the guard is
   faithful rather than an optimisation. */
static MgPmf g_table[20];
static int   g_table_built;

static void mgbase_build_table(void)
{
    if (g_table_built)
        return;
    g_table_built = 1;
    g_table[0]  = data_ov004_020bc974;
    g_table[1]  = data_ov004_020bc96c;
    g_table[2]  = data_ov004_020bc964;
    g_table[3]  = data_ov004_020bc95c;
    g_table[4]  = data_ov004_020bc954;
    g_table[5]  = data_ov004_020bc94c;
    g_table[6]  = data_ov004_020bc944;
    g_table[7]  = data_ov004_020bc92c;
    g_table[8]  = data_ov004_020bc934;
    g_table[9]  = data_ov004_020bc97c;
    g_table[10] = data_ov004_020bc984;
    g_table[11] = data_ov004_020bc99c;
    g_table[12] = data_ov004_020bc9a4;
    g_table[13] = data_ov004_020bca3c;
    g_table[14] = data_ov004_020bc9bc;
    g_table[15] = data_ov004_020bc9d4;
    g_table[16] = data_ov004_020bc9dc;
    g_table[17] = data_ov004_020bc9ec;
    g_table[18] = data_ov004_020bc9f4;
    g_table[19] = data_ov004_020bca0c;
}

/* PORT_HOST_ABI: mwcc pointer-to-member state setter (dScMgBase_c); builds and indexes an 8-byte {code,adj} table and dispatches through it, host-copied as an address switch */
extern "C" void func_ov004_020b87e0(void *cv, int idx)
{
    SetterObj *self = (SetterObj *)cv;

    ++g_setter_calls;
    mgbase_build_table();

    self->index = idx;

    /* THE ONE ADDED GUARD, and it is a host guard rather than the ROM's. The ROM
       indexes the table with no bounds check and would read the .bss word past
       0x020bfda8; every one of the eleven distinct indices the game's own call
       sites pass (0, 1, 3, 4, 5, 7, 8, 0xc, 0x10, 0x12, 0x13) is inside the
       twenty, so this arm has no reachable caller. It reports rather than
       clamping, because a clamp would dispatch a state the caller did not ask
       for and that is a wrong call dressed as a recovery. */
    if (self->index < 0 || self->index >= 20) {
        setter_bad_index(self->index);
        self->pmf0.code = 0;
        self->pmf0.adj = 0;
        self->pmf2 = data_02086b58;
        return;
    }

    ++g_setter_idx[self->index];
    self->pmf0 = g_table[self->index];
    self->pmf2 = data_02086b58;

    /* the ROM's own null guard, and it tests the CODE word only */
    if (self->pmf0.code) {
        ++g_setter_dispatched;
        port_mg_call0(self, self->pmf0.code, self->pmf0.adj);
    }
}

/* The census, for hal/scene_mg.cpp. The framework dispatch counters in
   MgBase_StateDispatch.cpp already say how many calls went through the switch;
   these say how many of them this seat is responsible for, which is the number
   that distinguishes "the setter ran" from "the setter set a state and something
   dispatched". */
extern "C" void port_mg_base_setter_counts(unsigned *calls, unsigned *dispatched,
                                           unsigned *states, unsigned *ticks,
                                           unsigned *closure)
{
    if (calls)      *calls      = g_setter_calls;
    if (dispatched) *dispatched = g_setter_dispatched;
    if (states)     *states     = g_base_state_hits;
    if (ticks)      *ticks      = g_base_tick_hits;
    if (closure)    *closure    = g_base_closure_hits;
}

/* The per-index histogram, for the census line. Returns the number of entries
   written; the caller passes a twenty-slot array. */
extern "C" unsigned port_mg_base_setter_index_hits(unsigned *out, unsigned n)
{
    unsigned i;
    for (i = 0; i < n && i < 20; ++i)
        out[i] = g_setter_idx[i];
    return i;
}
