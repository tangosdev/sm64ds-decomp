// PORT_HOST_ABI. The mwcc POINTER-TO-MEMBER WALL, per-class half:
// dScMgMemory_c's four TABLE dispatchers and its twenty-five state addresses.
// Run mg9, lane MMT.  Actor id 0x16a, scene 362, "Memory Match".
//
// Read unmatched/MgBase_StateDispatch.cpp's header first, then
// unmatched/MgMemory2_StateDispatch.cpp.  The first carries the ROM
// disassembly of the mwcc dispatch sequence and the finding that the overlay
// constructors already copy the pairs at the right stride; the second is this
// class's SIBLING and the template this file follows.  Neither is repeated
// here.
//
// THE SIBLING IS A TEMPLATE AND NOT A SOURCE.  dScMgMemory2_c (0x16b, scene
// 363, "Memory Master") has the same five-table shape, the same record stride
// and the same two-index object -- and every address, every offset and every
// record count below was re-read out of this class's own ROM code rather than
// taken from it.  Where the two differ they differ silently: the state indexes
// are at +0x5314 and +0x5318 here against +0x53d4 and +0x53d8 there, and the
// per-record loop runs TWELVE records here against twenty there.
//
// The dispatchers that read a member pointer out of an OBJECT FIELD rather
// than out of a table are NOT in this file and NOT this lane's: they are the
// shared ov006/ov004 glue unmatched/MgMemory2_FieldPmf.cpp already host-copies
// (func_ov006_020c07e8, func_ov006_020c19d0, func_ov004_020b52fc), and this
// class reaches the same three through the same sub-object at +0x4f38.
//
// ---- 1. THE TWENTY-FIVE ADDRESSES, AND WHERE THEY COME FROM ---------------
//
// src/__sinit_ov006_021311c8.c, this class's own overlay constructor, read
// assignment by assignment.
//
// READING THE CONSTRUCTOR IS NOT OPTIONAL HERE AND THE FAN-OUT BRIEF SAYS SO.
// port/mg_fanout_costs.txt section 9 records that its per-id attribution is
// address containment -- does a constructor's copied .data range bracket that
// id's SpawnInfo -- and that it OVER-ATTRIBUTES "for at least 0x16a and
// 0x16b".  The two constructors are adjacent (__sinit_ov006_021311c8 for this
// class, __sinit_ov006_021314e4 for Memory Master) and their pair runs are
// adjacent too, so a lane that took the neighbouring one would get a file that
// compiles, links and dispatches twenty-nine addresses belonging to the class
// next door.  The constructor's own assignment list is the only ground truth,
// and this lane checked the containment as well: exactly ONE doubled-id word
// lies inside 0x0213cfc0..0x0213d088, and it is 0x016a016a at 0x0213cfdc.
//
// A SWEEP OF THE PAIR RANGE WOULD ALSO BE WRONG, for section 4's reason and on
// this class as visibly as on curling: MgMemoryMatch_SpawnInfo sits at
// 0x0213cfd8, INSIDE the run of pair symbols (0x0213cfd0 is a pair, 0x0213cfe0
// is the next one), so a sweep would find the factory word 0x020f5504 and the
// doubled id as a "pair".  The constructor's own list skips it.
//
// Every pair was then read out of extracted/overlays/overlay_0006.bin at base
// 0x020bfec0.  Twenty-five pairs, twenty-five DISTINCT code words, every
// adjustment word zero, and all twenty-five inside this class's own code block
// 0x020f3834..0x020f5564.
//
//   table                n  arity  dispatched by
//   -------------------  -  -----  -------------------------------------------
//   data_ov006_021422dc  5    0    func_ov006_020f5388  (vtable slot 6)
//   data_ov006_02142304  6    0    func_ov006_020f5164
//   data_ov006_021422bc  4    0    func_ov006_020f50f8
//   data_ov006_02142334  7    1    func_ov006_020f3f10
//   data_ov006_021422a4  3    ?    NOTHING -- see section 3
//
// TWO OF THE FIVE TABLES ARE DISPATCHED BY A STATE OF ANOTHER TABLE.
// func_ov006_020f5164 is slot 1 of data_ov006_021422dc and func_ov006_020f50f8
// is slot 3 of it, so both appear in the arity-0 switch below AND are host
// copies further down this file.  That is the sibling's shape (0x020f7234 and
// 0x020f71c8) and not a new one.
//
// ---- 2. THE ROM'S OWN DISPATCH SHAPES, DISASSEMBLED -----------------------
//
// Read out of the shipped overlay image, not taken from src, because the state
// index offset and the `this` a callee is handed are the two things a wrong
// host copy gets silently wrong.
//
//   func_ov006_020f5388  vtable slot 6, Behavior, 0x5c = 23 words
//                        (21 instructions + 2 pool)
//     add r0,r4,#0x5000 / ldr r0,[r0,#0x314]      the index at +0x5314
//     ldr r1,[pc,#0x3c]                           = 0x021422dc
//     add r3,r1,r0,lsl #3                         stride EIGHT
//     ldr r1,[r3,#4] / ands r1,r1,#1 / ...        the ordinary PMF sequence
//     add r0,r4,r1,asr #1 / blx r1                this = the class base
//     bl func_ov004_020b65e4
//     ldr r0,[pc,#0x14] (= 0x00004f38) / add r0,r4,r0 / bl func_ov006_020c19d0
//     mov r0,#1
//
//   func_ov006_020f5164  0x4c = 19 words (18 + 1 pool).  Index at +0x5318,
//     table 0x02142304, then a `mov r0,r4 / bl 0x020f3f10` tail.
//   func_ov006_020f50f8  0x48 = 18 words (17 + 1 pool).  Index at +0x5318,
//     table 0x021422bc, returns nothing.
//   func_ov006_020f3f10  0x74 = 29 words (28 + 1 pool).  The one-argument
//     loop, and it is TWELVE records, not twenty:
//       mov r7,r0          r7 = the CLASS BASE, and it never changes
//       mov r5,r7          r5 = the per-record cursor
//       mov r6,#0          r6 = i
//       ldr r4,[pc,#0x54]  = 0x02142334
//     loop:
//       add r0,r5,#0x5000 / ldrb r1,[r0,#0x1bb]   the live flag, record +0x13
//       cmp r1,#0 / beq skip
//       ldrb r0,[r0,#0x1bc]                       the state byte, record +0x14
//       add r3,r4,r0,lsl #3                       stride EIGHT
//       ...the PMF sequence...
//       add r0,r7,r1,asr #1                       THIS = the class base
//       mov r1,r6                                 ARG  = i, the loop counter
//       blx r2
//       add r6,r6,#1 / cmp r6,#0xc / add r5,r5,#0x18
//
// So the arity-1 callee is passed (class base, i) and NOT (record base, i);
// r5 is only ever used to fetch the two bytes.  The records are the twenty
// halfword/byte structs at +0x51a8 with stride 0x18 -- src/func_ov006_020f4f94.c
// clears exactly TWELVE of them -- so the two bytes the loop reads are record
// field +0x13 (the in-play gate) and +0x14 (the per-card state).
//
// ---- 3. ONE TABLE HAS NO DISPATCHER AND IS CARRIED, NOT ROUTED ------------
//
// data_ov006_021422a4 takes three pairs -- 0x020f3ba0, 0x020f3a14 and
// 0x020f3a10 -- and it has exactly ONE relocation pointing at it in the whole
// overlay, from:0x021314e0, which is inside this class's own .init
// constructor.  The other four tables have two each: the constructor's, and
// their dispatching TU's literal-pool load.  So no code in ov006 names this
// table and its arity is derivable from nothing.  It is the same measurement
// port/mg_fanout_costs.txt section 3 records for the sibling's
// data_ov006_021423a8, on a different table in a different class, and it is a
// statement about the relocation set rather than a claim that the states are
// dead: a dispatch through a computed address would leave no relocation
// either.
//
// THE THREE ADDRESSES ARE DELIBERATELY NOT IN EITHER SWITCH BELOW.  Two of the
// three are spelled (char *, int) in src and the third is a four-byte `bx lr`,
// which LOOKS like arity 1 -- and guessing on that basis is exactly what
// port/tools/stategen.py refuses to do at the arity step.  Run on this
// constructor it refuses with "cannot derive the dispatch arity for 1
// table(s): data_ov006_021422a4", which is the tool agreeing with this
// paragraph rather than this paragraph quoting the tool.  If a dispatch
// through a computed address ever reaches one of them, the framework's own
// UNHANDLED line names the address and the run is the evidence.
//
//     0x020f3ba0   src/func_ov006_020f3ba0.c   void (char *c, int i)
//     0x020f3a14   src/func_ov006_020f3a14.c   void (char *c, int i)
//     0x020f3a10   src/func_ov006_020f3a10.c   void (void), 4 bytes, bx lr
//
// All three ARE in port/slice_mmt.txt, so the bodies are in the build and a
// later lane that finds the dispatcher has only a switch arm to write.
//
// ---- 4. EVERY ROUTED STATE HAS A MATCHED src TU ---------------------------
//
// All twenty-five code words resolve to a src file, all twenty-five have a
// delink block, and none of the twenty-five carries a NONMATCHING banner.  So
// this class has NO state floor at all -- neither the sibling's kind (a
// NONMATCHING body that is still reachable) nor curling's kind (an address
// with no src in either extension).  The bodiless counter below is kept
// structurally zero for the sibling's reason: if a later lane ever adds a
// state this class cannot reach, this is where it is counted.
//
// THE CLASS'S ONE FLOOR WAS NOT A STATE, AND IT IS CLOSED.
// func_ov006_020f3e68, the card draw, the sixth call vtable slot 9 makes, was
// a named trap in unmatched/MgMemory1_Faces.cpp when this file was written.
// Section 13's method in port/mg_fanout_costs.txt is what found it -- a
// delinks join over every callee of every override, not over the state
// addresses -- and it found it in exactly the place that section says to look.
// Run mg10 lane F362 decompiled it: src/func_ov006_020f3e68.c, carried by
// port/slice_mmt.txt, NONMATCHING at 24 of 42 words on colouring and schedule.
// The trap is deleted, so dScMgMemory_c now has NO floor of either kind.
//
// ---- 5. WHY THIS FILE HAS ITS OWN ENTRY POINTS ----------------------------
//
// unmatched/MgBase_StateDispatch.cpp owns port_mg_call0 and port_mg_call1 and
// chains them to exactly one per-class pair, which unmatched/
// MgCurling_StateDispatch.cpp defines.  A second class cannot define those
// names, so this file calls port_mg_memory1_call0 / _call1, which try THIS
// class's switch and hand everything else to the framework unchanged -- the
// shape every seated minigame after curling uses.  The framework therefore
// remains the single place that decides what a null code word means, what a
// nonzero adjustment means and how an unhandled address is reported.

#include <cstdio>

/* The eight-byte mwcc member pointer, in the only spelling that is true on both
   machines: two words, no member-pointer type anywhere. */
struct MgPmf { unsigned code; int adj; };

extern "C" {

/* the framework's entry points; see MgBase_StateDispatch.cpp */
void port_mg_call0(void *self, unsigned code, int adj);
void port_mg_call1(void *self, unsigned code, int adj, int a);

/* ---- the twenty routed state bodies, in address order --------------------
   Each is declared with the parameter list ITS OWN src TU defines, so a
   ride-through is called the way the ROM calls it rather than the way the
   slot's arity would suggest.  The two (void) ones in the arity-1 table are
   four-byte `bx lr` bodies -- there is nothing for an ignored argument to be
   wrong about, which is the ruling unmatched/MgCoin_StateDispatch.cpp and
   unmatched/MgMemory2_StateDispatch.cpp both make for the same shape, and this
   lane re-read both bodies out of the ROM to confirm the shape rather than the
   name.  Pointer parameters spelled with a local struct type in their own src
   (Ctx *, Big *) are void * here: these are C-linkage symbols and the
   declaration has to agree with the ROM's register use, not with a type name
   that exists in one TU.
   0x020f5164 and 0x020f50f8 are host copies further down this file. */
void  func_ov006_020f51b0(char *c);
void  func_ov006_020f5140(void *c);
void  func_ov006_020f50c0(char *self);
void  func_ov006_020f4c38(char *c);
void  func_ov006_020f4bbc(unsigned char *c);
void  func_ov006_020f4b30(char *c);
void  func_ov006_020f4ad4(unsigned char *c);
void *func_ov006_020f4a40(char *c);
void  func_ov006_020f49ac(char *c);
void  func_ov006_020f4888(char *self);
void  func_ov006_020f47d8(char *c);
void  func_ov006_020f46ec(char *c);
void  func_ov006_020f456c(void *c);           /* src spells it Ctx *       */
void  func_ov006_020f43c4(char *self, int i);
void  func_ov006_020f43c0(void);              /* one-argument slot, bx lr body */
void  func_ov006_020f4248(char *self, int idx);
void  func_ov006_020f41b0(char *base, int idx);
void  func_ov006_020f41ac(void);              /* one-argument slot, bx lr body */
void  func_ov006_020f411c(char *c, int i);
void  func_ov006_020f3f84(void *p, int i);    /* src spells it Big *       */

/* the four mount tables this file dispatches, re-typed to the ROM's eight-byte
   pair.  The ov006 mount defines the storage; __sinit_ov006_021311c8 fills it
   at minigame scene load.  data_ov006_021422a4 is deliberately absent -- see
   section 3; nothing here reads it. */
extern MgPmf data_ov006_021422bc[];
extern MgPmf data_ov006_021422dc[];
extern MgPmf data_ov006_02142304[];
extern MgPmf data_ov006_02142334[];

/* the ordinary callees the host copies below keep, each spelled as its own src
   TU spells it.  func_ov006_020c19d0 is the sub-object field dispatcher
   unmatched/MgMemory2_FieldPmf.cpp host-copies; this class reaches the same
   sub-object at the same +0x4f38 offset. */
void func_ov004_020b65e4(void);
void func_ov006_020c19d0(void *c);

/* host-copied further down this file, and called from above their own
   definitions -- 020f5164 and 020f50f8 are STATE BODIES as well as dispatchers,
   and 020f3f10 is 020f5164's tail call. */
void func_ov006_020f5164(void *c);
void func_ov006_020f50f8(void *c);
void func_ov006_020f3f10(void *c);

}  /* extern "C" */

// ---- the class's address switch --------------------------------------------

static unsigned g_mem1_state_hits;
/* THE BODILESS-STATE COUNTER, KEPT AND STRUCTURALLY ZERO. Every address in the
   two switches below reaches a real symbol, so nothing increments this. It is
   kept rather than deleted so hal/scene_mg_memory1.cpp's census field keeps its
   meaning: if a later lane ever adds a state this class cannot reach, this is
   where it is counted, and a nonzero reading is a regression rather than a new
   field. */
static unsigned g_mem1_floor_hits;

static int mem1_try_0(void *self, unsigned code)
{
    char *c = (char *)self;
    switch (code) {
    /* data_ov006_021422dc, dispatched by vtable slot 6 */
    case 0x020f51b0u: func_ov006_020f51b0(c); return 1;
    case 0x020f5164u: func_ov006_020f5164(c); return 1;   /* host copy below */
    case 0x020f5140u: func_ov006_020f5140(c); return 1;
    case 0x020f50f8u: func_ov006_020f50f8(c); return 1;   /* host copy below */
    case 0x020f50c0u: func_ov006_020f50c0(c); return 1;
    /* data_ov006_02142304, dispatched by func_ov006_020f5164 */
    case 0x020f4c38u: func_ov006_020f4c38(c); return 1;
    case 0x020f4bbcu: func_ov006_020f4bbc((unsigned char *)c); return 1;
    case 0x020f4b30u: func_ov006_020f4b30(c); return 1;
    case 0x020f4ad4u: func_ov006_020f4ad4((unsigned char *)c); return 1;
    case 0x020f4a40u: func_ov006_020f4a40(c); return 1;
    case 0x020f49acu: func_ov006_020f49ac(c); return 1;
    /* data_ov006_021422bc, dispatched by func_ov006_020f50f8 */
    case 0x020f4888u: func_ov006_020f4888(c); return 1;
    case 0x020f47d8u: func_ov006_020f47d8(c); return 1;
    case 0x020f46ecu: func_ov006_020f46ec(c); return 1;
    case 0x020f456cu: func_ov006_020f456c(c); return 1;
    default:                                  return 0;
    }
}

static int mem1_try_1(void *self, unsigned code, int a)
{
    char *c = (char *)self;
    switch (code) {
    /* data_ov006_02142334, dispatched by func_ov006_020f3f10.  `c` is the
       CLASS BASE and `a` is the loop counter, in that order; section 2 has the
       disassembly that says so. */
    case 0x020f43c4u: func_ov006_020f43c4(c, a);  return 1;
    case 0x020f43c0u: func_ov006_020f43c0();      return 1;  /* bx lr body */
    case 0x020f4248u: func_ov006_020f4248(c, a);  return 1;
    case 0x020f41b0u: func_ov006_020f41b0(c, a);  return 1;
    case 0x020f41acu: func_ov006_020f41ac();      return 1;  /* bx lr body */
    case 0x020f411cu: func_ov006_020f411c(c, a);  return 1;
    case 0x020f3f84u: func_ov006_020f3f84(c, a);  return 1;
    default:                                      return 0;
    }
}

/* The two entry points the host copies below use.  Everything this switch does
   not own goes to the framework unchanged, so the null-code guard, the
   nonzero-adjustment refusal and the UNHANDLED report all still live in exactly
   one place. */
extern "C" void port_mg_memory1_call0(void *self, unsigned code, int adj)
{
    if (code != 0 && adj == 0 && mem1_try_0(self, code)) {
        ++g_mem1_state_hits;
        return;
    }
    port_mg_call0(self, code, adj);
}

extern "C" void port_mg_memory1_call1(void *self, unsigned code, int adj, int a)
{
    if (code != 0 && adj == 0 && mem1_try_1(self, code, a)) {
        ++g_mem1_state_hits;
        return;
    }
    port_mg_call1(self, code, adj, a);
}

extern "C" unsigned port_mg_memory1_state_hits(void) { return g_mem1_state_hits; }
extern "C" unsigned port_mg_memory1_floor_hits(void) { return g_mem1_floor_hits; }

// ---- the four host copies --------------------------------------------------
//
// Each is its src TU verbatim except for the table declaration (MgPmf rather
// than a member-pointer type) and the dispatch site (port_mg_memory1_callN
// rather than `(c->*table[i].pmf)()`).  Where anything else moved it is stated
// on the line.

/* src/func_ov006_020f5388.cpp -- dScMgMemory_c::Behavior, vtable SLOT 6.  Its
   src reads
       struct C; typedef void (C::*PMF)();
       struct Entry { PMF pmf; };
       extern "C" Entry data_ov006_021422dc[];
       struct C { char pad[0x5314]; int idx; };
       (c->*(data_ov006_021422dc[c->idx].pmf))();
       func_ov004_020b65e4();
       func_ov006_020c19d0((char*)c + 0x4f38);
       return 1;
   NOTE THE `extern "C"` ON THE TABLE: this one is SILENT to a link.  It mangles
   as the plain C name the ov006 mount already defines, so the linker is
   satisfied while MSVC strides the eight-byte table by four.  It is one of the
   two silent shapes port/mg_fanout_costs.txt section 4 names, and only the
   `::*` sweep finds it. */
// PORT_HOST_ABI: mwcc table pointer-to-member dispatch, the 8-byte pair MSVC's 4-byte pmf strides wrong
extern "C" int func_ov006_020f5388(void *self)
{
    char *c = (char *)self;
    const MgPmf *e = &data_ov006_021422dc[*(int *)(c + 0x5314)];
    port_mg_memory1_call0(c, e->code, e->adj);
    func_ov004_020b65e4();
    func_ov006_020c19d0(c + 0x4f38);
    return 1;
}

/* src/func_ov006_020f5164.cpp.  Its table is declared at C++ linkage
   (`extern Entry data_ov006_02142304[];`), so this one IS link-visible, as
   ?data_ov006_02142304@@3PAUEntry@@A -- the PAU spelling that slips both of
   port/tools/facegen.py's guards, which is tool finding 1 in section 10.
   Refused by hand and host-copied here rather than aliased.
   THE RETURN TYPE IS THE ONE PLACE THIS FILE DOES NOT COPY src.  src spells
   this body `int` and returns func_ov006_020f3f10's value, while
   src/func_ov006_020f3f10.cpp defines that symbol `void` -- the two TUs
   disagree, and the ROM settles it by not caring: `mov r0,r4 / bl 0x020f3f10 /
   pop / bx lr` returns whatever the tail call left in r0, and the only consumer
   is an arity-0 PMF dispatch typed `void (C::*)()` that discards it.  Spelling
   both void here is the reading that cannot be wrong; inventing a return value
   would be.
   PORT_HOST_ABI: mwcc pointer-to-member wall (this class's MgPmf {code,adj}
   table, decoded through port_mg_memory1_call0) plus the link-visible PAU
   table name that slips facegen's alias guards (section 10). Host-copied. */
extern "C" void func_ov006_020f5164(void *c)
{
    char *p = (char *)c;
    const MgPmf *e = &data_ov006_02142304[*(int *)(p + 0x5318)];
    port_mg_memory1_call0(p, e->code, e->adj);
    func_ov006_020f3f10(p);
}

/* src/func_ov006_020f50f8.cpp.  Its table is declared inside extern "C"
   (`extern "C" Entry data_ov006_021422bc[];`), so this is the SECOND silent
   shape: the link never complains and MSVC strides eight bytes by four.
   PORT_HOST_ABI: mwcc pointer-to-member wall, this class's MgPmf {code,adj}
   table decoded through port_mg_memory1_call0. Host-copied. */
extern "C" void func_ov006_020f50f8(void *c)
{
    char *p = (char *)c;
    const MgPmf *e = &data_ov006_021422bc[*(int *)(p + 0x5318)];
    port_mg_memory1_call0(p, e->code, e->adj);
}

/* src/func_ov006_020f3f10.cpp -- the arity-1 per-record loop.  Its src reads
       struct C75; typedef void (C75::*PMF)(int);
       extern PMF data_ov006_02142334[];
       for (i = 0; i < 0xc; i++) {
           if (*(unsigned char*)(s + 0x51bb))
               (this->*data_ov006_02142334[*(unsigned char*)(s + 0x51bc)])(i);
           s += 0x18;
       }
   and `this` in that expression is the CLASS BASE, not the record cursor `s`.
   The ROM keeps them in two registers (r7 and r5) and only ever adds the
   adjustment to r7; section 2 has the disassembly.  Twelve records, which is
   this class's whole board -- src/func_ov006_020f4f94.c clears twelve and
   src/func_ov006_020f4cd8.c deals 8, 10 or 12 of them. */
// PORT_HOST_ABI: mwcc arity-1 table pointer-to-member dispatch, the 8-byte pair MSVC's 4-byte pmf strides wrong
extern "C" void func_ov006_020f3f10(void *c)
{
    char *base = (char *)c;
    char *rec  = base;
    for (int i = 0; i < 0xc; ++i, rec += 0x18) {
        if (*(unsigned char *)(rec + 0x51bb) == 0)
            continue;
        const MgPmf *e =
            &data_ov006_02142334[*(unsigned char *)(rec + 0x51bc)];
        port_mg_memory1_call1(base, e->code, e->adj, i);
    }
}
