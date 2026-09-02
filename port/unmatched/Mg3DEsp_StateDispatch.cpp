// PORT_HOST_ABI.  The mwcc POINTER-TO-MEMBER WALL, per-class half:
// dScMg3DEsp_c's SEVEN table dispatchers and its twenty-two state addresses.
// Run mg9, lane PSY.  Actor id 0x185, scene 389, "Psyche Out!".
//
// Read unmatched/MgBase_StateDispatch.cpp's header first, then
// unmatched/MgMemory2_StateDispatch.cpp.  The first carries the ROM
// disassembly of the mwcc dispatch sequence and the finding that the overlay
// constructors already copy the pairs at the right stride; the second is the
// template this file follows.  Neither is repeated here.
//
// ---- 1. THE TWENTY-TWO ADDRESSES, AND WHERE THEY COME FROM -----------------
//
// src/__sinit_ov006_02130a08.c, this class's own overlay constructor, read
// assignment by assignment -- NOT by sweeping the pair address range.  That
// trap is live for this class in TWO different ways at once, which is worth
// more than the addresses:
//
//   MgPsycheOut_SpawnInfo sits at 0x0213c78c, INSIDE the run of pair symbols
//   (0x0213c784 is table 02141f44 slot 2, 0x0213c794 is table 02141fac slot 3).
//   A sweep reads {0x020ea1f0, 0x01850185} there -- the factory word and the
//   doubled id -- as a pair with a wild adjustment.
//
//   And SIX MORE eight-byte records inside the same span read as {code, 0} and
//   are copied by NOTHING:
//       0x0213c704 {020e7fac,0}  0x0213c744 {020e7fac,0}  0x0213c754 {020e7cc0,0}
//       0x0213c764 {020e7de8,0}  0x0213c76c {020e7f04,0}  0x0213c774 {020e7fac,0}
//   plus 0x0213c75c, which is the ASCII string "ki_mizu" and reads as
//   {0x6d5f696b, 0x00757a69}.  The constructor's own list skips all eight.
//
// Every pair was then read out of extracted/overlays/overlay_0006.bin at base
// 0x020bfec0.  TWENTY-TWO pairs, TWENTY-TWO DISTINCT code words, and every
// adjustment word reads zero.
//
//   table                n  arity  dispatched by
//   -------------------  -  -----  -------------------------------------------
//   data_ov006_02141f2c  3    0    func_ov006_020e9e00  (VTABLE SLOT 6)
//   data_ov006_02141fac  4    0    func_ov006_020e9b70  (= f2c slot 1)
//   data_ov006_02141f5c  3    1    func_ov006_020e9374
//   data_ov006_02141f74  3    1    func_ov006_020e8d08
//   data_ov006_02141f1c  2    1    func_ov006_020e8a44
//   data_ov006_02141f8c  4    1    func_ov006_020e8830  (= f1c slot 1)
//   data_ov006_02141f44  3    0    func_ov006_020e82fc
//
// EVERY ONE OF THE SEVEN TABLES HAS EXACTLY TWO RELOCATIONS POINTING AT IT --
// the constructor's, at 0x02130d84..0x02130de8 inside __sinit_ov006_02130a08's
// own .init block, and its dispatcher's literal-pool load.  So this class has
// no orphan table at all, which is the thing port/mg_fanout_costs.txt section 3
// records as 0x16b's one open question and section 13 re-opens.  Nothing here
// is carried unrouted.
//
// ---- 2. THE MACHINE IS THREE LEVELS DEEP ----------------------------------
//
// port/mg_fanout_costs.txt section 14 says the cost question is not "how many
// TUs name the table" but "how deep is the machine", and dScMgPanel_c's answer
// was two.  This class's is THREE:
//
//   slot 6  020e9e00  -> f2c[+0x553c]
//                         [0] 020e9bbc
//                         [1] 020e9b70 -> fac[+0x5540]
//                                           [0] 020e97b0
//                                           [1] 020e96f4 -> 020e9374 -> f5c
//                                           [2] 020e968c -> 020e9374 -> f5c
//                                           [3] 020e9670 -> 020e8d08 -> f74
//                                                        -> 020e9374 -> f5c
//                                         then -> 020e82fc -> f44[+0x5552]
//                         [2] 020e989c -> 020e8d08 -> f74
//           020e9e00  -> 020e8a44 -> f1c[per-record +0x52d9]
//                                      [0] 020e8928
//                                      [1] 020e8830 -> f8c[per-record +0x52da]
//
// A lane that host-copied the Behavior alone would link, boot, and jump to a
// raw DS address the first frame f2c advanced to slot 1.
//
// ---- 3. THE ARITIES ARE HAND-READ OFF THE ROM, NOT TAKEN FROM src ---------
//
// The two things a wrong host copy gets silently wrong are the state index
// offset and what the callee is handed, so both were read out of the shipped
// overlay image.  All seven, verbatim from the disassembly:
//
//   020e9e00  add r0,r4,#0x5000 / ldr r0,[r0,#0x53c]     index at +0x553c
//             ldr r1,[pc,#0x4c] (= 0x02141f2c)
//             add r3,r1,r0,lsl #3                        stride EIGHT
//             ldr r1,[r3,#4] / ands r1,r1,#1 / ldrne ... the PMF sequence
//             add r0,r4,r1,asr #1 / blx r1               NO r1 -> ARITY 0
//
//   020e9b70  index at +0x5540, table 02141fac, ARITY 0, tail
//             `mov r0,r4 / bl 0x020e82fc` whose r0 IS the return value
//
//   020e9374  mov r7,r0 / mov r5,r7 / mov r6,#0 / ldr r4,[pc] (= 02141f5c)
//             loop: add r0,r5,#0x5000 / ldrb r1,[r0,#0x218]   the live gate
//                   cmp r1,#0 / beq skip
//                   ldrb r0,[r0,#0x219]                       the state byte
//                   add r3,r4,r0,lsl #3 / ...PMF...
//                   add r0,r7,r1,asr #1                       THIS = class base
//                   mov r1,r6                                 ARG = i  -> ARITY 1
//                   blx r2
//             skip: add r6,r6,#1 / cmp r6,#5 / add r5,r5,#0x18 / blt
//
//   020e8d08  the same loop, gate +0x5290, index +0x5291, 3 records, stride
//             0x14, ARITY 1
//
//   020e8a44  add r0,r6,r5,lsl #5 / add r0,r0,#0x5000        stride 0x20
//             ldrb r1,[r0,#0x2d4] gate / ldrb r0,[r0,#0x2d9] index
//             add r0,r6,r1,asr #1 / mov r1,r5 / blx r2       ARITY 1
//             cmp r5,#0x14                                    20 records
//
//   020e8830  add r0,r5,r4,lsl #5 / add r0,r0,#0x5000 / ldrb r0,[r0,#0x2da]
//             mov r1,r4 / blx r2                             ARITY 1
//
//   020e82fc  add r1,r0,#0x5000 / ldrb r2,[r1,#0x552]        index at +0x5552
//             cmp r2,#3 / bxhs lr                            THE ROM'S OWN BOUND
//             ...PMF... / blx r1                             NO r1 -> ARITY 0
//
// Every one of the twenty-two src TUs spells the parameter list its table's
// arity predicts -- the three arity-0 tables' bodies take (this) and the four
// arity-1 tables' bodies take (this, int).  Twenty-two for twenty-two, with no
// (void) body and no ride-through anywhere, which is a cleaner agreement than
// either precedent class got.
//
// 020e82fc's `cmp r2,#3 / bxhs lr` is worth naming on its own: the ROM states
// that table's length in its own instructions, so f44's three slots are
// measured twice -- by the constructor's assignment list and by the bound.
//
// ---- 4. FOUR SHAPES, AND TWO OF THEM ARE THE ONE NEITHER DETECTOR SEES ----
//
// port/mg_fanout_costs.txt section 4 prescribes two detectors, the link and a
// `::*` source sweep, and says between them they find twelve of thirteen.  Of
// this class's seven:
//
//   func_ov006_020e9374   C++-linkage `extern PMF data_ov006_02141f5c[];`
//   func_ov006_020e8d08   C++-linkage PMF                    LINK-VISIBLE, P8
//   func_ov006_020e9b70   `struct Entry { PMF pmf; }` array   LINK-VISIBLE, PAU
//   func_ov006_020e9e00   `extern "C" PMF ...[]`   SILENT to the link
//   func_ov006_020e8a44   `extern "C" PMF ...[]`   SILENT to the link
//   func_ov006_020e8830   OPEN-CODED INTS          SILENT TO BOTH
//   func_ov006_020e82fc   OPEN-CODED INTS          SILENT TO BOTH
//
// THE LAST TWO ARE THE THIRD SHAPE run mg5 lane FLW found at
// func_ov006_020c3d18 and run mg6 lane PPP found twice more.  Neither declares
// a member-pointer type and neither has a PMF global, so there is no symbol for
// a link to fail on AND no `::*` for a source sweep to match.  src/
// func_ov006_020e8830.c reads the pair as `int* ent = &data_ov006_02141f8c[b*2]`
// and does its own `adj >> 1` / `adj & 1` decode; src/func_ov006_020e82fc.cpp
// declares `struct Ent{ int a; int b; }` inside extern "C" and does the same.
// Both compile, both link, and only a run convicts them -- the tell is eip on a
// raw DS address.
//
// SO THE COUNT OF THIRD-SHAPE TUs IN THE TREE IS NOW SIX, not four, and the
// "treat twelve as a floor rather than a census" line in section 4 keeps
// earning its place.
//
// AND ONE OF THE TWO IS REACHED THROUGH THE OTHER: 020e8830 is slot 1 of the
// table 020e8a44 dispatches, and 020e8a44 is called unconditionally from the
// Behavior.  A lane that trusted a clean link and a clean sweep would have
// shipped both.
//
// ---- 5. THE TABLE SYMBOLS ARE SPELLED AS TWO INTS AND STAY MOUNTED --------
//
// port/mg_fanout_costs.txt section 4's rulebook corollary keys on the spelling:
// a pair whose consumer names a member-pointer type needs a host copy, a pair
// spelled as two ints is safe as an alias.  All seven tables are re-declared
// here as MgPmf -- two words, no member-pointer type anywhere -- and the
// STORAGE stays the ov006 mount's, so __sinit_ov006_02130a08 keeps filling the
// ROM's own words at the ROM's own stride.  Nothing rewrites a stored pair.
//
// ---- 6. WHY THIS FILE HAS ITS OWN ENTRY POINTS ----------------------------
//
// unmatched/MgBase_StateDispatch.cpp owns port_mg_call0 and port_mg_call1 and
// chains them to exactly one per-class pair, which
// unmatched/MgCurling_StateDispatch.cpp defines.  A second class cannot define
// those names, so this file calls port_mg_esp3d_call0 / _call1, which try THIS
// class's switch and hand everything else to the framework unchanged -- the
// shape MgCoin_, MgLuigi_, MgPachinko_ and MgMemory2_StateDispatch.cpp all use.
// The framework therefore remains the single place that decides what a null
// code word means, what a nonzero adjustment means, and how an unhandled
// address is reported.

#include <cstdio>

/* The eight-byte mwcc member pointer, in the only spelling that is true on both
   machines: two words, no member-pointer type anywhere. */
struct MgPmf { unsigned code; int adj; };

extern "C" {

/* the framework's entry points; see MgBase_StateDispatch.cpp */
void port_mg_call0(void *self, unsigned code, int adj);
void port_mg_call1(void *self, unsigned code, int adj, int a);

/* ---- the twenty routed state bodies, in address order --------------------
   Each is declared with the parameter list ITS OWN src TU defines.  There is
   no ride-through and no (void) body in this class: every arity-0 slot's body
   takes one pointer and every arity-1 slot's takes a pointer and an int, which
   is what section 3 above measures against the ROM.  020e9b70 and 020e8830 are
   host copies further down this file. */
void func_ov006_020e9bbc(char *c);                  /* f2c 0 */
void func_ov006_020e989c(void *cc);                 /* f2c 2 */
void func_ov006_020e97b0(char *c);                  /* fac 0 */
void func_ov006_020e96f4(char *thiz);               /* fac 1 */
void func_ov006_020e968c(char *c);                  /* fac 2 */
void func_ov006_020e9670(void *t);                  /* fac 3 */
void func_ov006_020e9318(char *c, int i);           /* f5c 0 */
void func_ov006_020e91a0(char *self, int idx);      /* f5c 1 */
void func_ov006_020e8f14(char *self, int i);        /* f5c 2 */
void func_ov006_020e8cb0(char *c, int i);           /* f74 0 */
void func_ov006_020e8c74(char *c, int i);           /* f74 1 */
void func_ov006_020e8bd0(char *c, int i);           /* f74 2 */
void func_ov006_020e8928(char *o, int idx);         /* f1c 0 */
void func_ov006_020e8728(char *c, int idx);         /* f8c 0 */
void func_ov006_020e85f0(char *c, int i);           /* f8c 1 */
void func_ov006_020e84b8(char *c, int i);           /* f8c 2 */
void func_ov006_020e83bc(char *c, int i);           /* f8c 3 */
void func_ov006_020e82c8(unsigned char *c);         /* f44 0 */
void func_ov006_020e8214(char *c);                  /* f44 1 */
void func_ov006_020e81e0(char *self);               /* f44 2 */

/* the seven mount tables this file dispatches, re-typed to the ROM's eight-byte
   pair.  The ov006 mount defines the storage; __sinit_ov006_02130a08 fills it
   at minigame scene load. */
extern MgPmf data_ov006_02141f1c[];
extern MgPmf data_ov006_02141f2c[];
extern MgPmf data_ov006_02141f44[];
extern MgPmf data_ov006_02141f5c[];
extern MgPmf data_ov006_02141f74[];
extern MgPmf data_ov006_02141f8c[];
extern MgPmf data_ov006_02141fac[];

/* the ordinary callees the host copies below keep, each spelled as its own src
   TU spells it */
void _ZN9Animation7AdvanceEv(void *anim);
void func_ov006_020e7be8(void *p);

/* host-copied further down this file, and called from above their own
   definitions: 020e9b70 and 020e8830 are STATE BODIES as well as dispatchers,
   020e82fc is 020e9b70's tail call, and 020e8a44 is called straight out of the
   Behavior. */
void func_ov006_020e9b70(void *c);
void func_ov006_020e9374(void *c);
void func_ov006_020e8d08(void *c);
void func_ov006_020e8a44(void *self);
void func_ov006_020e8830(char *base, int idx);
void func_ov006_020e82fc(char *c);

}  /* extern "C" */

// ---- the class's address switch --------------------------------------------

static unsigned g_esp_state_hits;
/* THE BODILESS-STATE COUNTER, KEPT AND STRUCTURALLY ZERO.  Every address in the
   two switches below reaches a real src symbol -- this class has no state with
   no body, which section 4 of port/mg_fanout_costs.txt says to expect one or
   two of -- so nothing increments this.  It is kept rather than deleted so
   hal/scene_mg_psycheout.cpp's census field keeps its meaning: if a later lane
   ever adds a state this class cannot reach, this is where it is counted, and a
   nonzero reading is a regression rather than a new field. */
static unsigned g_esp_floor_hits;
/* How often the one NONMATCHING routed body ran (func_ov006_020e8830, which is
   both a state and a dispatcher), counted on its own so a run can say the
   bannered TU is exercised rather than merely built. */
static unsigned g_esp_nonmatching_calls;

static int esp_try_0(void *self, unsigned code)
{
    char *c = (char *)self;
    switch (code) {
    /* data_ov006_02141f2c, dispatched by vtable slot 6 */
    case 0x020e9bbcu: func_ov006_020e9bbc(c);  return 1;
    case 0x020e9b70u: func_ov006_020e9b70(c);  return 1;   /* host copy below */
    /* the dispatched-state return value is DISCARDED at every level of this
       class: slot 6's PMF is spelled `void (C::*)()`, so nothing above ever
       reads what 020e9b70 leaves in r0, and 020e9b70 in turn only rides
       020e82fc's through.  Both host copies below return void and say so. */
    case 0x020e989cu: func_ov006_020e989c(c);  return 1;
    /* data_ov006_02141fac, dispatched by func_ov006_020e9b70 */
    case 0x020e97b0u: func_ov006_020e97b0(c);  return 1;
    case 0x020e96f4u: func_ov006_020e96f4(c);  return 1;
    case 0x020e968cu: func_ov006_020e968c(c);  return 1;
    case 0x020e9670u: func_ov006_020e9670(c);  return 1;
    /* data_ov006_02141f44, dispatched by func_ov006_020e82fc */
    case 0x020e82c8u: func_ov006_020e82c8((unsigned char *)c); return 1;
    case 0x020e8214u: func_ov006_020e8214(c);  return 1;
    case 0x020e81e0u: func_ov006_020e81e0(c);  return 1;
    default:                                   return 0;
    }
}

static int esp_try_1(void *self, unsigned code, int a)
{
    char *c = (char *)self;
    switch (code) {
    /* data_ov006_02141f5c, dispatched by func_ov006_020e9374.  `c` is the
       CLASS BASE and `a` is the loop counter, in that order; section 3 has the
       disassembly that says so -- r7 never moves and only r5 walks the
       records. */
    case 0x020e9318u: func_ov006_020e9318(c, a);  return 1;
    case 0x020e91a0u: func_ov006_020e91a0(c, a);  return 1;
    case 0x020e8f14u: func_ov006_020e8f14(c, a);  return 1;
    /* data_ov006_02141f74, dispatched by func_ov006_020e8d08 */
    case 0x020e8cb0u: func_ov006_020e8cb0(c, a);  return 1;
    case 0x020e8c74u: func_ov006_020e8c74(c, a);  return 1;
    case 0x020e8bd0u: func_ov006_020e8bd0(c, a);  return 1;
    /* data_ov006_02141f1c, dispatched by func_ov006_020e8a44 */
    case 0x020e8928u: func_ov006_020e8928(c, a);  return 1;
    case 0x020e8830u: ++g_esp_nonmatching_calls;
                      func_ov006_020e8830(c, a);  return 1;  /* host copy below */
    /* data_ov006_02141f8c, dispatched by func_ov006_020e8830 */
    case 0x020e8728u: func_ov006_020e8728(c, a);  return 1;
    case 0x020e85f0u: func_ov006_020e85f0(c, a);  return 1;
    case 0x020e84b8u: func_ov006_020e84b8(c, a);  return 1;
    case 0x020e83bcu: func_ov006_020e83bc(c, a);  return 1;
    default:                                      return 0;
    }
}

/* The two entry points the host copies below use.  Everything this switch does
   not own goes to the framework unchanged, so the null-code guard, the
   nonzero-adjustment refusal and the UNHANDLED report all still live in exactly
   one place. */
extern "C" void port_mg_esp3d_call0(void *self, unsigned code, int adj)
{
    if (code != 0 && adj == 0 && esp_try_0(self, code)) {
        ++g_esp_state_hits;
        return;
    }
    port_mg_call0(self, code, adj);
}

extern "C" void port_mg_esp3d_call1(void *self, unsigned code, int adj, int a)
{
    if (code != 0 && adj == 0 && esp_try_1(self, code, a)) {
        ++g_esp_state_hits;
        return;
    }
    port_mg_call1(self, code, adj, a);
}

extern "C" unsigned port_mg_esp3d_state_hits(void)  { return g_esp_state_hits; }
extern "C" unsigned port_mg_esp3d_floor_hits(void)  { return g_esp_floor_hits; }
extern "C" unsigned port_mg_esp3d_nonmatching(void) { return g_esp_nonmatching_calls; }

/* Per-table dispatch counts, so a run can say WHICH of the seven ran rather
   than only that the machine ran.  Section 2's depth is the reason: a boot that
   settles in f2c slot 0 dispatches the Behavior's table every frame and none of
   the other six, and a single total cannot tell that apart from a machine that
   is working its way down. */
static unsigned g_esp_table_hits[7];
enum { T_F2C, T_FAC, T_F5C, T_F74, T_F1C, T_F8C, T_F44 };
extern "C" void port_mg_esp3d_table_counts(unsigned *out7)
{
    for (int i = 0; i < 7; ++i) out7[i] = g_esp_table_hits[i];
}

// ---- the seven host copies -------------------------------------------------
//
// Each is its src TU verbatim except for the table declaration (MgPmf rather
// than a member-pointer type or a bare int array) and the dispatch site
// (port_mg_esp3d_callN rather than the member-pointer call or the open-coded
// decode).  Where anything else moved it is stated on the line.

/* src/func_ov006_020e9e00.cpp -- dScMg3DEsp_c::Behavior, VTABLE SLOT 6.  Its
   src reads
       class C; typedef void (C::*PMF)();
       extern "C" PMF data_ov006_02141f2c[];
       int idx = *(int *)(f + 0x553c);
       (self->*data_ov006_02141f2c[idx])();
       func_ov006_020e8a44(self);
       _ZN9Animation7AdvanceEv(f + 0x51f4);
       func_ov006_020e7be8(f + 0x4fd8);
       return 1;
   NOTE THE `extern "C"` ON THE TABLE: this one is SILENT to a link.  It mangles
   as the plain C name the ov006 mount already defines, so the linker is
   satisfied while MSVC strides the eight-byte table by four.  port/
   mg_fanout_costs.txt section 14's practical rule -- read the src before wiring
   slot 6 by name -- is what caught it here. */
/* PORT_HOST_ABI: mwcc pointer-to-member dispatch (dScMg3DEsp_c three-level state machine); the 8-byte {code,adj} pair is host-copied as an address switch, MSVC's 4-byte member pointer cannot express it */
extern "C" int func_ov006_020e9e00(void *self)
{
    char *c = (char *)self;
    const MgPmf *e = &data_ov006_02141f2c[*(int *)(c + 0x553c)];
    ++g_esp_table_hits[T_F2C];
    port_mg_esp3d_call0(c, e->code, e->adj);
    func_ov006_020e8a44(c);
    _ZN9Animation7AdvanceEv(c + 0x51f4);
    func_ov006_020e7be8(c + 0x4fd8);
    return 1;
}

/* src/func_ov006_020e9b70.cpp, and it is ALSO slot 1 of the table above.  Its
   src declares `struct Entry { PMF pmf; }; extern Entry data_ov006_02141fac[];`
   at C++ linkage, so this is the ?data_ov006_02141fac@@3PAUEntry@@A spelling --
   the PAU form that slips both of port/tools/facegen.py's guards (section 10,
   tool finding 1).  The struct is four bytes on MSVC and eight in the ROM.

   THE RETURN VALUE IS A RIDE-THROUGH AND IT IS DROPPED ON PURPOSE.  The ROM's
   tail is `mov r0,r4 / bl 0x020e82fc / pop / bx lr`, so 020e9b70 returns
   whatever 020e82fc leaves in r0, which is whatever the state IT dispatched
   left there.  Neither value is reachable: 020e82fc's own src spells it
   `void`, and the only consumer of 020e9b70's is slot 6's arity-0 member
   pointer, which is `void (C::*)()`.  Two host copies cannot carry a value
   through a routed call anyway -- port_mg_esp3d_call0 is void -- so this is
   recorded rather than reproduced. */
/* PORT_HOST_ABI: mwcc pointer-to-member dispatch (dScMg3DEsp_c three-level state machine); the 8-byte {code,adj} pair is host-copied as an address switch, MSVC's 4-byte member pointer cannot express it */
extern "C" void func_ov006_020e9b70(void *c)
{
    char *p = (char *)c;
    const MgPmf *e = &data_ov006_02141fac[*(int *)(p + 0x5540)];
    ++g_esp_table_hits[T_FAC];
    port_mg_esp3d_call0(p, e->code, e->adj);
    func_ov006_020e82fc(p);
}

/* src/func_ov006_020e9374.cpp.  C++-linkage `extern PMF data_ov006_02141f5c[];`
   with a one-argument member pointer, so this one IS link-visible as a P8
   mangle.  Five records at +0x5218, stride 0x18; the gate is the record's own
   byte and the index is the byte after it; `this` is the CLASS BASE at every
   iteration and the argument is the loop counter. */
/* PORT_HOST_ABI: mwcc pointer-to-member dispatch (dScMg3DEsp_c three-level state machine); the 8-byte {code,adj} pair is host-copied as an address switch, MSVC's 4-byte member pointer cannot express it */
extern "C" void func_ov006_020e9374(void *c)
{
    char *base = (char *)c;
    char *s = base;
    for (int i = 0; i < 5; ++i, s += 0x18) {
        if (*(unsigned char *)(s + 0x5218)) {
            const MgPmf *e = &data_ov006_02141f5c[*(unsigned char *)(s + 0x5219)];
            ++g_esp_table_hits[T_F5C];
            port_mg_esp3d_call1(base, e->code, e->adj, i);
        }
    }
}

/* src/func_ov006_020e8d08.cpp.  The same shape one table over: three records at
   +0x5290, stride 0x14. */
/* PORT_HOST_ABI: mwcc pointer-to-member dispatch (dScMg3DEsp_c three-level state machine); the 8-byte {code,adj} pair is host-copied as an address switch, MSVC's 4-byte member pointer cannot express it */
extern "C" void func_ov006_020e8d08(void *c)
{
    char *base = (char *)c;
    char *s = base;
    for (int i = 0; i < 3; ++i, s += 0x14) {
        if (*(unsigned char *)(s + 0x5290)) {
            const MgPmf *e = &data_ov006_02141f74[*(unsigned char *)(s + 0x5291)];
            ++g_esp_table_hits[T_F74];
            port_mg_esp3d_call1(base, e->code, e->adj, i);
        }
    }
}

/* src/func_ov006_020e8a44.cpp -- `extern "C" PMF data_ov006_02141f1c[];`, the
   second SILENT one, and the Behavior calls it unconditionally on every frame.
   Twenty records, stride 0x20 (the ROM's `lsl #5`), gate at +0x52d4 and index
   at +0x52d9 of each. */
/* PORT_HOST_ABI: mwcc pointer-to-member dispatch (dScMg3DEsp_c three-level state machine); the 8-byte {code,adj} pair is host-copied as an address switch, MSVC's 4-byte member pointer cannot express it */
extern "C" void func_ov006_020e8a44(void *self)
{
    char *base = (char *)self;
    for (int i = 0; i < 0x14; ++i) {
        char *r = base + i * 0x20;
        if (*(unsigned char *)(r + 0x52d4)) {
            const MgPmf *e = &data_ov006_02141f1c[*(unsigned char *)(r + 0x52d9)];
            ++g_esp_table_hits[T_F1C];
            port_mg_esp3d_call1(base, e->code, e->adj, i);
        }
    }
}

/* src/func_ov006_020e8830.c -- THE THIRD SHAPE, and also slot 1 of the table
   above.  Its src is bannered
       NONMATCHING: different op / idiom (div=36). Logic verified correct vs ROM
   and it decodes the pair by hand out of `extern int data_ov006_02141f8c[]`:
       int* ent = &data_ov006_02141f8c[b*2];
       int adj = ent[1];
       void* obj = base + (adj >> 1);
       if (adj & 1) { void* vt = *(void**)obj; fn = *(Fn*)((char*)vt + ent[0]); }
       else         { fn = (Fn)ent[0]; }
       fn(obj, idx);
   which is the five-instruction ARM Itanium sequence one for one and reads
   clean on a link and on a `::*` sweep.  ONLY THE DECODE MOVES; everything
   below the call is the src's own arithmetic, kept line for line, and it is
   what the ROM does at 0x020e8878..0x020e8918. */
/* PORT_HOST_ABI: mwcc pointer-to-member dispatch (dScMg3DEsp_c three-level state machine); the 8-byte {code,adj} pair is host-copied as an address switch, MSVC's 4-byte member pointer cannot express it */
extern "C" void func_ov006_020e8830(char *base, int idx)
{
    char *e = base + idx * 0x20;
    const MgPmf *ent = &data_ov006_02141f8c[*(unsigned char *)(e + 0x52da)];
    ++g_esp_table_hits[T_F8C];
    port_mg_esp3d_call1(base, ent->code, ent->adj, idx);

    unsigned char c = *(unsigned char *)(e + 0x52d6);
    *(int *)(base + 0x52c8 + idx * 0x20) -= (c << 3) + 0x10;
    int sh = *(unsigned short *)(e + 0x52cc) >> 3;
    if (sh >= 7) sh = 7;
    *(unsigned char *)(e + 0x52d7) = (unsigned char)(7 - sh);
    if (*(unsigned short *)(e + 0x52cc) != 0) {
        *(unsigned short *)(base + 0x52cc + idx * 0x20) -= 1;
        if (*(short *)(e + 0x52cc) < 0) *(short *)(e + 0x52cc) = 0;
    } else {
        *(unsigned char *)(e + 0x52d9) = 0;
    }
}

/* src/func_ov006_020e82fc.cpp -- THE THIRD SHAPE AGAIN, spelled
   `struct Ent{ int a; int b; }` inside extern "C", which is eight bytes on both
   machines and therefore reads and links entirely clean.  THE BOUND IS THE
   ROM'S OWN: `cmp r2,#3 / bxhs lr` at 0x020e830c, and it is kept rather than
   dropped because it is the only place in the class that states a table
   length in code. */
/* PORT_HOST_ABI: mwcc pointer-to-member dispatch (dScMg3DEsp_c three-level state machine); the 8-byte {code,adj} pair is host-copied as an address switch, MSVC's 4-byte member pointer cannot express it */
extern "C" void func_ov006_020e82fc(char *c)
{
    unsigned char idx = *(unsigned char *)(c + 0x5552);
    if (idx >= 3) return;
    const MgPmf *e = &data_ov006_02141f44[idx];
    ++g_esp_table_hits[T_F44];
    port_mg_esp3d_call0(c, e->code, e->adj);
}
