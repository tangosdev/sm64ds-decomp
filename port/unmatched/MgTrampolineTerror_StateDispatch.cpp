// PORT_HOST_ABI.  dScMgTrampoline2_c's pointer-to-member wall, and it is the
// FIELD shape rather than the table shape.  Run mg11, lane TTE.
// actor id 0x181 = scene 385, "Trampoline Terror".
//
// Read port/slice_tte.txt for the identity derivation, the five width checks
// and the closure; read unmatched/MgBase_StateDispatch.cpp's header for the
// ROM's dispatch sequence and the framework entry points this file falls
// through to.  This file is the wall and nothing else.
//
// ---- 1. ONE DECODE SITE, AND THE ROM IS THE DETECTOR ----------------------
//
// port/mg_fanout_costs.txt section 4 says a link and a `::*` sweep are a floor
// rather than a census, because a TU that open-codes the Itanium sequence in
// plain ints defeats both.  So this class was measured the way run mg9 lane LKY
// measured its own: by scanning the OVERLAY IMAGE for the five-instruction ARM
// encoding
//
//     add Rd,Rn,Rm,asr #1     then     ands Rx,Ry,#1
//
// which is blind to spelling.  Over the whole of ov006 it returns 114 sites --
// the same 114 LKY reported, reproduced here from the image rather than cited.
// Over this class's OWN bodies, each taken at its own start and size from
// config/arm9/overlays/ov006/symbols.txt, it returns exactly ONE:
//
//     the 12 vtable overrides this class adds              1 site, 0x021233d4
//     the 13 it inherits from dScMgD3DBase_c               0 sites
//     the 4 more that base overrides and this class does not use
//                                (slots 16/17/24/25)       0 sites
//     the factory, the 5 state bodies, the 3 state writers 0 sites
//     the 2 bodiless floors (section 4)                    0 sites
//
// 40 bodies, one site, and it is vtable SLOT 6 -- section 14's practical rule
// ("read the slot-6 src before wiring it by name") firing again.
//
// ---- 2. THE PAIR IS A FIELD, NOT A TABLE ---------------------------------
//
// Out of extracted/overlays/overlay_0006.bin at base 0x020bfec0, the tail of
// func_ov006_02123340:
//
//     021233C8  ldr   r0,[pc,#0x54]     pool 0x02123424 = 0x00005004
//     021233CC  add   r3,r5,r0          r3 = this + 0x5004
//     021233D0  ldr   r1,[r3,#4]        the ADJUSTMENT word
//     021233D4  add   r0,r5,r1,asr #1
//     021233D8  ands  r1,r1,#1
//     021233DC  ldrne r2,[r0]
//     021233E0  ldrne r1,[r3]
//     021233E4  ldrne r1,[r2,r1]
//     021233E8  ldreq r1,[r3]
//     021233EC  blx   r1
//
// so the member pointer lives at THIS + 0x5004 and nowhere else.  That is the
// worse shape port/unmatched/MgMemory2_FieldPmf.cpp names: MSVC's
// single-inheritance member pointer is four bytes where the ROM's is eight, so
// a field spelled as a real `T::*` moves every field after it before any
// dispatch happens.  Here it is the LAST member src/func_ov006_02123340.cpp
// declares, so nothing moves and this one is a wrong-decode only -- but the
// decode is still wrong, and it is wrong silently.
//
// THE ROM HAS NO NULL GUARD AT THIS SITE, which is worth stating because this
// file adds one.  There is no `cmp r2,#0` anywhere in the ten instructions: the
// DS would branch to zero on a pair that was never written.  The port falls
// through to port_mg_call0, whose first statement is the framework's
// `if (code == 0) return;`.  That is a divergence in the SAFE direction and it
// is visible rather than hidden -- the call count below counts every dispatch
// and the routed count counts only the ones that landed, so a run in which the
// field is never seeded reads as a large call count and a zero routed count.
//
// ---- 3. THE FIVE PAIRS, AND WHY THEIR WRITERS ARE NOT HOST COPIES --------
//
// The pairs are five contiguous mwcc {code, adj} records in this class's own
// .data, every adjustment ZERO, read out of the overlay image:
//
//     0x0213fbd0  {0x02123b20, 0}      loaded by func_ov006_02123b24
//     0x0213fbd8  {0x02124088, 0}      loaded by func_ov006_02124228
//     0x0213fbe0  {0x02123b24, 0}      loaded by func_ov006_02123bf4
//     0x0213fbe8  {0x02123cb4, 0}      loaded by func_ov006_02124040
//     0x0213fbf0  {0x02123bf4, 0}      loaded by func_ov006_02123c78
//
// Each "loaded by" is a relocation, not a guess: config/arm9/overlays/ov006/
// relocs.txt carries from:0x02123bf0 to:0x0213fbd0, from:0x02124294 to:
// 0x0213fbd8, from:0x02123c74 to:0x0213fbe0, from:0x02124084 to:0x0213fbe8 and
// from:0x02123cb0 to:0x0213fbf0, and every one of those five sources is a
// literal pool inside the body named beside it.
//
// THE FIVE WRITERS NEED NO HOST COPY, and that is the run law's spelling rule
// rather than an oversight: "two-ints consumer = alias OK, PMF-typed consumer =
// host copy".  All five src TUs copy the pair as TWO PLAIN WORDS and none of
// them names a member-pointer type --
//
//     src/func_ov006_02123b24.c    two int loads, two int stores
//     src/func_ov006_02123bf4.cpp  *(double*)(c + 0x5004) = data_ov006_0213fbe0
//     src/func_ov006_02123c78.c    struct P2 { int a, b; } assignment
//     src/func_ov006_02124040.c    struct S2 { int a; int b; } assignment
//     src/func_ov006_02124228.c    struct P2 { int a, b; } assignment
//
// -- so all five are eight bytes on MSVC as well, they land at +0x5004 and
// +0x5008 exactly as the ROM does, and they stay ordinary slice lines.  The
// seat therefore routes at the DISPATCH SITE and never rewrites a stored pair,
// which is the same ruling MgMemory2_FieldPmf.cpp reaches for its by-value
// sentinel and port/mg_fanout_costs.txt section 4 reaches for the flower.
//
// THE MACHINE IS ONE LEVEL DEEP AND LINEAR, from the writers' own call graph:
//
//     slot 0  (func_ov006_021243ec) dispatches slot 18 through the object's
//             own vtable with mode -1  (`ldr r2,[r0]; ldr r2,[r2,#0x48]; blx r2`)
//       slot 18 (func_ov006_021242cc) tail-calls func_ov006_02124228
//         -> state 0x02124088   calls func_ov006_02124040
//              -> state 0x02123cb4   calls func_ov006_02123c78
//                   -> state 0x02123bf4  (its own writer)
//                        -> state 0x02123b24  (its own writer)
//                             -> state 0x02123b20, which is `bx lr`: the
//                                terminal idle state, four bytes in the ROM.
//
// Five states, five distinct code words, none of them a dispatcher itself (the
// section-1 scan is what says so), so there is no second level and no host copy
// beyond this one.
//
// ---- 4. THE TWO FLOORS WERE REAL; RUN mg12 LANE TRM RETIRED BOTH ---------
//
// This section is kept as the RECORD of what was trapped here in run mg11 and
// how it was established, because the four callees it names below are exactly
// the lines the retirement cost and a later reader needs that accounting. As of
// run mg12 both bodies have src files and both are slice_tte.txt lines:
// func_ov006_02123938 BYTE-MATCHES at 2004/b56 and is enrolled in ov006's
// delinks; func_ov006_02123428 is an honest NONMATCHING seat. What follows,
// down to the trap note, is how run mg11 found them:
//
//   func_ov006_02123938  0x1e8  called by slot 6 on EVERY tick.  Its own calls
//                        are func_ov004_020ae5c4 (the family's line rasteriser,
//                        seated by run mg10 lane F371 out of slice_mg1) plus
//                        the two sound helpers 0x020126e8 and 0x02012468.  So
//                        what this floor costs is a DRAW: the lines this scene
//                        stamps every frame are not stamped in the port.
//   func_ov006_02123428  0x314  called by state 0x02123cb4, the play state.
//                        Its callees are 0x0203b990 (three times), this class's
//                        own 0x021238d0 / 0x0212382c / 0x021237c8 / 0x0212373c,
//                        _Z14ApproachLinearRiii, 0x0203cc28 and 0x020cefa4.
//
// A trap-shaped floor HIDES its callees from static closure -- the run law's
// warning -- so the four own bodies 0x021238d0, 0x0212382c, 0x021237c8 and
// 0x0212373c were reachable ONLY through 02123428 and were deliberately NOT in
// port/slice_tte.txt: nothing would have called them and /OPT:REF would have
// dropped them anyway.  THE PREDICTION HELD EXACTLY: retiring 02123428 in run
// mg12 cost those four slice lines and no others.

#include <cstdio>
#include <cstdlib>

/* The eight-byte mwcc member pointer, in the only spelling that is true on both
   machines.  Each host-copy TU declares its own; neither is a header. */
struct MgPmf { unsigned code; int adj; };

extern "C" {

/* the framework's one entry point for a zero-argument state call */
void port_mg_call0(void *self, unsigned code, int adj);

/* ---- the five state bodies, section 3 ---------------------------------- */
void func_ov006_02123b20(void);          /* `bx lr`, the terminal idle state */
void func_ov006_02123b24(char *c);
void func_ov006_02123bf4(char *c);
void func_ov006_02123cb4(char *c);
void func_ov006_02124088(char *c);

/* ---- slot 6's other callees, spelled the way the ROM names them -------- */
void  func_ov006_02120c40(void);
void  func_ov006_020eef90(void);
void  func_ov006_02122ab8(void);
void  func_ov004_020adb1c(int self);
void *_ZN8Particle6System17NewUnkCallback818Ejj5Fix12IiES2_S2_PK11Vector3_16f(
          unsigned uniqueID, unsigned effect, int x, int y, int z,
          const void *pos);
void *_ZN8Particle6System12FromUniqueIDEj(unsigned uniqueID);
void  _Z14ApproachLinearRiii(int &v, int a, int b);

/* src/func_ov006_02123340.cpp declares this one OUTSIDE its extern "C" block,
   so MSVC mangles the reference as ?data_ov006_02140830@@3HA against the ov006
   mount's plain _data_ov006_02140830 -- the ordinary name-spelling defect this
   port carries /alternatename rows for.  The host copy declares it at C linkage
   instead, which is the same fix without an alias.  Same treatment
   unmatched/MgMemory2_FieldPmf.cpp gives data_ov006_0212b890. */
extern int data_ov006_02140830;

/* Formerly the two bodiless floors; both are decompiled and seated as of run
   mg12 lane TRM, so these are ordinary forward declarations of slice bodies
   (src/func_ov006_02123938.c and src/func_ov006_02123428.c) now. */
void func_ov006_02123938(void *self);
void func_ov006_02123428(char *c);

}  /* extern "C" */

// ---- the floors -------------------------------------------------------------

/* BOTH TRAPS ARE RETIRED.  Run mg12 lane TRM decompiled both bodies:
   src/func_ov006_02123938.c BYTE-MATCHES at 2004/b56 (link-verified, 0 blind,
   and enrolled in config/arm9/overlays/ov006/delinks.txt), and
   src/func_ov006_02123428.c is an honest NONMATCHING seat whose banner carries
   its three measured divergences.  Both are slice_tte.txt lines now, so a trap
   body here would be a duplicate definition rather than a stand-in.

   The accessor keeps its exact shape because hal/scene_mg_trampoline2.cpp reads
   it and that file was not touched for this; it can only report 0 now, and the
   census text says RETIRED rather than printing a want count that would read as
   "never called" when the truth is the opposite. */
extern "C" void port_mg_tte_floor_counts(unsigned *a, unsigned *b)
{
    if (a) *a = 0;              /* 02123938 RETIRED -- seated, byte-matched */
    if (b) *b = 0;              /* 02123428 RETIRED -- seated, NONMATCHING  */
}

// ---- the field switch -------------------------------------------------------
//
// The ordinal is the ROM PAIR ORDER, 0x0213fbd0 first, so the census can print
// a state INDEX progression for a class whose state is a code address and not
// an integer.  The linear chain of section 3 walks 1 -> 3 -> 4 -> 2 -> 0.

static unsigned g_tte_calls;      /* dispatches attempted at the site  */
static unsigned g_tte_routed;     /* dispatches that landed on a body  */
static unsigned g_tte_hits[5];
static int      g_tte_lo = -1, g_tte_hi = -1;
static int      g_tte_seq[64];
static int      g_tte_seq_n;
static int      g_tte_last = -1;

static void tte_note(int ord)
{
    ++g_tte_hits[ord];
    if (g_tte_lo < 0 || ord < g_tte_lo) g_tte_lo = ord;
    if (ord > g_tte_hi) g_tte_hi = ord;
    if (ord != g_tte_last) {
        g_tte_last = ord;
        if (g_tte_seq_n < (int)(sizeof g_tte_seq / sizeof g_tte_seq[0]))
            g_tte_seq[g_tte_seq_n++] = ord;
    }
}


/* ---- RUN link100 LANE PMFB7 GATE 3: THE FIVE PAIRS ARE SEATED -------------
 *
 * Section 3 above says the seat "routes at the DISPATCH SITE and never rewrites
 * a stored pair". That was the right ruling for the reason it gave -- a pair a
 * second reader compares BY VALUE must not be rewritten -- and the measurement
 * that supersedes it is that THIS class has no such reader.
 * dScMgTrampoline2_c's own vtable (0x0213fc7c) names its bodies, the
 * first-to-last span of them snapped to symbol boundaries is
 * 0x021225ac..0x021245a8, and every `load` relocation in ov006 landing there
 * whose word reads the target and whose FOLLOWING word is zero gives exactly
 * the FIVE pairs section 3 lists. Every reference into that run comes from
 * INSIDE the block and each is one writer's literal pool -- 0x02123bf0 in
 * func_ov006_02123b24, 0x02123c74 in _02123bf4, 0x02123cb0 in _02123c78,
 * 0x02124084 in _02124040, 0x02124294 in _02124228 -- five writers, five
 * records, no comparison anywhere.
 * (runs/link100/out/PMFB7/sweep_tramp2.txt)
 *
 * THE EMITTED SIDE, WITH /Zp4: mov eax,[esi+20484] / mov ecx,[esi+20488] /
 * add ecx,esi / call eax -- 0x5004 and 0x5008, the ROM's own offsets, ARITY
 * ZERO. WITHOUT it the same listing reads [esi+20488]/[esi+20492], four bytes
 * late, and those are the only four lines that differ. 0x5004 mod 8 is 4, which
 * is PMFB6's rule; the option is claimed on this ONE source.
 *
 * THE FIVE WRITERS DO NOT CHANGE. Section 3 measured that all five copy the
 * pair as two plain words and none names a member-pointer type, so all five are
 * eight bytes on MSVC too and land at +0x5004/+0x5008 exactly as the ROM does.
 * What they copy is now a host address instead of a DS one, which is the whole
 * change.
 *
 * WHAT THE CENSUS CAN STILL SEE: calls, routed and the five per-ordinal rows
 * are bumped by every face and stay exact, and the ordinal is still ROM pair
 * order 0x0213fbd0 first. The framework-routed arm is STRUCTURALLY ZERO after
 * the seat, because the field can only hold one of the five faces.
 */
#define TTE_FACE(tag, ord, call)                                          \
    static void __fastcall tte_f##tag(void *self, void *dead_edx)         \
    {                                                                     \
        (void)dead_edx;                                                   \
        ++g_tte_calls;                                                    \
        ++g_tte_routed;                                                   \
        tte_note(ord);                                                    \
        call;                                                             \
    }

TTE_FACE(02123b20, 0, func_ov006_02123b20())
TTE_FACE(02124088, 1, func_ov006_02124088((char *)self))
TTE_FACE(02123b24, 2, func_ov006_02123b24((char *)self))
TTE_FACE(02123cb4, 3, func_ov006_02123cb4((char *)self))
TTE_FACE(02123bf4, 4, func_ov006_02123bf4((char *)self))

extern "C" {
extern MgPmf data_ov006_0213fbd0, data_ov006_0213fbd8, data_ov006_0213fbe0,
    data_ov006_0213fbe8, data_ov006_0213fbf0;
}

namespace {
struct TteSeat { MgPmf *rec; unsigned rom; void *face; const char *what; };
const TteSeat g_tte_seats[] = {
    {&data_ov006_0213fbd0, 0x02123b20u, (void *)tte_f02123b20, "0213fbd0"},
    {&data_ov006_0213fbd8, 0x02124088u, (void *)tte_f02124088, "0213fbd8"},
    {&data_ov006_0213fbe0, 0x02123b24u, (void *)tte_f02123b24, "0213fbe0"},
    {&data_ov006_0213fbe8, 0x02123cb4u, (void *)tte_f02123cb4, "0213fbe8"},
    {&data_ov006_0213fbf0, 0x02123bf4u, (void *)tte_f02123bf4, "0213fbf0"},
};
}  /* namespace */

extern "C" void port_mg_tte2_field_seat(void)
{
    static int done;
    if (done)
        return;
    done = 1;
    for (unsigned i = 0; i < sizeof g_tte_seats / sizeof g_tte_seats[0]; ++i) {
        MgPmf *p = g_tte_seats[i].rec;
        if (p->code != g_tte_seats[i].rom || p->adj != 0) {
            std::fprintf(stderr, "FATAL: dScMgTrampoline2_c pair %s: the mount holds "
                         "%08x/%d, the cartridge's own record says %08x/0 -- "
                         "WRONG BYTES\n", g_tte_seats[i].what, p->code, p->adj,
                         g_tte_seats[i].rom);
            std::abort();
        }
        p->code = (unsigned)(size_t)g_tte_seats[i].face;
        p->adj = 0;
    }
}

/* THE CALL-TIME SWITCH IS DEAD AND SAYS SO -- the abort below is what a route
   by DS code word would hit, and reaching it means the seat did not run. */
static int tte_try(void *self, unsigned code)
{
    std::fprintf(stderr, "FATAL: dScMgTrampoline2_c field dispatch reached the call-time "
                 "switch with code %08x -- the seat did not run, or the pair "
                 "universe is not the one the sweep closed\n", code);
    (void)self;
    std::abort();
    return 0;
}
/* The entry point the host copy uses.  It tries this class's five addresses and
   hands everything else to the framework, so the null-code guard, the
   nonzero-adjustment refusal and the UNHANDLED report stay in exactly one
   place -- unmatched/MgBase_StateDispatch.cpp. */
extern "C" void port_mg_tte_call0(void *self, unsigned code, int adj)
{
    ++g_tte_calls;
    if (code != 0 && adj == 0 && tte_try(self, code)) {
        ++g_tte_routed;
        return;
    }
    port_mg_call0(self, code, adj);
}

extern "C" unsigned port_mg_tte_state_hits(void) { return g_tte_routed; }
extern "C" unsigned port_mg_tte_state_calls(void) { return g_tte_calls; }

extern "C" void port_mg_tte_index_range(int *lo, int *hi)
{
    if (lo) *lo = g_tte_lo;
    if (hi) *hi = g_tte_hi;
}

extern "C" void port_mg_tte_state_report(void)
{
    static const char *kName[5] = {
        "02123b20 idle(bx lr)", "02124088 open", "02123b24 close",
        "02123cb4 play", "02123bf4 result",
    };
    std::printf("[scene] dScMgTrampoline2_c state bodies entered:");
    for (int i = 0; i < 5; ++i)
        if (g_tte_hits[i]) std::printf(" %d=%s(x%u)", i, kName[i], g_tte_hits[i]);
    std::printf("\n[scene] dScMgTrampoline2_c state index order:");
    for (int i = 0; i < g_tte_seq_n; ++i) std::printf(" %d", g_tte_seq[i]);
    std::printf("   (ordinals are ROM pair order 0x0213fbd0..0x0213fbf0)\n");
}

// ---- the one host copy ------------------------------------------------------
//
// src/func_ov006_02123340.cpp verbatim except for two lines: `PMF cb` becomes
// `MgPmf cb` (eight bytes on both machines, and it is the last member so
// nothing else in the layout moves), and `(self->*(self->cb))()` becomes the
// routed call.  Every other statement, constant and offset is that file's.
// src/func_ov006_02123340.cpp is OUT of port/slice_tte.txt: listing it would be
// an LNK2005 against this definition.

struct TteObj {
    char  pad[0x5004];
    MgPmf cb;                 /* 0x5004, two words -- DELTA: was PMF */
};

/* HOST COPY RETIRED, run link100 lane PMFB7 gate 3.
   src/func_ov006_02123340.cpp dispatches its own field now, compiled with /Zp4
   so the member lands at the ROM's own 0x5004. The five records the class's own
   writers copy from hold zero-argument __fastcall faces. */
