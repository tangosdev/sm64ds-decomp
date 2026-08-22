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
// ---- ONE HARD FLOOR, AND IT IS NOT IN THE nosrc COLUMN --------------------
//
//   func_ov006_0211bc8c   slot 1 of data_ov006_02142df8, 0x2b8 bytes
//
// It has a config symbol, NO delink block (the gap 0x0211bc8c..0x0211bf44 is
// exactly its symbol size) and NO src file in either extension anywhere in the
// tree. port/mg_fanout_costs.txt section 3 lists this class at nosrc 0, and
// that column is right: it is computed over VTABLE slots, and this floor is a
// STATE. Section 13's CORRECTION 3 says the same thing from the other side --
// "both of this class's 'is it a floor' questions were answered wrong by
// looking at the STATE tables" -- and the general rule that survives both is
// that the floor question has to be asked of every reachable address, not of
// one axis. It is emitted below as a REPORTING case and never as a call, and
// no symbol is invented for it.
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

#include <cstdio>

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

/* ---- the fifty-six reachable state bodies -------------------------------
   Reached ONLY through the switch below: the pair words are mounted DATA
   holding DS addresses, so nothing else in the build names them and /OPT:REF
   would drop every one of them without this file. The forty that are not
   defined here or in MgSound_ShadowSlot35.cpp join port/slice_box.txt in the
   same commit.

   EVERY DECLARATION'S PARAMETER COUNT MATCHES ITS DEFINITION'S. The three
   spelled `(void)` below are empty bodies in src and are declared and called
   with no argument, which is the shape port/tools/aritycheck.py checks and the
   shape MgPanel_StateDispatch.cpp already uses for func_ov006_02104ec8. */
void func_ov006_0211bf44(char *c, int i);   /* 02142df8 p0 */
/* 02142df8 p1 is func_ov006_0211bc8c, THE FLOOR -- no symbol is invented */
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

/* the non-dispatching callees the sub-object host copy below reaches */
int  func_ov006_020c2be8(void *c);
int  func_ov006_020c2290(void *c);
void _ZN9Animation7AdvanceEv(void *a);
void _ZN14BlendModelAnim7AdvanceEv(void *b);

/* what this file DEFINES, so the seat can name it */
void func_ov006_0211b954(char *c);
void func_ov006_0211b5e0(char *c);
void func_ov006_020c2b8c(void *c);
void port_mg_sound_counts(unsigned *hits, unsigned *floor, unsigned *unknown,
                          unsigned *field_calls, unsigned *field_routed);

}  /* extern "C" */

/* THE WITNESS, AND THE COUNTS ARE DISJOINT ON PURPOSE. A hit is a state this
   class ROUTED TO A REAL BODY; a floor entry is the one address with no body;
   unknown is a code word this switch does not know, which is the number that
   convicts a missed dispatcher. MgPanel_StateDispatch.cpp records that an
   earlier version of its own counter added floor entries to the hit count and
   read "2508 routed" when 951 of those went nowhere; that mistake is not
   repeated here -- the switch returns -1 for a floor and only +1 counts. */
static unsigned g_snd_hits;
static unsigned g_snd_floor_02142df8;
static unsigned g_snd_unknown;
/* the FIELD dispatch is counted separately: a table state and an object-field
   state are different failures and a single number cannot say which fired. */
static unsigned g_snd_field_calls;
static unsigned g_snd_field_routed;

static int sound_try_1(void *self, unsigned code, int a)
{
    char *c = (char *)self;
    switch (code) {
    /* data_ov006_02142df8, 5 slots, dispatched by func_ov006_0211b954 */
    case 0x0211bf44u: func_ov006_0211bf44(c, a);   return 1;
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

    /* ---- THE FLOOR, REPORTED AND NEVER CALLED ---------------------------
     *
     * 0x0211bc8c is slot 1 of data_ov006_02142df8. It has a config symbol
     * (0x2b8 bytes), no delink block and no src file in either extension. It
     * is one of the ten per-entity states of the 0x14-stride array, so a class
     * that asks for it loses one entity's tick and carries on; the count is
     * the witness and this is the name. REPORTED ONCE, not once per entry, for
     * MgPanel_StateDispatch.cpp's reason -- a per-entry line puts identical
     * rows in the shipped playlog and says nothing the count does not. */
    case 0x0211bc8cu:
        if (++g_snd_floor_02142df8 == 1)
            std::fprintf(stderr, "  [scene] dScMgSound_c FLOOR: state "
                         "0x0211bc8c (slot 1 of data_ov006_02142df8) has no "
                         "delink block and no src file in any extension. The "
                         "state is not entered. (Reported once; the count is "
                         "in the run report.)\n");
        return -1;
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
        const int r = sound_try_1(self, code, a);
        if (r > 0) { ++g_snd_hits; return; }
        if (r < 0) { return; }            /* the named floor, already counted */
    }
    if (code != 0)
        ++g_snd_unknown;
    port_mg_call1(self, code, adj, a);
}

/* AND THE ZERO-ARGUMENT ONE, for the sub-object's FIELD pointer only. This
   class contributes no arity-0 TABLE, so there is nothing to switch on here
   and nothing is invented: the call goes straight to the framework, which
   knows the ov004 and ov006 addresses other seats routed and reports anything
   else by name. What the field holds has never been observed -- no TU in this
   class's closure writes it, and the ROM's own `cmp r2,#0 / beq` guard at
   0x020c2b9c means a .bss zero is a legal state. The two counters below are
   how a run says which of those happened. */
extern "C" void port_mg_sound_call0(void *self, unsigned code, int adj)
{
    ++g_snd_field_calls;
    if (code != 0)
        ++g_snd_field_routed;
    port_mg_call0(self, code, adj);
}

extern "C" void port_mg_sound_counts(unsigned *hits, unsigned *floor,
                                     unsigned *unknown, unsigned *field_calls,
                                     unsigned *field_routed)
{
    if (hits)         *hits         = g_snd_hits;
    if (floor)        *floor        = g_snd_floor_02142df8;
    if (unknown)      *unknown      = g_snd_unknown;
    if (field_calls)  *field_calls  = g_snd_field_calls;
    if (field_routed) *field_routed = g_snd_field_routed;
}

// ---- the sixteen host copies -----------------------------------------------
//
// Each is its src TU with the pair declaration replaced by MgPmf and the
// dispatch replaced by port_mg_sound_call0/1. Everything else is verbatim, and
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

/* src/func_ov006_020c2b8c.cpp -- THE FIELD SHAPE, section 4's group of four.
   The member pointer is the first eight bytes OF THE SUB-OBJECT at this+0x4f38
   rather than a table entry, so no global is emitted for it in any linkage and
   no link anywhere can name it; only the `::*` in the source finds it, and the
   src declares `struct C { PMF pmf; }`, which is FOUR bytes on MSVC.
   ROM 0x020c2b8c, 0x5c:
       bl 0x020c2be8
       ldr r2,[r4]          the code word
       cmp r2,#0 / beq      the ROM's own null guard
       ldr r1,[r4,#4]       the adjustment
       add r0,r4,r1,asr #1
       ands r1,r1,#1 / ldrne r1,[r0] / ldrne r1,[r1,r2] / ldreq r1,[r4]
       blx r1               NO argument register is set -- arity 0
       Animation::Advance(this+0xc8), Animation::Advance(this+0xdc),
       BlendModelAnim::Advance(this+8), func_ov006_020c2290(this)
   The three trailing offsets are computed from the object pointer directly, so
   the four-byte member pointer in the src struct shifts nothing else and the
   only defect is the decode. THIS TU IS SHARED with dScMgCup_c's closure --
   the only other arm_call to 0x020c2b8c in ov006 is from 0x020e02dc -- and
   port/slice_mga361.txt, which lists it, is in NO target today, so this seat
   is the first to build it. If 0x169 is ever seated, ITS lane inherits this
   host copy rather than writing a second: a second definition of the same
   symbol is an LNK2005, not a safety net. */
extern "C" void func_ov006_020c2b8c(void *c)
{
    char *o = (char *)c;
    const MgPmf *p = (const MgPmf *)o;
    func_ov006_020c2be8(c);
    port_mg_sound_call0(c, p->code, p->adj);
    _ZN9Animation7AdvanceEv(o + 0xc8);
    _ZN9Animation7AdvanceEv(o + 0xdc);
    _ZN14BlendModelAnim7AdvanceEv(o + 8);
    func_ov006_020c2290(c);
}
