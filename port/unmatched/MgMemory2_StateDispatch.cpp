// PORT_HOST_ABI. The mwcc POINTER-TO-MEMBER WALL, per-class half:
// dScMgMemory2_c's four TABLE dispatchers and its twenty-nine state addresses.
// Run mg6, lane MEM.  Actor id 0x16b, scene 363, "Memory Master".
//
// Read unmatched/MgBase_StateDispatch.cpp's header first, then
// unmatched/MgCoin_StateDispatch.cpp.  The first carries the ROM disassembly of
// the mwcc dispatch sequence and the finding that the overlay constructors
// already copy the pairs at the right stride; the second is the template this
// file follows.  Neither is repeated here.
//
// The three dispatchers that read a member pointer out of an OBJECT FIELD
// rather than out of a table are in unmatched/MgMemory2_FieldPmf.cpp, because
// they are a different defect (a four-byte field where the ROM has eight moves
// every field after it) and two of the three are shared ov006/ov004 glue rather
// than this class's.
//
// ---- 1. THE TWENTY-NINE ADDRESSES, AND WHERE THEY COME FROM ----------------
//
// src/__sinit_ov006_021314e4.c, this class's own overlay constructor, read
// assignment by assignment -- NOT by sweeping the pair address range, which is
// the trap port/mg_fanout_costs.txt section 4 names and which this class walks
// straight into: MgMemoryMaster_SpawnInfo sits at 0x0213d288, INSIDE the run of
// pair symbols (0x0213d280 is a pair, 0x0213d290 is the next one), so a sweep
// would find the factory word and the doubled id 0x016b016b as a "pair".  The
// constructor's own list skips it.
//
// Every pair was then read out of extracted/overlays/overlay_0006.bin at base
// 0x020bfec0.  Twenty-nine pairs, twenty-nine DISTINCT code words, and every
// adjustment word reads zero.  The listing is
// runs/mg6/out/MEM/state_pairs_16b.txt.
//
//   table                n  arity  dispatched by
//   -------------------  -  -----  -------------------------------------------
//   data_ov006_021423e0  5    0    func_ov006_020f7458  (vtable slot 6)
//   data_ov006_02142440  10   0    func_ov006_020f7234
//   data_ov006_021423c0  4    0    func_ov006_020f71c8
//   data_ov006_02142408  7    1    func_ov006_020f5c40
//   data_ov006_021423a8  3    ?    NOTHING -- see section 3
//
// TWO OF THE FIVE TABLES ARE DISPATCHED BY A STATE OF ANOTHER TABLE.
// func_ov006_020f7234 is slot 1 of data_ov006_021423e0 and func_ov006_020f71c8
// is slot 3 of it, so both appear in the arity-0 switch below AND are host
// copies further down this file.  That is the MgCoin_StateDispatch shape
// (0x020de26c and 0x020de440) and not a new one.
//
// ---- 2. THE ROM'S OWN DISPATCH SHAPES, DISASSEMBLED --------------------
//
// Read out of the shipped overlay image, not taken from src, because the state
// index offset and the `this` a callee is handed are the two things a wrong
// host copy gets silently wrong.
//
//   func_ov006_020f7458  vtable slot 6, Behavior
//     add r0,r4,#0x5000 / ldr r0,[r0,#0x3d4]      the index at +0x53d4
//     ldr r1,[pc,#0x3c]                           = 0x021423e0
//     add r3,r1,r0,lsl #3                         stride EIGHT
//     ldr r1,[r3,#4] / ands r1,r1,#1 / ...        the ordinary PMF sequence
//     add r0,r4,r1,asr #1 / blx                   this = the class base
//     bl func_ov004_020b65e4
//     ldr r0,[pc,#0x14] (= 0x00004f38) / add r0,r4,r0 / bl func_ov006_020c19d0
//     mov r0,#1
//
//   func_ov006_020f7234  index at +0x53d8, table 0x02142440, then a
//     `mov r0,r4 / bl 0x020f5c40` tail whose r0 IS the return value
//   func_ov006_020f71c8  index at +0x53d8, table 0x021423c0, returns nothing
//   func_ov006_020f5c40  the one-argument loop:
//     mov r7,r0          r7 = the CLASS BASE, and it never changes
//     mov r5,r7          r5 = the per-record cursor
//     mov r6,#0          r6 = i
//     ldr r4,[pc,#0x54]  = 0x02142408
//   loop:
//     add r0,r5,#0x5000 / ldrb r1,[r0,#0x1bb]     the live flag
//     cmp r1,#0 / beq skip
//     ldrb r0,[r0,#0x1bc]                         the state byte
//     add r3,r4,r0,lsl #3                         stride EIGHT
//     ...the PMF sequence...
//     add r0,r7,r1,asr #1                         THIS = the class base
//     mov r1,r6                                   ARG  = i, the loop counter
//     blx r2
//     add r6,r6,#1 / cmp r6,#0x14 / add r5,r5,#0x18
//
// So the arity-1 callee is passed (class base, i) and NOT (record base, i);
// r5 is only ever used to fetch the two bytes.  The seven bodies in that table
// agree: five of them are spelled (char *c, int i) in their own src and the
// other two take (void) and are four bytes of `bx lr` in the ROM.
//
// ---- 3. ONE TABLE HAS NO DISPATCHER AND IS CARRIED, NOT ROUTED ------------
//
// data_ov006_021423a8 takes three pairs -- 0x020f58d0, 0x020f5744 and
// 0x020f5740 -- and port/mg_fanout_costs.txt section 3 measures that it has
// exactly ONE relocation pointing at it in the whole overlay, from 0x02131898,
// which is inside this class's own .init constructor.  The other four tables
// have two each: the constructor's, and their dispatching TU's literal-pool
// load.  So no code in ov006 names this table and its arity is derivable from
// nothing.
//
// THE THREE ADDRESSES ARE DELIBERATELY NOT IN EITHER SWITCH BELOW.  Two of the
// three are spelled (char *, int) in src and the third is a four-byte `bx lr`,
// which LOOKS like arity 1 -- and guessing on that basis is exactly what
// port/tools/stategen.py refuses to do at the arity step, correctly.  If a
// dispatch through a computed address ever reaches one of them, the framework's
// own UNHANDLED line names the address and the run is the evidence.  That is a
// better outcome than a switch arm that cannot be wrong because nothing calls
// it.
//
//     0x020f58d0   src/func_ov006_020f58d0.c   void (char *c, int i)
//     0x020f5744   src/func_ov006_020f5744.c   void (char *base, int i)
//     0x020f5740   src/func_ov006_020f5740.c   void (void), 4 bytes, bx lr
//
// ---- 4. ONE STATE HAS A src TU AND NO DELINK BLOCK ------------------------
//
// 0x020f6904 is slot 5 of data_ov006_02142440.  config/arm9/overlays/ov006/
// delinks.txt has a hole there -- the block before it ends at 0x020f6904 and
// the next starts at 0x020f6a00 -- so port/tools/stategen.py cannot emit a case
// for it and this file's case is hand-written.  src/func_ov006_020f6904.c
// EXISTS and is bannered
//
//     NONMATCHING: different op / idiom (div=29). Logic verified correct vs
//     ROM; not byte-matchable from C at mwccarm 1.2/sp2p3.
//
// NONMATCHING TUs are routinely sliced and built by this port, so the state is
// reachable and the cost is one slice line plus one switch arm.  It is NOT a
// floor: port/mg_fanout_costs.txt's own correction to itself says so, and the
// contrast is curling's func_ov006_020e1854, which has no src in either
// extension.  THIS CLASS HAS NO HARD STATE FLOOR AT ALL -- all twenty-nine
// addresses reach a real src body, and the count that would say otherwise is
// reported by hal/scene_mg_memory2.cpp on every run so a regression is loud.
//
// ---- 5. WHY THIS FILE HAS ITS OWN ENTRY POINTS ----------------------------
//
// unmatched/MgBase_StateDispatch.cpp owns port_mg_call0 and port_mg_call1 and
// chains them to exactly one per-class pair, port_mg_try_ov006_0 and _1, which
// unmatched/MgCurling_StateDispatch.cpp defines.  A second class cannot define
// those names, so this file calls port_mg_memory2_call0 / _call1, which try
// THIS class's switch and hand everything else to the framework unchanged --
// the shape MgCoin_StateDispatch.cpp, MgLuigi_StateDispatch.cpp and
// MgPachinko_StateDispatch.cpp all use.  The framework therefore remains the
// single place that decides what a null code word means, what a nonzero
// adjustment means and how an unhandled address is reported.

#include <cstdio>

/* The eight-byte mwcc member pointer, in the only spelling that is true on both
   machines: two words, no member-pointer type anywhere. */
struct MgPmf { unsigned code; int adj; };

extern "C" {

/* the framework's entry points; see MgBase_StateDispatch.cpp */
void port_mg_call0(void *self, unsigned code, int adj);
void port_mg_call1(void *self, unsigned code, int adj, int a);

/* ---- the twenty-six routed state bodies, in address order ----------------
   Each is declared with the parameter list ITS OWN src TU defines, so a
   ride-through is called the way the ROM calls it rather than the way the
   slot's arity would suggest.  The two (void) ones in the arity-1 table are
   four-byte `bx lr` bodies -- there is nothing for an ignored argument to be
   wrong about, which is the MgCoin_StateDispatch.cpp ruling for the same
   shape.  0x020f7234 and 0x020f71c8 are host copies further down this file. */
void  func_ov006_020f5cb4(void *ctx, int idx);
void  func_ov006_020f5de0(char *c, int i);
void  func_ov006_020f5e70(void);              /* one-argument slot, bx lr body */
void  func_ov006_020f5e74(char *base, int idx);
void  func_ov006_020f5f0c(char *self, int idx);
void  func_ov006_020f6084(void);              /* one-argument slot, bx lr body */
void  func_ov006_020f6088(char *self, int i);
void  func_ov006_020f6230(char *p);
void  func_ov006_020f639c(char *c);
void  func_ov006_020f6488(char *c);
void  func_ov006_020f6538(char *c);
void  func_ov006_020f6678(char *c);
void *func_ov006_020f670c(char *c);
void  func_ov006_020f67a0(char *c);
void  func_ov006_020f6830(char *o);
void  func_ov006_020f6904(char *c);           /* NONMATCHING src, section 4 */
void  func_ov006_020f6a00(char *thiz);
void  func_ov006_020f6a78(char *c);
void  func_ov006_020f6b00(char *thiz);
void  func_ov006_020f6b78(char *thiz);
void  func_ov006_020f6bf0(char *c);
void  func_ov006_020f7190(char *self);
void  func_ov006_020f7210(void *c);
void  func_ov006_020f7280(char *c);

/* the four mount tables this file dispatches, re-typed to the ROM's eight-byte
   pair.  The ov006 mount defines the storage; __sinit_ov006_021314e4 fills it
   at minigame scene load.  data_ov006_021423a8 is deliberately absent -- see
   section 3; nothing here reads it. */
extern MgPmf data_ov006_021423c0[];
extern MgPmf data_ov006_021423e0[];
extern MgPmf data_ov006_02142408[];
extern MgPmf data_ov006_02142440[];

/* the ordinary callees the host copies below keep, each spelled as its own src
   TU spells it */
void func_ov004_020b65e4(void);
void func_ov006_020c19d0(void *c);

/* host-copied further down this file, and called from above their own
   definitions -- 020f7234 and 020f71c8 are STATE BODIES as well as dispatchers,
   and 020f5c40 is 020f7234's tail call. */
void func_ov006_020f7234(void *c);
void func_ov006_020f71c8(void *c);
void func_ov006_020f5c40(void *c);

}  /* extern "C" */

// ---- the class's address switch --------------------------------------------

static unsigned g_mem2_state_hits;
/* THE BODILESS-STATE COUNTER, KEPT AND STRUCTURALLY ZERO. Every address in the
   two switches below reaches a real symbol, so nothing increments this. It is
   kept rather than deleted so hal/scene_mg_memory2.cpp's census field keeps its
   meaning: if a later lane ever adds a state this class cannot reach, this is
   where it is counted, and a nonzero reading is a regression rather than a new
   field. */
static unsigned g_mem2_floor_hits;
/* How often the NONMATCHING state of section 4 ran, counted on its own so the
   run can say the hole is closed rather than merely unreported. */
static unsigned g_mem2_nonmatching_calls;

static int mem2_try_0(void *self, unsigned code)
{
    char *c = (char *)self;
    switch (code) {
    /* data_ov006_021423e0, dispatched by vtable slot 6 */
    case 0x020f7280u: func_ov006_020f7280(c); return 1;
    case 0x020f7234u: func_ov006_020f7234(c); return 1;   /* host copy below */
    case 0x020f7210u: func_ov006_020f7210(c); return 1;
    case 0x020f71c8u: func_ov006_020f71c8(c); return 1;   /* host copy below */
    case 0x020f7190u: func_ov006_020f7190(c); return 1;
    /* data_ov006_02142440, dispatched by func_ov006_020f7234 */
    case 0x020f6bf0u: func_ov006_020f6bf0(c); return 1;
    case 0x020f6b78u: func_ov006_020f6b78(c); return 1;
    case 0x020f6b00u: func_ov006_020f6b00(c); return 1;
    case 0x020f6a78u: func_ov006_020f6a78(c); return 1;
    case 0x020f6a00u: func_ov006_020f6a00(c); return 1;
    case 0x020f6904u: ++g_mem2_nonmatching_calls;
                      func_ov006_020f6904(c); return 1;   /* section 4 */
    case 0x020f6830u: func_ov006_020f6830(c); return 1;
    case 0x020f67a0u: func_ov006_020f67a0(c); return 1;
    case 0x020f670cu: func_ov006_020f670c(c); return 1;
    case 0x020f6678u: func_ov006_020f6678(c); return 1;
    /* data_ov006_021423c0, dispatched by func_ov006_020f71c8 */
    case 0x020f6538u: func_ov006_020f6538(c); return 1;
    case 0x020f6488u: func_ov006_020f6488(c); return 1;
    case 0x020f639cu: func_ov006_020f639c(c); return 1;
    case 0x020f6230u: func_ov006_020f6230(c); return 1;
    default:                                  return 0;
    }
}

static int mem2_try_1(void *self, unsigned code, int a)
{
    char *c = (char *)self;
    switch (code) {
    /* data_ov006_02142408, dispatched by func_ov006_020f5c40.  `c` is the
       CLASS BASE and `a` is the loop counter, in that order; section 2 has the
       disassembly that says so. */
    case 0x020f6088u: func_ov006_020f6088(c, a);  return 1;
    case 0x020f6084u: func_ov006_020f6084();      return 1;  /* bx lr body */
    case 0x020f5f0cu: func_ov006_020f5f0c(c, a);  return 1;
    case 0x020f5e74u: func_ov006_020f5e74(c, a);  return 1;
    case 0x020f5e70u: func_ov006_020f5e70();      return 1;  /* bx lr body */
    case 0x020f5de0u: func_ov006_020f5de0(c, a);  return 1;
    case 0x020f5cb4u: func_ov006_020f5cb4(c, a);  return 1;
    default:                                      return 0;
    }
}

/* The two entry points the host copies below use.  Everything this switch does
   not own goes to the framework unchanged, so the null-code guard, the
   nonzero-adjustment refusal and the UNHANDLED report all still live in exactly
   one place. */
extern "C" void port_mg_memory2_call0(void *self, unsigned code, int adj)
{
    if (code != 0 && adj == 0 && mem2_try_0(self, code)) {
        ++g_mem2_state_hits;
        return;
    }
    port_mg_call0(self, code, adj);
}

extern "C" void port_mg_memory2_call1(void *self, unsigned code, int adj, int a)
{
    if (code != 0 && adj == 0 && mem2_try_1(self, code, a)) {
        ++g_mem2_state_hits;
        return;
    }
    port_mg_call1(self, code, adj, a);
}

extern "C" unsigned port_mg_memory2_state_hits(void)     { return g_mem2_state_hits; }
extern "C" unsigned port_mg_memory2_floor_hits(void)     { return g_mem2_floor_hits; }
extern "C" unsigned port_mg_memory2_nonmatching(void)    { return g_mem2_nonmatching_calls; }

// ---- the four host copies --------------------------------------------------
//
// Each is its src TU verbatim except for the table declaration (MgPmf rather
// than a member-pointer type) and the dispatch site (port_mg_memory2_callN
// rather than `(c->*table[i].pmf)()`).  Where anything else moved it is stated
// on the line.

/* src/func_ov006_020f7458.cpp -- dScMgMemory2_c::Behavior, vtable SLOT 6.  Its
   src reads
       struct C; typedef void (C::*PMF)();
       struct Entry { PMF pmf; };
       extern "C" Entry data_ov006_021423e0[];
       struct C { char pad[0x53d4]; int idx; };
       (c->*(data_ov006_021423e0[c->idx].pmf))();
       func_ov004_020b65e4();
       func_ov006_020c19d0((char*)c + 0x4f38);
       return 1;
   NOTE THE `extern "C"` ON THE TABLE: this one is SILENT to a link.  It mangles
   as the plain C name the ov006 mount already defines, so the linker is
   satisfied while MSVC strides the eight-byte table by four.  It is one of the
   two silent shapes port/mg_fanout_costs.txt section 4 names, and only the
   `::*` sweep finds it. */
/* PORT_HOST_ABI: mwcc pointer-to-member dispatch (dScMgMemory2_c state table); the 8-byte {code,adj} pair is host-copied as an address switch, MSVC's 4-byte member pointer cannot express it */
extern "C" int func_ov006_020f7458(void *self)
{
    char *c = (char *)self;
    const MgPmf *e = &data_ov006_021423e0[*(int *)(c + 0x53d4)];
    port_mg_memory2_call0(c, e->code, e->adj);
    func_ov004_020b65e4();
    func_ov006_020c19d0(c + 0x4f38);
    return 1;
}

/* src/func_ov006_020f7234.cpp.  Its table is declared at C++ linkage
   (`extern Entry data_ov006_02142440[];`), so this one IS link-visible, as
   ?data_ov006_02142440@@3PAUEntry@@A -- the PAU spelling that slips both of
   port/tools/facegen.py's guards.  See section 4 of port/mg_fanout_costs.txt.

   ONE THING OTHER THAN THE DISPATCH MOVED, and it is a dead value.  src ends
   `return func_ov006_020f5c40(c);` with a local declaration of that callee as
   int, while the callee's own definition returns void; on ARM that is the
   ordinary r0 ride-through and in the ROM it is `mov r0,r4 / bl 0x020f5c40`
   with the callee's r0 falling out.  Both declarations cannot live in one host
   TU, and the value is dead in every case -- this function is reached only as
   slot 1 of an arity-0 (void) table -- so it is DROPPED rather than invented.
   Nothing reads it. */
/* PORT_HOST_ABI: mwcc pointer-to-member dispatch (dScMgMemory2_c state table); the 8-byte {code,adj} pair is host-copied as an address switch, MSVC's 4-byte member pointer cannot express it */
extern "C" void func_ov006_020f7234(void *self)
{
    char *c = (char *)self;
    const int j = *(int *)(c + 0x53d8);
    port_mg_memory2_call0(c, data_ov006_02142440[j].code,
                             data_ov006_02142440[j].adj);
    func_ov006_020f5c40(c);
}

/* src/func_ov006_020f71c8.cpp.  Table declared inside extern "C", so SILENT.
   The index is at +0x53d8, which is the neighbour of slot 6's +0x53d4 and not
   the same field; both offsets are read off the ROM in section 2. */
/* PORT_HOST_ABI: mwcc pointer-to-member dispatch (dScMgMemory2_c state table); the 8-byte {code,adj} pair is host-copied as an address switch, MSVC's 4-byte member pointer cannot express it */
extern "C" void func_ov006_020f71c8(void *self)
{
    char *c = (char *)self;
    const MgPmf *e = &data_ov006_021423c0[*(int *)(c + 0x53d8)];
    port_mg_memory2_call0(c, e->code, e->adj);
}

/* src/func_ov006_020f5c40.cpp -- the ONE-ARGUMENT loop.  Its src spells the
   table as a bare member-pointer array at C++ linkage
   (`extern PMF data_ov006_02142408[];`), so it is link-visible as
   ?data_ov006_02142408@@3PAP8C77@@AEXH@ZA, the P8 form facegen's WALL test does
   catch.

   The loop, the two byte offsets, the 0x18 stride, the twenty iterations and
   the argument are src's, and every one of them is confirmed against the
   disassembly in section 2. */
/* PORT_HOST_ABI: mwcc pointer-to-member dispatch (dScMgMemory2_c state table); the 8-byte {code,adj} pair is host-copied as an address switch, MSVC's 4-byte member pointer cannot express it */
extern "C" void func_ov006_020f5c40(void *self)
{
    char *base = (char *)self;
    char *s = base;
    for (int i = 0; i < 0x14; i++) {
        if (*(unsigned char *)(s + 0x51bb)) {
            const MgPmf *e =
                &data_ov006_02142408[*(unsigned char *)(s + 0x51bc)];
            port_mg_memory2_call1(base, e->code, e->adj, i);
        }
        s += 0x18;
    }
}
