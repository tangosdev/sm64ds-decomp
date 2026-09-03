// PORT_HOST_ABI. The mwcc POINTER-TO-MEMBER WALL for dScMgHanachan_c, actor id
// 0x182, scene 386 -- the "Which Wiggler" minigame. Run mg9, lane WIG.
//
// Read port/slice_wig.txt for the identity derivation, the five width checks and
// the closure. Read unmatched/MgBase_StateDispatch.cpp's header first for the
// ROM's dispatch sequence and for port_mg_call0, which every chain in this
// family ends at. This file is the whole of this class's dispatch half.
//
// ---- 1. THIS CLASS HAS NO STATE TABLE AT ALL, AND THAT IS THE FINDING -------
//
// Every seated minigame so far dispatches an INDEXED TABLE that its overlay
// constructor builds in .bss: curling's data_ov006_02141950, the panel's
// two-level 02142888/02142840 pair, Memory Master's five. This class has none.
// It has NO OVERLAY CONSTRUCTOR either, and that is measured rather than
// assumed: not one relocation in config/arm9/overlays/ov006/relocs.txt whose
// source lies in any __sinit block lands anywhere in
// [0x0213c98c, 0x0213cab8], the whole of this class's .data.
//
// What it has instead is THIRTY-FIVE INDIVIDUALLY SYMBOLISED {code, adj} PAIRS
// laid end to end in .data at 0x0213c95c..0x0213ca74, stride 8, EVERY
// ADJUSTMENT WORD ZERO, each with exactly ONE load relocation reaching it and
// that one from a code literal pool inside this class's own block. So the ROM
// does not index a table; each state-setting site names the one pair it
// installs, and the member pointer lives in a FIELD of the object that reads
// it. Section 4 of port/mg_fanout_costs.txt calls the field form the worse of
// the two, and the reason applies here in full: a link can never name any of
// this, because there is no table global to be unresolved.
//
// THE SPAWNINFO SITS INSIDE THE PAIR RUN, AND THAT IS SECTION 4's SWEEP HAZARD
// IN ITS PUREST FORM. 0x0213c98c is not a pair: it is {0x020ede18, 0x01820182},
// the factory word and the doubled id. It sits between the pair at 0x0213c984
// and the pair at 0x0213c994, so the run is 0x0213c95c..0x0213ca74 MINUS that
// one record. THIS LANE GOT IT WRONG FIRST TIME and the way it got it wrong is
// worth keeping: the SpawnInfo was derived first, and the pair scan was then
// started at the word AFTER it, which found 29 pairs and 14 code words and
// missed the six below. What corrected it was a link -- the three unresolved
// Pair symbols src/func_ov006_020eb8f0.cpp asks for include 0x0213c96c, which
// is below the SpawnInfo -- and the fifteenth code word, 0x020eb3e4, was
// installed by a pair no scan had looked at. Anchor a pair scan on the
// RELOCATION SET, not on an address the SpawnInfo happens to bound.
//
// THE THIRTY-FIVE PAIRS HOLD FIFTEEN DISTINCT CODE WORDS, and all fifteen are
// routed below. Read out of extracted/overlays/overlay_0006.bin at base
// 0x020bfec0, with the loading function named from the relocation's own source
// address:
//
//   pair        code      loaded by                pair        code      loaded by
//   0213c95c  020eb31c  020eb9dc                 0213c9fc  020eb0c8  020eb7f8
//   0213c964  020ed844  020ed8a4                 0213ca04  020eb31c  020ebb40
//   0213c96c  020ecb80  020eb8f0  (the sentinel) 0213ca0c  020ec93c  020ec9c0
//   0213c974  020eb3e4  020eb9dc                 0213ca14  020ec6e8  020ec84c
//   0213c97c  020eb31c  020eac38                 0213ca1c  020ecb80  020eb558
//   0213c984  020ecb80  020eb1e0                 0213ca24  020ec458  020ec4dc *
//   0213c994  020ed34c  020ecee4 (Render)        0213ca2c  020ed844  020ecee4
//   0213c99c  020eb1e0  020eb7f8                 0213ca34  020ed274  020ecee4
//   0213c9a4  020eb31c  020eb7f8                 0213ca3c  020ed328  020ecee4
//   0213c9ac  020ed494  020ecee4 (Render)        0213ca44  020ecb80  020eb3e4
//   0213c9b4  020ecb80  020eb7f8                 0213ca4c  020ecb80  020ed494
//   0213c9bc  020eb018  020eb8f0                 0213ca54  020ed270  020ed274
//   0213c9c4  020eb1e0  020eb8f0                 0213ca5c  020ed274  020ed300
//   0213c9cc  020ed494  020ed81c                 0213ca64  020ed328  020ed32c
//   0213c9d4  020eb0c8  020ebb40                 0213ca6c  020ed34c  020ed40c
//   0213c9dc  020ecb80  020ecba4                 0213ca74  020ecb80  020ed494
//   0213c9e4  020ec6e8  020eb9dc
//   0213c9ec  020ecb80  020eb9dc
//   0213c9f4  020ecb80  020eb9dc
//
//   * func_ov006_020ec4dc is one of this class's two FLOORS -- a config symbol
//     with no delink block and no src file. It is the sole loader of the pair
//     that installs 0x020ec458, so that state is installed by nothing the port
//     can run. The address is routed anyway, because the trap that stands for
//     the loader (unmatched/MgWiggler_Faces.cpp) is what fires instead, and a
//     routed address that is never asked for costs one switch arm.
//
// TWO OBJECTS, THREE FIELDS. The SCENE object holds its own state pair at
// +0x4660 and the six 020ed2xx..020ed8xx code words are its states. Each of the
// FIFTEEN wiggler sub-objects at scene+0x4678 (stride 0x98, and the factory's
// own __destroy_arr call says 0xf and 0x98 in as many words) holds a state pair
// at +0x10, a SECOND at +0x00 and a THIRD at +0x08 that
// src/func_ov006_020eb8f0.cpp copies to +0x10; the nine 020eb0xx..020ecbxx code
// words are theirs. 6 + 9 = 15, which is the whole set.
//
// ---- 2. FIVE DISPATCHERS, AND THE FIFTH IS THE THIRD SHAPE -----------------
//
// port/mg_fanout_costs.txt section 4 prescribes two detectors and lane FLW's
// amendment says to treat their union as a floor rather than a census. On this
// class the amendment is the whole story:
//
//   func_ov006_020ed18c  VTABLE SLOT 6, the Behavior. scene +0x4660.
//   func_ov006_020eb018  sub-object +0x10.
//   func_ov006_020eb0c8  sub-object +0x10.
//   func_ov006_020eb31c  sub-object +0x10, and it also COPIES the pair down
//                        from +0x10 to +0x00 as two plain ints.
//   func_ov006_020eb610  sub-object +0x00, OPEN-CODED IN PLAIN INTS inside
//                        extern "C". No member-pointer type, no PMF global.
//                        NEITHER PRESCRIBED DETECTOR SEES IT.
//
// The first four are what a `::*` source sweep returns. The fifth was found by
// a third detector this lane wrote and recommends: decode every word of ov006's
// text and look for `add Rd,Rn,Rm,asr #1` immediately followed by
// `ands Rm,Rm,#1`. That is the ARM Itanium sequence's own fingerprint and it
// cannot be spelled away by any source-level disguise. Over all of ov006 it
// finds 114 sites; five are in this class's block and they are exactly the five
// above. A companion SOURCE sweep for `>>1` beside `&1` also flagged
// src/func_ov006_020ecba4.c, which is a FALSE POSITIVE (ordinary 20.12 fixed-
// point arithmetic) -- and the ROM sweep is what says so, which is the argument
// for preferring it.
//
// SLOT 6 BEING ONE OF THE FIVE IS WHY THE RULE EXISTS. Lane PPP found the same
// shape in a vtable slot and wrote the rule as "before wiring slot 6 by name,
// read the src". This lane read it: src/func_ov006_020ed18c.cpp's one dispatch
// line is `(((C *)c)->*(*(PMF *)(c + 0x4660)))();`, a member-pointer TYPE, so
// the rulebook corollary puts it on the host-copy side and the seat wires the
// copy below rather than the src TU.
//
// ---- 3. THE STORED PAIR KEEPS THE ROM'S OWN WORDS --------------------------
//
// The ov085/ov100 shape -- write host addresses over the source pairs before
// anything reads them -- is WRONG for this class, and it is wrong for the exact
// reason section 4 gives for src/func_ov006_020c3b80.c. This class's slot 9
// Render (src/func_ov006_020ecee4.c) reads the scene object's stored pair AS TWO
// INTS and compares it BY VALUE against five of the .data pairs, five times, to
// decide what to draw:
//
//     Pair *p = (Pair *)(self + 0x4660);  Pair *g = &data_ov006_0213c9ac;
//     if (p->a == g->a) { if (p->b == g->b || *(int *)(self + 0x4660) == 0) ...
//
// A host address never equals a DS address, so a seat that rewrote the pair
// would leave all five tests permanently false while every dispatch still looked
// fine, and the class would draw the wrong thing with a clean census. Routing
// happens at the DISPATCH SITE only. Render is therefore an ordinary slice line
// and NOT a host copy: it names the pair symbols, but it reads them as two ints,
// which is the alias-safe half of the rulebook corollary.
//
// ---- 4. WHAT ONLY A RUN CAN SAY -------------------------------------------
//
// The census below counts the open-coded dispatcher separately, for the reason
// hal/scene_mg_bomroom.cpp gives: on a build that compiled the src TU instead,
// that count would read zero AND the run would fault with eip on a DS address.
// A nonzero count beside a clean run is the positive evidence. Anything this
// file does not own falls through to port_mg_call0, which prints the address on
// an UNHANDLED line, so a run with zero UNHANDLED is the evidence that fourteen
// is the whole set rather than the set this lane happened to find.

#include <cstdio>

/* The eight-byte mwcc member pointer in the only spelling that is true on both
   machines. Each host-copy TU in this port declares its own; neither this file
   nor MgMemory2_FieldPmf.cpp is a header. */
struct MgPmf { unsigned code; int adj; };

extern "C" {

/* the family chain's end: the null-code guard, the nonzero-adjustment refusal
   and the UNHANDLED report all live in exactly one place */
void port_mg_call0(void *self, unsigned code, int adj);

/* ---- the six SCENE-object states, +0x4660 ------------------------------- */
void func_ov006_020ed270(void);          /* 4 bytes in the ROM, `bx lr` */
void func_ov006_020ed274(char *c);
void func_ov006_020ed328(void);          /* 4 bytes in the ROM, `bx lr` */
void func_ov006_020ed34c(char *p);
void func_ov006_020ed494(char *c);
void func_ov006_020ed844(char *c);

/* ---- the nine WIGGLER sub-object states, +0x00, +0x08 and +0x10 --------- */
void func_ov006_020eb1e0(char *c);
void func_ov006_020eb3e4(char *c);
void func_ov006_020ec458(void *self);
void func_ov006_020ec6e8(char *c);
void func_ov006_020ec93c(void *self);
void func_ov006_020ecb80(void *a);
/* the three that are themselves host copies in this file, declared here
   because the switch reaches them by address like any other state */
void func_ov006_020eb018(char *c);
void func_ov006_020eb0c8(char *self);
void func_ov006_020eb31c(char *c);

/* ---- the ordinary callees the five host copies keep --------------------- */
void Math_Function_0203b0fc(int *p, int target, int scale, int max);
void func_0203d388(int *p, int angle);
int  func_02012468(int a, int b, int c, int d, int e, int f, int g, int h);
int  _Z15ApproachLinear2Rsss(short *v, short target, short step);
void func_ov006_020ec134(void *c);
void func_ov006_020ebf20(char *p);
void func_ov006_020eb9b0(char *p);
void func_ov006_020eb558(char *q);
void func_ov006_020ea71c(void);
void func_ov006_020ea3d0(char *p);
/* src/func_ov006_020eb31c.cpp spells this as the STATIC MEMBER Sound::
   PlayBank2_2D, which mangles as ?PlayBank2_2D@Sound@@SAXI@Z and reaches its
   definition only through the /alternatename row unmatched/MgMemory2_Faces.cpp
   carries. The host copy calls the C-named symbol directly instead -- the same
   choice unmatched/MgCoin_StateDispatch.cpp made -- so this file depends on no
   other seat's alias. */
void _ZN5Sound12PlayBank2_2DEj(unsigned int id);
extern unsigned short data_ov006_02141fcc;
extern int data_ov006_0213c958;

/* host-copied below and called from above their own definitions */
void func_ov006_020eb610(char *c);

}  /* extern "C" */

/* src/func_ov006_020eb0c8.cpp and src/func_ov006_020eb610.cpp both reach
   ApproachLinear through its C++ spelling; src/_Z14ApproachLinearRiii.cpp
   defines that overload and is already in the build (it is a wired slice line),
   so the host copies keep the declaration their src files use rather than
   acquiring an alias row. */
extern int ApproachLinear(int &ref, int target, int step);

// ---- the address switch ----------------------------------------------------

static unsigned g_wig_calls;        /* every dispatch that reached this file */
static unsigned g_wig_hits;         /* dispatches this file routed itself */
static unsigned g_wig_opencoded;    /* dispatches through func_ov006_020eb610 */
static unsigned g_wig_scene_hits;   /* of g_wig_hits, the scene object's own */

/* A PER-ADDRESS CENSUS, and it is not decoration. A total routed count and a
   zero UNHANDLED count together say "every address the class asked for was one
   this file owns"; neither says WHICH of the fifteen ran. The difference
   matters for a class with no state index: without this, "the state machine
   moved" would rest on an aggregate. Taken at the DISPATCH SITE, which is the
   shape lane L375 recommends over a counter inside a ROM body -- a census here
   survives its body being decompiled, or in this class's case being routed. */
static const unsigned kWigStates[] = {
    0x020ed270u, 0x020ed274u, 0x020ed328u, 0x020ed34cu, 0x020ed494u,
    0x020ed844u,
    0x020eb018u, 0x020eb0c8u, 0x020eb1e0u, 0x020eb31cu, 0x020eb3e4u,
    0x020ec458u, 0x020ec6e8u, 0x020ec93cu, 0x020ecb80u,
};
enum { kWigStateCount = sizeof kWigStates / sizeof kWigStates[0] };
static unsigned g_wig_state_hits[kWigStateCount];

static void wig_note(unsigned code)
{
    for (unsigned i = 0; i < (unsigned)kWigStateCount; ++i)
        if (kWigStates[i] == code) { ++g_wig_state_hits[i]; return; }
}

static int wig_try(void *self, unsigned code)
{
    char *c = (char *)self;
    wig_note(code);
    switch (code) {
    /* the scene object's own six, +0x4660 */
    case 0x020ed270u: func_ov006_020ed270();  ++g_wig_scene_hits; return 1;
    case 0x020ed274u: func_ov006_020ed274(c); ++g_wig_scene_hits; return 1;
    case 0x020ed328u: func_ov006_020ed328();  ++g_wig_scene_hits; return 1;
    case 0x020ed34cu: func_ov006_020ed34c(c); ++g_wig_scene_hits; return 1;
    case 0x020ed494u: func_ov006_020ed494(c); ++g_wig_scene_hits; return 1;
    case 0x020ed844u: func_ov006_020ed844(c); ++g_wig_scene_hits; return 1;
    /* the wiggler sub-object's nine, +0x00, +0x08 and +0x10 */
    case 0x020eb018u: func_ov006_020eb018(c); return 1;
    case 0x020eb0c8u: func_ov006_020eb0c8(c); return 1;
    case 0x020eb1e0u: func_ov006_020eb1e0(c); return 1;
    case 0x020eb31cu: func_ov006_020eb31c(c); return 1;
    case 0x020eb3e4u: func_ov006_020eb3e4(c); return 1;
    case 0x020ec458u: func_ov006_020ec458(c); return 1;
    case 0x020ec6e8u: func_ov006_020ec6e8(c); return 1;
    case 0x020ec93cu: func_ov006_020ec93c(c); return 1;
    case 0x020ecb80u: func_ov006_020ecb80(c); return 1;
    default:                                  return 0;
    }
}

extern "C" unsigned port_mg_wiggler_state_count(void)
{ return (unsigned)kWigStateCount; }

extern "C" unsigned port_mg_wiggler_state_addr(unsigned i)
{ return i < (unsigned)kWigStateCount ? kWigStates[i] : 0u; }

extern "C" unsigned port_mg_wiggler_state_hit(unsigned i)
{ return i < (unsigned)kWigStateCount ? g_wig_state_hits[i] : 0u; }

/* The one entry point for this class. Everything it does not own goes to the
   framework's chain, so the null guard, the nonzero-adjustment refusal and the
   UNHANDLED report stay in one place. */
extern "C" void port_mg_wiggler_call0(void *self, unsigned code, int adj)
{
    ++g_wig_calls;
    if (code != 0 && adj == 0 && wig_try(self, code)) {
        ++g_wig_hits;
        return;
    }
    port_mg_call0(self, code, adj);
}

extern "C" void port_mg_wiggler_counts(unsigned *calls, unsigned *hits,
                                       unsigned *scene, unsigned *opencoded)
{
    if (calls)     *calls     = g_wig_calls;
    if (hits)      *hits      = g_wig_hits;
    if (scene)     *scene     = g_wig_scene_hits;
    if (opencoded) *opencoded = g_wig_opencoded;
}

// ---- the five host copies --------------------------------------------------
//
// Each is its src TU verbatim except for the member-pointer declaration and the
// dispatch site. Where anything else moved it is stated on the line.

/* src/func_ov006_020ed18c.cpp -- VTABLE SLOT 6, the Behavior. Its `struct C`
   carried nothing but the member-pointer typedef, so replacing the dispatch
   removes the struct entirely and no layout moves. The halfword read-modify-
   write on data_ov006_02141fcc is kept at halfword width, which is both what
   the ROM does (ldrh / add #0x800 / strh at 0x020ed19c..0x020ed1ac) and what
   include/decl_common.h declares. */
// PORT_HOST_ABI: dScMgHanachan_c vtable slot 6 Behavior; src spells the dispatch as the member-pointer type (c->*(*(PMF*)(c+0x4660)))() that is eight bytes on the ROM and four on MSVC, so the host reads the {code, adj} pair and routes it.
extern "C" int func_ov006_020ed18c(char *c)
{
    data_ov006_02141fcc = (unsigned short)(data_ov006_02141fcc + 0x800);
    {
        const MgPmf *p = (const MgPmf *)(c + 0x4660);
        port_mg_wiggler_call0(c, p->code, p->adj);
    }
    int i = 0;
    if (data_ov006_0213c958 > 0) {
        char *p = c + 0x4678;
        do {
            func_ov006_020eb610(p);
            i++;
            p += 0x98;
        } while (i < data_ov006_0213c958);
    }
    int j = 0;
    if (data_ov006_0213c958 > 0) {
        char *q = c + 0x4678;
        do {
            func_ov006_020eb558(q);
            j++;
            q += 0x98;
        } while (j < data_ov006_0213c958);
    }
    func_ov006_020ea71c();
    func_ov006_020ea3d0(c + 0x4670);
    return 1;
}

/* src/func_ov006_020eb018.cpp. Its `struct C` is `char pad[0x10]; PMF pmf;`,
   so the member pointer is the LAST member and nothing in this TU reads past
   it -- every other access is a raw char* offset. The copy still spells the
   pair as two words, because the DISPATCH is wrong at four bytes whatever the
   layout does. The #pragma O3 the src carries is a mwcc directive and is
   dropped; it affects no host semantics.
   PORT_HOST_ABI: mwcc pointer-to-member wall, the field-embedded {code,adj}
   pair decoded through port_mg_wiggler_call0. Host-copied. */
extern "C" void func_ov006_020eb018(char *c)
{
    {
        const MgPmf *p = (const MgPmf *)(c + 0x10);
        port_mg_wiggler_call0(c, p->code, p->adj);
    }
    if (*(unsigned char *)(c + 0x95) != 5) return;
    *(int *)(((int)c + 0x60)) += 1;
    {
        int i = 0;
        int max = 0x200;
        int *p = (int *)(c + 0x4c);
        do {
            if (*(int *)(c + 0x60) > (5 - i) * 3)
                Math_Function_0203b0fc(p, 0x9800, 0x400, max);
            i++;
            max += 0x140;
            p++;
        } while (i < 5);
    }
    func_ov006_020ec134(c);
}

/* src/func_ov006_020eb0c8.cpp. Same `char pad10[0x10]; Pmf cb;` shape and the
   same reasoning; the dispatch sits in the else arm and stays there.
   PORT_HOST_ABI: mwcc pointer-to-member wall, the field-embedded {code,adj}
   pair decoded through port_mg_wiggler_call0. Host-copied. */
extern "C" void func_ov006_020eb0c8(char *self)
{
    if (_Z15ApproachLinear2Rsss((short *)(self + 0x90), 0, 1)) {
        *(unsigned char *)(self + 0x94) = 0;
        *(unsigned char *)(self + 0x93) = 1;
        *(short *)(self + 0x8e) = 0x21;
        *(int *)(self + 0x44) = 0x1000;
        *(short *)(self + 0x86) = 0x900;
        *(unsigned char *)(self + 0x95) = 6;
    } else {
        const MgPmf *p = (const MgPmf *)(self + 0x10);
        port_mg_wiggler_call0(self, p->code, p->adj);
        if (*(short *)(self + 0x90) == 0x30) {
            if (*(unsigned char *)(self + 0x95) != 4)
                *(unsigned char *)(self + 0x95) = 3;
        }
    }

    if (*(unsigned char *)(self + 0x95) != 6) return;

    int *cnt = (int *)(self + 0x60);
    *cnt = *cnt + 1;

    int i = 0;
    int scale = 0x200;
    int *q = (int *)(self + 0x4c);
    do {
        if (*(int *)(self + 0x60) > (5 - i) * 3) {
            Math_Function_0203b0fc(q, 0x9800, 0x400, scale);
        }
        scale += 0x140;
        q += 1;
        i++;
    } while (i < 5);

    func_ov006_020ec134(self);
}

/* src/func_ov006_020eb31c.cpp. THIS ONE ALSO COPIES THE PAIR, and the copy is
   why func_ov006_020eb610 below has anything to dispatch: it moves the two
   words from +0x10 down to +0x00, which is the sub-object's SECOND member-
   pointer field. The src already spells that copy as two ints
   (`*(int*)c = b ? a : a; *(int*)(c+4) = b;`, mwcc's own shape for a member-
   pointer assignment), so the copy is verbatim and eight bytes wide on both
   machines. Only the dispatch changed.
   PORT_HOST_ABI: mwcc pointer-to-member wall, the field-embedded {code,adj}
   pair decoded through port_mg_wiggler_call0. Host-copied. */
extern "C" void func_ov006_020eb31c(char *c)
{
    short *t = (short *)(((int)c + 0x90));
    short v = (short)(*t - 1);
    *t = v;
    if (*(short *)(c + 0x90) == 0) {
        *(unsigned char *)(c + 0x94) = 0;
        *(unsigned char *)(c + 0x93) = 1;
        *(int *)(c + 0x44) = 0x1000;
        *(short *)(c + 0x86) = 0x900;
        *(unsigned char *)(c + 0x95) = 4;
        *(short *)(c + 0x8e) = 0x78;
        {
            int a = *(int *)(c + 0x10);
            int b = *(int *)(c + 0x14);
            *(int *)c = b ? a : a;
            *(int *)(c + 4) = b;
        }
        return;
    }
    {
        const MgPmf *p = (const MgPmf *)(c + 0x10);
        port_mg_wiggler_call0(c, p->code, p->adj);
    }
    if (*(short *)(c + 0x90) != 0x30) return;
    if (*(int *)(c + 0x70) == 0)
        _ZN5Sound12PlayBank2_2DEj(0x1ea);
    *(unsigned char *)(c + 0x95) = 3;
}

/* src/func_ov006_020eb610.cpp -- THE THIRD SHAPE. The src open-codes the ARM
   Itanium sequence in plain ints inside extern "C":

       int off = *(int *)(c + 4);  char *base = c + (off >> 1);
       if (off & 1) fn = **(void(***)(char*))base + *(int*)c; else fn = *(...)c;
       fn(base);

   which is the ROM at 0x020eb724..0x020eb73c one for one. It compiles, it
   links, and both prescribed detectors read clean over it; only a run convicts
   it, and the first symptom would be eip on a raw DS address. THE NULL GUARD IS
   THE SRC'S OWN AND THE ROM'S: `cmp r2,#0 / beq` at 0x020eb71c, so this copy is
   not adding one. Everything above and below the decode is verbatim, including
   the L()/M() casts, which are mwcc codegen shapes and mean `*(int*)`.
   PORT_HOST_ABI: mwcc pointer-to-member wall, the open-coded ARM Itanium
   decode over a DS code address; the port's own address-switch answers the
   raw call. Host-copied. */
extern "C" void func_ov006_020eb610(char *c)
{
#define WIG_L(a) (*(int *)(((long long)(int)(a))))
#define WIG_M(a) (*(int *)(((long long)(unsigned)(a))))
    int v[2];
    int step;

    if (*(unsigned char *)(c + 0x92) != 0) {
        step = 0x1000;
        if (*(int *)(c + 0x40) < 0xa00) step = step >> 2;
        ApproachLinear(*(int *)(c + 0x40), *(int *)(c + 0x44), step);
        v[1] = -*(int *)(c + 0x40);
        v[0] = 0;
        func_0203d388(v, *(short *)(c + 0x7c));
        WIG_L(c + 0x20) += v[0];
        WIG_M(c + 0x24) += v[1];
        if (*(unsigned char *)(c + 0x93) != 0) {
            *(short *)(long long)(int)(c + 0x8e) =
                *(short *)(long long)(int)(c + 0x8e) - 1;
            if (*(short *)(c + 0x8e) == 0) {
                func_ov006_020eb9b0(c);
            }
        }
    }
    if (*(int *)(c + 0x6c) != 0 && *(int *)(c + 0x70) == 0) {
        int v93 = 0;
        if (*(unsigned char *)(c + 0x93) != 0) v93 = 0x1f4;
        *(int *)(c + 0x68) = func_02012468(*(int *)(c + 0x68), 2,
            *(int *)(c + 0x6c), 2, 0, v93, 0, 0);
    } else {
        *(int *)(c + 0x68) = 0;
    }
    /* THE ONE CHANGED CONSTRUCT. The src's own null test is a test of the CODE
       word, which is what it stays. */
    if (*(int *)c != 0) {
        const MgPmf *p = (const MgPmf *)c;
        ++g_wig_opencoded;
        port_mg_wiggler_call0(c, p->code, p->adj);
    }
    if (*(unsigned char *)(c + 0x92) == 0) return;
    func_ov006_020ebf20(c);
#undef WIG_L
#undef WIG_M
}
