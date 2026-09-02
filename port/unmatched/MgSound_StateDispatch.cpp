// PORT_HOST_ABI. dScMgSound_c's pointer-to-member state machine, host-copied.
// Run mg9, lane BOX. actor id 0x16f = scene 367, the "Boom Box" minigame.
//
// Everything below was read out of extracted/overlays/overlay_0006.bin at base
// 0x020bfec0, never out of a dsd export. The alignment was checked first on a
// body this lane did not write and could not have tuned: func_ov006_0210a600
// disassembles to `mov r0,#1 / bx lr`, which is exactly what
// port/tools/inferred_stub_adjudicated.txt already records for it.
//
// ---- WHY THIS FILE HAS SIXTEEN HOST COPIES --------------------------------
//
// port/mg_fanout_costs.txt section 4 costs the wall at "five dispatching TUs
// to host-copy" per minigame. Section 14 raised that to eleven for
// dScMgPanel_c and said the question to ask is not "how many TUs name the
// table" but "how deep is the machine". This class answers SIXTEEN, and the
// depth is the whole of the reason:
//
//   level 1  data_ov006_02142df8   5 slots  arity 1  <- func_ov006_0211b954
//   level 1  data_ov006_02142e20  13 slots  arity 1  <- func_ov006_0211b5e0
//   level 2  each of 02142e20's THIRTEEN states is itself a 3-slot dispatcher
//            02142d38 02142d50 02142d80 02142dc8 02142de0 02142cd8 02142cf0
//            02142d20 02142d68 02142d98 02142cc0 02142d08 02142db0
//   plus     func_ov006_020c2b8c, the +0x4f38 sub-object's tick, which holds
//            its member pointer as an object FIELD rather than in a table
//
// 2 + 13 + 1 = 16, over FIFTEEN tables and FIFTY-SEVEN pairs.
//
// Both level-1 dispatchers are called DIRECTLY by vtable slot 6, which is a
// PLAIN SWITCH and not a dispatcher itself -- so the "read the slot-6 src
// before wiring it by name" rule (section 14) was obeyed and came back clean
// here, and the machine still had to be found one level down.
//
// ---- THE FIFTY-SEVEN PAIRS, AND HOW THE SLOT NUMBERS WERE GOT -------------
//
// From src/__sinit_ov006_02132970.c ONE ASSIGNMENT AT A TIME, which is the
// only ground truth for a slot number: an overlay constructor does NOT copy
// its pairs in address order, and this one interleaves fifteen destinations.
// A sweep of the source span would also have manufactured phantoms --
// MgBoomBox_SpawnInfo itself (0x0213f69c, two words) sits INSIDE the pair span
// 0x0213f4fc..0x0213f6dc, bracketed by pair 0x0213f694 and pair 0x0213f6a4.
//
// ALL FIFTY-SEVEN ADJUSTMENT WORDS READ ZERO and all fifty-seven code words
// are DISTINCT, and every one of them lands in 0x02119d50..0x0211bf44, inside
// this class's own code block (slot 16 at 0x02119904 to the factory at
// 0x0211cb70). That range check is what says these tables are this class's.
//
//   THE FIRST CONSTRUCTOR THIS LANE LANDED ON WAS THE WRONG CLASS'S, and it
//   is worth recording because the next lane will meet the same shape.
//   __sinit_ov006_02132f68 -- the block immediately after this one, and the
//   one a relocation scan over the data region ABOVE the vtable finds first --
//   builds four tables of 27 pairs whose code words all lie in
//   0x0211cd24..0x021200cc. That is dScMgTeresa_c's code (id 0x183, scene
//   387), not this class's. The pairs are physically parked between this
//   class's vtable and Teresa's type_info, so proximity says nothing; only the
//   code-word range does.
//
// ---- THE THREE PMF SHAPES ARE ALL PRESENT IN THIS ONE CLASS ---------------
//
// Section 4 names three ways a pointer-to-member dispatch can hide, and says
// the link finds one of them and a `::*` source sweep finds another and the
// third needs a run. This class has all three, which makes it the first seat
// that can state the trio from one closure:
//
//   1. OPEN-CODED IN PLAIN INTS -- invisible to BOTH prescribed detectors.
//      func_ov006_0211b954  table 02142df8
//      func_ov006_0211b5e0  table 02142e20
//      Both declare `struct Ent { int a; int b; }` inside extern "C" and do
//      their own `>> 1` / `& 1` decode. The ROM at 0x0211b984, one for one
//      with what the src spells:
//          ldr   r1,[r3,#4]        the adjustment
//          add   r0,r7,r1,asr #1   this, advanced, arithmetic shift
//          ands  r1,r1,#1          the virtual bit, in the LSB
//          ldrne r2,[r0] / ldrne r1,[r3] / ldrne r2,[r2,r1]
//          ldreq r2,[r3]
//          blx   r2
//      and `mov r1,r6` immediately before the blx is the loop counter, which
//      is what makes both tables arity ONE.
//
//   2. PMF-TYPED BEHIND extern "C" -- silent on the link, found by the sweep.
//      The twelve sub-dispatchers whose table extern sits inside the extern
//      "C" block, so the global mangles as the plain C name the ov006 mount
//      already defines and the linker is satisfied while MSVC strides an
//      eight-byte table by four.
//
//   3. PMF-TYPED AT C++ LINKAGE -- the only one a link can name.
//      src/func_ov006_0211a2c4.cpp declares `extern PMF data_ov006_02142cc0[];`
//      OUTSIDE its extern "C" block, so it is the class's single LNK2019.
//
// THE THIRTEEN SUB-DISPATCHERS ARE THE SAME NINETEEN INSTRUCTIONS THIRTEEN
// TIMES. Read word for word out of the image: 0x0211b590, 0x0211b398,
// 0x0211b17c, 0x0211af60, 0x0211ad44, 0x0211abdc, 0x0211aa44, 0x0211a7ac,
// 0x0211a648, 0x0211a4b0, 0x0211a2c4, 0x0211a0d8 and 0x02119eec are BYTE
// IDENTICAL over their first nineteen words and differ only in the twentieth,
// which is the literal-pool word holding their table address. So one shape is
// transcribed once below and applied thirteen times, and the check that this
// is legitimate is the byte comparison rather than a reading of the sources.
//
//     mla   r2,r1,#0x24,r0     the entity, stride 0x24
//     add   r2,r2,#0x5000
//     ldrb  r2,[r2,#0x1d1]     the SUB-state index, at entity +5
//     add   ip,r3,r2,lsl #3    an EIGHT-byte stride
//     ... the five-instruction Itanium sequence ...
//     blx   r2                 with r1 still holding the incoming index
//
// ---- ROUTE AT THE DISPATCH SITE, NEVER BY REWRITING THE STORED PAIR -------
//
// Section 4's rulebook, applied rather than restated. The tables' STRIDE is
// already right on both machines -- `struct MgPmf { unsigned code; int adj; }`
// is eight bytes here and eight bytes in the ROM -- so what is wrong is only
// that the code word is a DS address. The pairs keep the ROM's own words and
// the DECODE is what this file replaces. The ov085 / ov100 shape (write host
// addresses over the .data pairs before the constructor copies them) is wrong
// for the same reason section 4 gives for func_ov006_020c3d18, and for one
// more that is this class's own: the source pairs share their span with
// MgBoomBox_SpawnInfo, whose factory word the registry writes.
//
// ---- THE ONE HARD FLOOR IS RETIRED, run mg10 lane F367 --------------------
//
//   func_ov006_0211bc8c   slot 1 of data_ov006_02142df8, 0x2b8 bytes
//
// Run mg9 shipped this address as a REPORTING case with no symbol invented for
// it: config symbol, no delink block, no src file in either extension. It is
// now src/func_ov006_0211bc8c.cpp, a BYTE MATCH on mwccarm 2004/b56 with all
// fourteen relocation destinations checked against ov006's relocs.txt, and it
// has a delink block covering exactly 0x0211bc8c..0x0211bf44. The switch below
// routes it like the other fifty-five and the counter that used to say "asks
// for the floor" now says "entries into state 1".
//
// WHY IT MATTERED, kept because it is the reason the run law calls a
// trap-shaped floor worse than a missing one. An unattended 1200-frame boot
// asked for it ZERO times, which said only that it was not in the way of a
// BOOT. Driven -- SM64DS_TOUCH_PROBE="100-104:64:32,160-164:128:32" pokes two
// of the six pads, both records move from state 0 to state 1, the per-round tap
// counter at +0x5624 reaches its own early-out of 2 -- the SAME 300-frame run
// reported 338 asks where the untouched one reported none. Every one of those
// 338 was a pad tick that did nothing, so a tap changed the state byte and then
// the pad froze: no press animation, no note, no sound. That is the whole of
// what stood between this seat and a playable Boom Box.
//
// WHAT THE BODY DOES, so the next reader need not re-derive it. It runs the
// press animation on the per-step timer at +0x50f0 against the eight-entry
// frame table data_ov006_0212ef3c, stepping the sub-index at +0x50f7 from 0 to
// 6 and returning each time. On the step that would make it 7 it clamps back to
// 6 and COMMITS: the record moves to state 2 (func_ov006_0211bc68), the note
// index at +0x50f8 plus one is appended to the 16-bit play log at +0x5610 with
// the pad index packed into the high byte, the write cursor at +0x5625 advances,
// +0x5614 is seeded 0x20, func_ov006_0211b654 fills up to three of the THIRTY
// records with note entities -- which is what the second level-1 dispatcher then
// ticks and draws -- and one of three arm9 sound entry points fires on vtable
// slot 35 and the mode byte at +0x5627. The slot-35 call goes through a 36-slot
// shadow class, so the receiver arrives in r0; this class has already produced
// three receiver-drop defects and the byte match is what proves this is not a
// fourth. Finally, on the SECOND tap of a round (+0x5624 == 1) it calls
// func_ov006_020c2300 on the +0x4f38 sub-object.
//
// TWO SLICE LINES STOP BEING FREE. src/func_ov006_0211b654.c and
// src/func_ov006_020c2300.c were in port/slice_box.txt reachable only from
// 0x0211bd88 and 0x0211bf10, both inside this body, so /OPT:REF dropped them
// and run mg9 budgeted them as +0 on the headline. They now have a caller.
//
// ---- ONE STATE IS A JOIN MISS AND IS NOT A FLOOR --------------------------
//
//   func_ov006_0211ba88   slot 4 of data_ov006_02142df8, 0x158 bytes
//
// Also outside the delinks join, but src/func_ov006_0211ba88.c EXISTS,
// bannered NONMATCHING (register allocation, div=35). That is section 13's
// 0x020f6904 shape and the cost is a slice line plus a switch arm -- except
// that this one ALSO drops its receiver, so it is host-copied in
// port/unmatched/MgSound_ShadowSlot35.cpp rather than sliced. That file
// carries the evidence.

/* <cstdio> is gone with the floor's reporting case: this file prints nothing
   now, it counts. The seat does the printing. */

/* The eight-byte mwcc member pointer, in the only spelling that is true on
   both machines: two words, no member-pointer type anywhere. */
struct MgPmf { unsigned code; int adj; };

extern "C" {

/* the framework's entry points; see unmatched/MgBase_StateDispatch.cpp */
void port_mg_call0(void *self, unsigned code, int adj);
void port_mg_call1(void *self, unsigned code, int adj, int a);

/* ---- the fifteen state tables, re-typed ---------------------------------
   The ov006 mount defines the storage and __sinit_ov006_02132970 fills it.
   Each is spelled here as an array of two plain ints, which is eight bytes on
   MSVC and eight bytes in the ROM. */
extern MgPmf data_ov006_02142df8[];
extern MgPmf data_ov006_02142e20[];
extern MgPmf data_ov006_02142d38[];
extern MgPmf data_ov006_02142d50[];
extern MgPmf data_ov006_02142d80[];
extern MgPmf data_ov006_02142dc8[];
extern MgPmf data_ov006_02142de0[];
extern MgPmf data_ov006_02142cd8[];
extern MgPmf data_ov006_02142cf0[];
extern MgPmf data_ov006_02142d20[];
extern MgPmf data_ov006_02142d68[];
extern MgPmf data_ov006_02142d98[];
extern MgPmf data_ov006_02142cc0[];
extern MgPmf data_ov006_02142d08[];
extern MgPmf data_ov006_02142db0[];

/* ---- the fifty-seven reachable state bodies ------------------------------
   Reached ONLY through the switch below: the pair words are mounted DATA
   holding DS addresses, so nothing else in the build names them and /OPT:REF
   would drop every one of them without this file. The forty-one that are not
   defined here or in MgSound_ShadowSlot35.cpp are in port/slice_box.txt.

   FIFTY-SIX UNTIL RUN mg10 LANE F367. The fifty-seventh is 0x0211bc8c, which
   run mg9 could only report; it now has a matched body and a slice line and is
   declared and routed exactly like the rest.

   EVERY DECLARATION'S PARAMETER COUNT MATCHES ITS DEFINITION'S. The three
   spelled `(void)` below are empty bodies in src and are declared and called
   with no argument, which is the shape port/tools/aritycheck.py checks and the
   shape MgPanel_StateDispatch.cpp already uses for func_ov006_02104ec8. */
void func_ov006_0211bf44(char *c, int i);   /* 02142df8 p0 */
void func_ov006_0211bc8c(char *c, int i);   /* 02142df8 p1, was THE FLOOR */
void func_ov006_0211bc68(char *c, int i);   /* 02142df8 p2 */
void func_ov006_0211bbe0(char *c, int i);   /* 02142df8 p3 */
int  func_ov006_0211ba88(char *c, int i);   /* 02142df8 p4, host copy */
void func_ov006_0211b590(char *c, int i);   /* 02142e20 p0,  sub-dispatcher */
void func_ov006_0211b398(char *c, int i);   /* 02142e20 p1,  sub-dispatcher */
void func_ov006_0211b17c(char *c, int i);   /* 02142e20 p2,  sub-dispatcher */
void func_ov006_0211af60(char *c, int i);   /* 02142e20 p3,  sub-dispatcher */
void func_ov006_0211ad44(char *c, int i);   /* 02142e20 p4,  sub-dispatcher */
void func_ov006_0211abdc(char *c, int i);   /* 02142e20 p5,  sub-dispatcher */
void func_ov006_0211aa44(char *c, int i);   /* 02142e20 p6,  sub-dispatcher */
void func_ov006_0211a7ac(char *c, int i);   /* 02142e20 p7,  sub-dispatcher */
void func_ov006_0211a648(char *c, int i);   /* 02142e20 p8,  sub-dispatcher */
void func_ov006_0211a4b0(char *c, int i);   /* 02142e20 p9,  sub-dispatcher */
void func_ov006_0211a2c4(char *c, int i);   /* 02142e20 p10, sub-dispatcher */
void func_ov006_0211a0d8(char *c, int i);   /* 02142e20 p11, sub-dispatcher */
void func_ov006_02119eec(char *c, int i);   /* 02142e20 p12, sub-dispatcher */
void func_ov006_0211b4fc(char *c, int i);   /* 02142d38 p0 */
void func_ov006_0211b3ec(char *c, int i);   /* 02142d38 p1 */
void func_ov006_0211b3e8(void);             /* 02142d38 p2, empty body */
void func_ov006_0211b308(char *c, int i);   /* 02142d50 p0 */
void func_ov006_0211b278(char *c, int i);   /* 02142d50 p1 */
void func_ov006_0211b1cc(char *c, int i);   /* 02142d50 p2 */
void func_ov006_0211b0ec(char *c, int i);   /* 02142d80 p0 */
void func_ov006_0211b05c(char *c, int i);   /* 02142d80 p1 */
void func_ov006_0211afb0(char *c, int i);   /* 02142d80 p2 */
void func_ov006_0211aed0(char *c, int i);   /* 02142dc8 p0 */
void func_ov006_0211ae40(char *c, int i);   /* 02142dc8 p1 */
void func_ov006_0211ad94(char *c, int i);   /* 02142dc8 p2 */
void func_ov006_0211ad00(char *c, int i);   /* 02142de0 p0 */
void func_ov006_0211ac30(char *c, int i);   /* 02142de0 p1 */
void func_ov006_0211ac2c(void);             /* 02142de0 p2, empty body */
void func_ov006_0211ab80(char *c, int i);   /* 02142cd8 p0 */
void func_ov006_0211ab0c(char *c, int i);   /* 02142cd8 p1 */
void func_ov006_0211aa94(char *c, int i);   /* 02142cd8 p2 */
void func_ov006_0211a9fc(char *c, int i);   /* 02142cf0 p0 */
void func_ov006_0211a910(char *c, int i);   /* 02142cf0 p1 */
void func_ov006_0211a7fc(char *c, int i);   /* 02142cf0 p2 */
void func_ov006_0211a714(char *c, int i);   /* 02142d20 p0 */
void func_ov006_0211a69c(char *c, int i);   /* 02142d20 p1 */
void func_ov006_0211a698(void);             /* 02142d20 p2, empty body */
void func_ov006_0211a5ec(char *c, int i);   /* 02142d68 p0 */
void func_ov006_0211a578(char *c, int i);   /* 02142d68 p1 */
void func_ov006_0211a500(char *c, int i);   /* 02142d68 p2 */
void func_ov006_0211a420(char *c, int i);   /* 02142d98 p0 */
void func_ov006_0211a388(char *c, int i);   /* 02142d98 p1 */
void func_ov006_0211a314(char *c, int i);   /* 02142d98 p2 */
void func_ov006_0211a234(char *c, int i);   /* 02142cc0 p0 */
void func_ov006_0211a19c(char *c, int i);   /* 02142cc0 p1 */
void func_ov006_0211a128(char *c, int i);   /* 02142cc0 p2 */
void func_ov006_0211a048(char *c, int i);   /* 02142d08 p0 */
void func_ov006_02119fb0(char *c, int i);   /* 02142d08 p1 */
void func_ov006_02119f3c(char *c, int i);   /* 02142d08 p2 */
void func_ov006_02119e5c(char *c, int i);   /* 02142db0 p0 */
void func_ov006_02119dc4(char *c, int i);   /* 02142db0 p1 */
void func_ov006_02119d50(char *c, int i);   /* 02142db0 p2 */

/* THE SUB-OBJECT'S FIELD-HELD PMF DISPATCH MOVED OUT OF THIS FILE.
   func_ov006_020c2b8c belongs to the +0x4f38 sub-object, which dScMgCup_c
   mounts as well -- ov006 has exactly two arm_calls into it, 0x0211c960 (this
   class's slot 6) and 0x020e02dc (0x169's). Run mg9 landed both classes in one
   merge and the body is now defined once, in
   port/unmatched/MgShared4f38_SubDispatch.cpp, which carries the seven-pair
   derivation this file used to hold and routes SIX states rather than this
   lane's six or lane CUP's four -- the union, which is this lane's set. Its
   counters are port_mg_sub4f38_calls / _routed / _idle / _unknown and this
   file's port_mg_sound_counts reports them unchanged. */
unsigned port_mg_sub4f38_calls(void);
unsigned port_mg_sub4f38_routed(void);
unsigned port_mg_sub4f38_unknown(void);

/* what this file DEFINES, so the seat can name it */
void func_ov006_0211b954(char *c);
void func_ov006_0211b5e0(char *c);
void port_mg_sound_counts(unsigned *hits, unsigned *floor, unsigned *unknown,
                          unsigned *field_calls, unsigned *field_routed,
                          unsigned *field_unknown);

}  /* extern "C" */

/* THE WITNESS. A hit is a state this class ROUTED TO A REAL BODY; unknown is a
   code word this switch does not know, which is the number that convicts a
   missed dispatcher.

   THE SECOND COUNTER IS NO LONGER A FLOOR COUNT AND IS NOT SUBTRACTED FROM THE
   HITS. Run mg9 kept it because 0x0211bc8c had no body and an ask for it went
   nowhere; MgPanel_StateDispatch.cpp records reading "2508 routed" when 951 of
   those had gone nowhere, and that mistake was not repeated. Since run mg10
   lane F367 the address has a matched body, so it is counted BOTH as an
   ordinary hit and here, and this counter's only job is to say how many of the
   hits were the tapped-pad state. No case returns a negative any more. */
static unsigned g_snd_hits;
static unsigned g_snd_state1_0211bc8c;
static unsigned g_snd_unknown;

static int sound_try_1(void *self, unsigned code, int a)
{
    char *c = (char *)self;
    switch (code) {
    /* data_ov006_02142df8, 5 slots, dispatched by func_ov006_0211b954 */
    case 0x0211bf44u: func_ov006_0211bf44(c, a);   return 1;
    /* p1, run mg9's one hard floor, decompiled and routed by run mg10 lane
       F367. The separate counter stays: it is the number that says a tap
       produced an EXECUTION and not just an ask. */
    case 0x0211bc8cu: ++g_snd_state1_0211bc8c;
                      func_ov006_0211bc8c(c, a);   return 1;
    case 0x0211bc68u: func_ov006_0211bc68(c, a);   return 1;
    case 0x0211bbe0u: func_ov006_0211bbe0(c, a);   return 1;
    case 0x0211ba88u: func_ov006_0211ba88(c, a);   return 1;
    /* data_ov006_02142e20, 13 slots, dispatched by func_ov006_0211b5e0.
       Every one of the thirteen is itself a dispatcher, host-copied below. */
    case 0x0211b590u: func_ov006_0211b590(c, a);   return 1;
    case 0x0211b398u: func_ov006_0211b398(c, a);   return 1;
    case 0x0211b17cu: func_ov006_0211b17c(c, a);   return 1;
    case 0x0211af60u: func_ov006_0211af60(c, a);   return 1;
    case 0x0211ad44u: func_ov006_0211ad44(c, a);   return 1;
    case 0x0211abdcu: func_ov006_0211abdc(c, a);   return 1;
    case 0x0211aa44u: func_ov006_0211aa44(c, a);   return 1;
    case 0x0211a7acu: func_ov006_0211a7ac(c, a);   return 1;
    case 0x0211a648u: func_ov006_0211a648(c, a);   return 1;
    case 0x0211a4b0u: func_ov006_0211a4b0(c, a);   return 1;
    case 0x0211a2c4u: func_ov006_0211a2c4(c, a);   return 1;
    case 0x0211a0d8u: func_ov006_0211a0d8(c, a);   return 1;
    case 0x02119eecu: func_ov006_02119eec(c, a);   return 1;
    /* data_ov006_02142d38 */
    case 0x0211b4fcu: func_ov006_0211b4fc(c, a);   return 1;
    case 0x0211b3ecu: func_ov006_0211b3ec(c, a);   return 1;
    case 0x0211b3e8u: func_ov006_0211b3e8();       return 1;
    /* data_ov006_02142d50 */
    case 0x0211b308u: func_ov006_0211b308(c, a);   return 1;
    case 0x0211b278u: func_ov006_0211b278(c, a);   return 1;
    case 0x0211b1ccu: func_ov006_0211b1cc(c, a);   return 1;
    /* data_ov006_02142d80 */
    case 0x0211b0ecu: func_ov006_0211b0ec(c, a);   return 1;
    case 0x0211b05cu: func_ov006_0211b05c(c, a);   return 1;
    case 0x0211afb0u: func_ov006_0211afb0(c, a);   return 1;
    /* data_ov006_02142dc8 */
    case 0x0211aed0u: func_ov006_0211aed0(c, a);   return 1;
    case 0x0211ae40u: func_ov006_0211ae40(c, a);   return 1;
    case 0x0211ad94u: func_ov006_0211ad94(c, a);   return 1;
    /* data_ov006_02142de0 */
    case 0x0211ad00u: func_ov006_0211ad00(c, a);   return 1;
    case 0x0211ac30u: func_ov006_0211ac30(c, a);   return 1;
    case 0x0211ac2cu: func_ov006_0211ac2c();       return 1;
    /* data_ov006_02142cd8 */
    case 0x0211ab80u: func_ov006_0211ab80(c, a);   return 1;
    case 0x0211ab0cu: func_ov006_0211ab0c(c, a);   return 1;
    case 0x0211aa94u: func_ov006_0211aa94(c, a);   return 1;
    /* data_ov006_02142cf0 */
    case 0x0211a9fcu: func_ov006_0211a9fc(c, a);   return 1;
    case 0x0211a910u: func_ov006_0211a910(c, a);   return 1;
    case 0x0211a7fcu: func_ov006_0211a7fc(c, a);   return 1;
    /* data_ov006_02142d20 */
    case 0x0211a714u: func_ov006_0211a714(c, a);   return 1;
    case 0x0211a69cu: func_ov006_0211a69c(c, a);   return 1;
    case 0x0211a698u: func_ov006_0211a698();       return 1;
    /* data_ov006_02142d68 */
    case 0x0211a5ecu: func_ov006_0211a5ec(c, a);   return 1;
    case 0x0211a578u: func_ov006_0211a578(c, a);   return 1;
    case 0x0211a500u: func_ov006_0211a500(c, a);   return 1;
    /* data_ov006_02142d98 */
    case 0x0211a420u: func_ov006_0211a420(c, a);   return 1;
    case 0x0211a388u: func_ov006_0211a388(c, a);   return 1;
    case 0x0211a314u: func_ov006_0211a314(c, a);   return 1;
    /* data_ov006_02142cc0 */
    case 0x0211a234u: func_ov006_0211a234(c, a);   return 1;
    case 0x0211a19cu: func_ov006_0211a19c(c, a);   return 1;
    case 0x0211a128u: func_ov006_0211a128(c, a);   return 1;
    /* data_ov006_02142d08 */
    case 0x0211a048u: func_ov006_0211a048(c, a);   return 1;
    case 0x02119fb0u: func_ov006_02119fb0(c, a);   return 1;
    case 0x02119f3cu: func_ov006_02119f3c(c, a);   return 1;
    /* data_ov006_02142db0 */
    case 0x02119e5cu: func_ov006_02119e5c(c, a);   return 1;
    case 0x02119dc4u: func_ov006_02119dc4(c, a);   return 1;
    case 0x02119d50u: func_ov006_02119d50(c, a);   return 1;

    default:
        return 0;
    }
}

/* THE ENTRY POINT the fifteen table host copies call. A hit is this class's; a
   miss falls through to the framework, which owns the guards and the report,
   and is counted here as well because a nonzero unknown count is the number
   that says a dispatcher was missed. */
extern "C" void port_mg_sound_call1(void *self, unsigned code, int adj, int a)
{
    if (code != 0 && adj == 0) {
        if (sound_try_1(self, code, a) > 0) { ++g_snd_hits; return; }
    }
    if (code != 0)
        ++g_snd_unknown;
    port_mg_call1(self, code, adj, a);
}


/* THE SECOND PARAMETER IS STILL SPELLED `floor` SO THE SEAT'S CALL AND EVERY
   OTHER CALLER'S KEEP THE SAME ABI ACROSS THIS CHANGE -- a six-argument
   signature that changed arity here would be exactly the cross-TU drift
   port/tools/aritycheck.py exists to catch. What it CARRIES changed: it is now
   the number of entries into state 1, which run mg10 lane F367 decompiled. */
extern "C" void port_mg_sound_counts(unsigned *hits, unsigned *floor,
                                     unsigned *unknown, unsigned *field_calls,
                                     unsigned *field_routed,
                                     unsigned *field_unknown)
{
    if (hits)          *hits          = g_snd_hits;
    if (floor)         *floor         = g_snd_state1_0211bc8c;
    if (unknown)       *unknown       = g_snd_unknown;
    /* The FIELD counts come from the shared host copy now. The mapping is
       exact: its `calls` counts every entry as this file's did, its `routed`
       counts the same six states, and its `unknown` counts the same misses.
       Its fourth counter, `idle`, has no field in this census and reads zero on
       every measured run of this scene. */
    if (field_calls)   *field_calls   = port_mg_sub4f38_calls();
    if (field_routed)  *field_routed  = port_mg_sub4f38_routed();
    if (field_unknown) *field_unknown = port_mg_sub4f38_unknown();
}

// ---- the sixteen host copies -----------------------------------------------
//
// Each is its src TU with the pair declaration replaced by MgPmf and the
// dispatch replaced by port_mg_sound_call1. Everything else is verbatim, and
// every offset below was read off the ROM disassembly rather than off the src
// struct.

/* src/func_ov006_0211b954.cpp, table 02142df8, arity 1. THE FIRST OF THE TWO
   THIRD-SHAPE TUs: the src reads the pair as two ints and open-codes the
   decode, so neither a link nor a `::*` sweep sees it.
   ROM 0x0211b954, 0x74: r7 = this, r5 = the walking entity pointer, r6 = the
   loop counter. Per iteration `add r0,r5,#0x5000 / ldrb r1,[r0,#0xf5]` is the
   in-play gate at +0x50f5 and `ldrb r0,[r0,#0xf4]` is the state index at
   +0x50f4; `add r3,r4,r0,lsl #3` is an EIGHT-byte stride off the pool word
   0x0211b9c4 = 02142df8. The dispatch takes `this` from r7 -- the ORIGINAL
   object, not the entity -- and `mov r1,r6` puts the loop counter in the
   argument register. `add r5,r5,#0x14 / cmp r6,#0xa` is ten entities at stride
   0x14. */
// PORT_HOST_ABI: mwcc pointer-to-member dispatch open-coded as two ints, arity-1 loop MSVC's pmf ABI cannot reproduce
extern "C" void func_ov006_0211b954(char *c)
{
    int i = 0;
    char *e = c;
    do {
        if (*(unsigned char *)(e + 0x50f5) != 0) {
            const unsigned char idx = *(unsigned char *)(e + 0x50f4);
            const MgPmf *p = &data_ov006_02142df8[idx];
            port_mg_sound_call1(c, p->code, p->adj, i);
        }
        i++;
        e += 0x14;
    } while (i < 10);
}

/* src/func_ov006_0211b5e0.cpp, table 02142e20, arity 1. THE SECOND THIRD-SHAPE
   TU, and the same nineteen-instruction body with three constants changed.
   ROM 0x0211b5e0, 0x74: the gate is `ldrb r1,[r0,#0x1cc]` at +0x51cc, the
   index is `ldrb r0,[r0,#0x1d0]` at +0x51d0, the pool word 0x0211b650 is
   02142e20, and `add r5,r5,#0x24 / cmp r6,#0x1e` is THIRTY entities at stride
   0x24. `mov r1,r6` again, so arity 1. */
// PORT_HOST_ABI: mwcc pointer-to-member dispatch open-coded as two ints, arity-1 loop MSVC's pmf ABI cannot reproduce
extern "C" void func_ov006_0211b5e0(char *c)
{
    int i = 0;
    char *e = c;
    do {
        if (*(unsigned char *)(e + 0x51cc) != 0) {
            const unsigned char idx = *(unsigned char *)(e + 0x51d0);
            const MgPmf *p = &data_ov006_02142e20[idx];
            port_mg_sound_call1(c, p->code, p->adj, i);
        }
        i++;
        e += 0x24;
    } while (i < 30);
}

/* THE THIRTEEN SUB-DISPATCHERS. One shape, thirteen tables. Every one of the
   thirteen ROM bodies is byte identical to 0x0211b590 over its first nineteen
   words and differs only in the pool word that names its table, so the macro
   below is the measurement rather than a convenience: writing thirteen bodies
   by hand would let a typo say something the ROM does not.
   ROM 0x0211b590: `mov r2,#0x24 / mla r2,r1,r2,r0 / add r2,r2,#0x5000 /
   ldrb r2,[r2,#0x1d1]` is the SUB-state index at entity +5, `add ip,r3,r2,lsl
   #3` is the eight-byte stride, and r1 is untouched across the mla so the
   incoming index is still in the argument register at the blx. */
#define SOUND_SUB(sym, table)                                                 \
    extern "C" void sym(char *c, int i)                                       \
    {                                                                         \
        const unsigned char idx =                                             \
            *(unsigned char *)(c + i * 0x24 + 0x51d1);                        \
        const MgPmf *p = &table[idx];                                         \
        port_mg_sound_call1(c, p->code, p->adj, i);                           \
    }

SOUND_SUB(func_ov006_0211b590, data_ov006_02142d38)
SOUND_SUB(func_ov006_0211b398, data_ov006_02142d50)
SOUND_SUB(func_ov006_0211b17c, data_ov006_02142d80)
SOUND_SUB(func_ov006_0211af60, data_ov006_02142dc8)
SOUND_SUB(func_ov006_0211ad44, data_ov006_02142de0)
SOUND_SUB(func_ov006_0211abdc, data_ov006_02142cd8)
SOUND_SUB(func_ov006_0211aa44, data_ov006_02142cf0)
SOUND_SUB(func_ov006_0211a7ac, data_ov006_02142d20)
SOUND_SUB(func_ov006_0211a648, data_ov006_02142d68)
SOUND_SUB(func_ov006_0211a4b0, data_ov006_02142d98)
SOUND_SUB(func_ov006_0211a2c4, data_ov006_02142cc0)
SOUND_SUB(func_ov006_0211a0d8, data_ov006_02142d08)
SOUND_SUB(func_ov006_02119eec, data_ov006_02142db0)

#undef SOUND_SUB

/* func_ov006_020c2b8c is NOT defined here any more -- see the note above the
   declarations at the head of this file. */
