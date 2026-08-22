// dScMgBomroom_c, the "Sort or 'Splode" minigame -- actor id 0x172, scene 370.
// Run mg6, lane SOS.
//
// Read port/slice_sos.txt for the identity derivation, the three width checks,
// the closure and the two floors. Read port/mg_fanout_costs.txt for what the
// family costs. This file is the seat: one face array, one fill, one factory
// forwarder and the run census.
//
// ---- 1. WHY THIS IS A SEPARATE FILE FROM hal/scene_mg.cpp -----------------
//
// The same reason hal/scene_mg_flower.cpp is one, and it is the reason the
// fan-out exists: scene_mg.cpp is the FAMILY half -- the mounts, the
// thirty-five overlay constructors, dScMgBase_c's twenty-eight address-keyed
// faces and the gate that keeps the constructors off a level boot. All of it
// is paid once and none of it is re-derived here. This file reaches it through
// the one seam that file exports, port_scene_mg_fill_shared(), and adds only
// what is this class's. Five lanes of one run appending five face arrays to
// one file is five merges of the same region.
//
// ---- 2. THE SHAPE FOLLOWED IS LANE CCN'S ---------------------------------
//
// Six seated classes stand as precedent and dScMgCoin_c (0x17a, lane CCN) is
// the closest: SIX override slots at exactly 0, 6, 9, 16, 17 and 18, five of
// them marker-carrying, no nosrc body in the vtable, and a TWO-DEEP hierarchy
// with no intermediate table. Everything about the fill, the face array's
// address keying, the two SM64DS_SCENE_SLOTn diagnostics, the raw-word census
// and the atexit report is that class's shape with this class's addresses.
// What is NOT CCN's is the file split, which is lane FLW's, and the state
// machine, which is bigger than any seated class's -- seven tables, seven
// dispatchers, thirty-three states, against curling's seven/five/twenty-five
// and dScMgCoin_c's five/seven/twenty-four.
//
// ---- 3. THE HIERARCHY IS TWO DEEP, AND THE ROM SAYS SO -------------------
//
//     Scene -> dScMgBase_c      data_ov004_020bc0c0  36 slots
//           -> dScMgBomroom_c   data_ov006_0213bbb4  36 slots
//
// Measured, not assumed from curling. Slot 16 (D2, func_ov006_020d5a54) and
// slot 17 (D0, func_ov006_020d5a78) BOTH store data_ov006_0213bbb4 into [this]
// and then call func_ov004_020b29c0, dScMgBase_c's teardown, with nothing in
// between -- read off the ROM disassembly at 0x020d5a54 and 0x020d5a78, whose
// literal pools hold 0x0213bbb4 and nothing else. So there is no intermediate
// base of the dScMgSingle3DBase_c kind 0x169 and 0x186 need, and this seat
// fills ONE derived table plus the shared base one.
//
// ---- 4. THE WIDTH IS 36, CHECKED THREE WAYS ------------------------------
//
//   1. SPAN. The next config symbol after data_ov006_0213bbb4 is
//      data_ov006_0213bc44, exactly 0x90 = 36 words on.
//   2. TERMINAL SLOT. Slot 35 holds 0x020ad660, the word every dScMgBase_c-
//      derived table holds there.
//   3. WHAT THE WORD PAST IT IS. Index 36 reads 0x00000001, an address in no
//      module -- the first word of data_ov006_0213bc44.
//
// A 37-slot fill here would have written a host thunk over that word, which is
// the failure port/mg_fanout_costs.txt section 11 exists to prevent.
//
// ---- 5. ALL THIRTY-SIX SLOTS, AND THE SIX THIS FILE OWNS ------------------
//
// A fresh word-for-word diff against dScMgBase_c's own table finds SIX slots
// differing and no others. "ruled" means the body was disassembled out of
// extracted/overlays/overlay_0006.bin at base 0x020bfec0 with
// port/tools/w13_dump.py and compared instruction for instruction with src/
// BEFORE it was seated; the per-body evidence is in
// port/tools/inferred_stub_adjudicated.txt.
//
//   slot  ROM word    body
//    0   020d9244    InitResources         ruled REAL_DECOMP
//    6   020d91b0    Behavior              ruled REAL_DECOMP, HOST COPY
//    9   020d9160    Render                ruled REAL_DECOMP
//   16   020d5a54    D2                    (no marker)
//   17   020d5a78    D0                    ruled REAL_DECOMP
//   18   020d9104    state reset           ruled REAL_DECOMP
//
// The other thirty are dScMgBase_c's and arm9's, and port_scene_mg_fill_shared
// writes every one of them, which is why this file's array is six rows long.
//
// ---- 6. THE STATE MACHINE, AND THE THING ONLY A RUN CAN SAY --------------
//
// SEVEN tables, THIRTY-THREE states, seven dispatching TUs, all host-copied in
// port/unmatched/MgBomroom_StateDispatch.cpp. SIX of the seven are named by a
// link or by a member-pointer source sweep. THE SEVENTH IS NOT: it open-codes
// the ARM Itanium sequence in plain ints, so it emits no unresolvable symbol
// and spells no member-pointer operator, and both prescribed detectors return
// clean over it.
//
// SO THE CENSUS BELOW COUNTS IT SEPARATELY. port_mg_bomroom_opencoded_calls()
// is how often func_ov006_020d8f98's host copy dispatched. On a build that
// still compiled the src TU that count would read zero AND the run would fault
// with eip on a DS address; a nonzero count beside a clean run is the positive
// evidence that the seventh copy is doing its job. A zero-dispatch clean run
// proves nothing about this class and the report says which zero it is.
//
// ---- 7. TWO FLOORS, BOTH DECOMP GAPS -------------------------------------
//
//   func_ov006_020d7c4c  0x230  STATE slot 5 of data_ov006_02141730. No delink
//                               block, no src. Counted at the dispatch site.
//   func_ov006_020d8408  0x4fc  the second call of state 2 of
//                               data_ov006_021416e0. No delink block, no src.
//                               A named trap in unmatched/MgBomroom_Traps.cpp.
//
// Both are reported below whether or not they fire, because a silent zero and
// an absent instrument look the same in a log.
//
// ---- 8. THE GAPLESS LATCH IS CALLED AND THE SPLICE IS NOT ----------------
//
// hal_gapless_minigames_latch() runs from slot 0 the way every seated
// minigame's does, because a scene that does not latch inherits the previous
// scene's answer and because the latch is what prints the honest "gap
// simulated" line for a scene the mod does not name.
//
// hal_gapless_splice() is deliberately NOT called. Scene 370 has no row in
// screen_gap.cpp's table, and gapless_row() returns 0 for every scene today
// (the mod is on hold pending the seam-band art), so a splice call would be a
// provable no-op now and an UNVERIFIED per-game gapless change the day the mod
// comes back. Gapless behaviour is decided per minigame, by playing it; this
// lane has not played it and does not get to vote. hal/scene_mg_flower.cpp
// makes the same call for the same reason.

#include "hal/screen_gap.h"

#include <cstdio>
#include <cstdlib>

extern "C" {

/* the seam into hal/scene_mg.cpp -- mounts, arm9 fill, dScMgBase_c's faces */
unsigned port_scene_mg_fill_shared(void **vt, unsigned n);
void     port_scene_mg_prepare(int id);
int      port_scene_env_want(void);
int      IsMinigameActorID(unsigned int id);

/* the framework's own dispatch census, from unmatched/MgBase_StateDispatch.cpp */
void port_mg_dispatch_counts(unsigned *calls, unsigned *unknown);

/* the mount storage the fill writes into. Both tables are inside their
   overlay's mount, so the fill replaces the mount's own words rather than
   building a fresh host array -- the ov007 and curling treatment, for the same
   reason: a second host array of the same name is a duplicate symbol, and
   leaving the mounted table alone leaves live wild DS pointers in a table the
   factory installs. */
extern unsigned char data_ov004_020bc0c0[];   /* dScMgBase_c,     36 slots */
extern unsigned char data_ov006_0213bbb4[];   /* dScMgBomroom_c,  36 slots */

/* the class's own six vtable bodies. func_ov006_020d91b0 is the HOST COPY in
   unmatched/MgBomroom_StateDispatch.cpp, not the src TU: it is the
   pointer-to-member dispatcher and the port cannot compile the src. */
int   func_ov006_020d9244(void *self);          /* slot 0  InitResources */
int   func_ov006_020d91b0(char *self);          /* slot 6  Behavior, host copy */
int   func_ov006_020d9160(void *self);          /* slot 9  Render */
int   func_ov006_020d5a54(int *self);           /* slot 16 D2 */
int  *func_ov006_020d5a78(int *self);           /* slot 17 D0 */
void  func_ov006_020d9104(unsigned char *self); /* slot 18 state reset */

/* the factory */
void *MgSortOrSplode_Spawn(void);

/* the state machine's witnesses, from unmatched/MgBomroom_StateDispatch.cpp */
unsigned port_mg_bomroom_state_hits(void);
unsigned port_mg_bomroom_floor_hits(void);
unsigned port_mg_bomroom_opencoded_calls(void);
/* the undecompiled callee's witness, from unmatched/MgBomroom_Traps.cpp */
unsigned port_mg_bomroom_trap_hits(void);

}  /* extern "C" */

// ---- the tick witness ------------------------------------------------------
//
// One counter per dispatched slot, the instrument every scene seat in this
// port carries, for the reason hal/scene_mg.cpp gives: an object that EXISTS
// and an object that RUNS look identical from outside. These count only the
// six slots THIS file's faces own; the framework slots dScMgBase_c keeps are
// counted by scene_mg.cpp's own array and printed by its atexit report, which
// a scene-370 boot also registers because 370 is an id IsMinigameActorID
// accepts.
static unsigned g_sos_hits[36];

#define SOS(n)  (++g_sos_hits[(n)])

static int  __fastcall sos_init(void *s, void *)
{ SOS(0);  const int r = func_ov006_020d9244(s);
  hal_gapless_minigames_latch(); return r; }
static int  __fastcall sos_beh(void *s, void *)
{ SOS(6);  return func_ov006_020d91b0((char *)s); }
static int  __fastcall sos_render(void *s, void *)
{ SOS(9);  return func_ov006_020d9160(s); }
static void *__fastcall sos_d2(void *s, void *)
{ SOS(16); return (void *)(size_t)func_ov006_020d5a54((int *)s); }
static void *__fastcall sos_d0(void *s, void *)
{ SOS(17); return (void *)func_ov006_020d5a78((int *)s); }
/* THE RIDE-THROUGH IS LOAD-BEARING. Every slot-18 dispatch in both overlay
   images passes one argument (lane BASESET's 22-site census,
   runs/mg5/out/baseset/slot18_19_scan.txt), and the ROM caller is a __thiscall
   one that cleans nothing. A thunk declared (void*, void*) compiles to a bare
   ret, leaks those four bytes, and the caller's own epilogue then takes a
   garbage return address. The parameter exists so __fastcall cleans four
   bytes; the ROM body ignores its r1 and is called without it. */
static int  __fastcall sos_reset(void *s, void *, int /*ridethrough*/)
{ SOS(18); func_ov006_020d9104((unsigned char *)s); return 1; }

/* SM64DS_SCENE_SLOT0=0 and SM64DS_SCENE_SLOT9=0, the diagnostics the ov003,
   ov007, curling and flower seats all carry, counted separately so a run can
   never read a no-op as the real body having run. */
static unsigned g_sos_init_skipped, g_sos_render_skipped;
static int __fastcall sos_init_noop(void *, void *)
{ ++g_sos_init_skipped; return 1; }
static int __fastcall sos_render_noop(void *, void *)
{ ++g_sos_render_skipped; return 1; }

struct SosFace { unsigned ds; void *host; };

/* dScMgBomroom_c's own six, the per-class half. Keyed on the ROM WORD each
   slot holds, exactly like kCurlingFaces and kCoinFaces, so the array is
   order-independent and cannot land on a slot the ROM did not park that body
   in. */
static const SosFace kBomroomFaces[] = {
    {0x020d9244u, (void *)sos_init},   {0x020d91b0u, (void *)sos_beh},
    {0x020d9160u, (void *)sos_render}, {0x020d5a54u, (void *)sos_d2},
    {0x020d5a78u, (void *)sos_d0},     {0x020d9104u, (void *)sos_reset},
};

static unsigned sos_apply(void **vt, unsigned n, const SosFace *f, unsigned nf)
{
    unsigned hit = 0;
    for (unsigned i = 0; i < n; ++i) {
        const unsigned ds = (unsigned)(size_t)vt[i];
        for (unsigned k = 0; k < nf; ++k)
            if (f[k].ds == ds) { vt[i] = f[k].host; ++hit; break; }
    }
    return hit;
}

/* Words still holding a DS address. A minigame table is 36 slots and every one
   of them is dispatched by something, so a nonzero answer is a wild call
   waiting to happen and the seat says so out loud rather than booting. */
static unsigned sos_raw_left(void **vt, unsigned n)
{
    unsigned left = 0;
    for (unsigned i = 0; i < n; ++i) {
        const unsigned w = (unsigned)(size_t)vt[i];
        if (w >= 0x02000000u && w < 0x02400000u)
            ++left;
    }
    return left;
}

extern "C" void port_scene_bomroom_hits(void);

extern "C" void port_scene_fill_bomroom(void)
{
    void **base = (void **)data_ov004_020bc0c0;
    void **vt   = (void **)data_ov006_0213bbb4;

    /* THE BASE TABLE IS FILLED HERE TOO AND IT IS NOT CEREMONY, for the reason
       hal/scene_mg_flower.cpp's fill states: on a tree that also carries the
       curling row this is a second pass over words that are already host
       pointers and finds nothing, because the fill keys on a DS word and there
       are none left. It is here so this class does not depend on another
       class's registry row existing -- the factory's first act is
       func_ov004_020b2adc, which writes data_ov004_020bc0c0 into the object's
       first word before the derived table lands. */
    port_scene_mg_fill_shared(base, 36);

    /* THE DERIVED TABLE: shared arm9 words and dScMgBase_c's twenty-eight
       first, then this class's six. The two key sets are disjoint by
       construction, since a word is one address. */
    port_scene_mg_fill_shared(vt, 36);
    sos_apply(vt, 36, kBomroomFaces,
              sizeof kBomroomFaces / sizeof kBomroomFaces[0]);

    /* the two diagnostics, applied after the fill so they override it */
    {
        const char *s0 = std::getenv("SM64DS_SCENE_SLOT0");
        const char *s9 = std::getenv("SM64DS_SCENE_SLOT9");
        if (s0 && s0[0] == '0') vt[0] = (void *)sos_init_noop;
        if (s9 && s9[0] == '0') vt[9] = (void *)sos_render_noop;
    }

    {
        const unsigned lb = sos_raw_left(base, 36);
        const unsigned lv = sos_raw_left(vt, 36);
        if (lb || lv) {
            std::fprintf(stderr, "  [scene] MINIGAME FILL INCOMPLETE: "
                         "dScMgBase_c leaves %u of 36 raw DS words, "
                         "dScMgBomroom_c %u. A dispatch of any of them jumps "
                         "to a DS address as a host one.\n", lb, lv);
            std::fflush(stderr);
        }
    }

    /* The constructors, gated on the requested id, exactly as the curling and
       flower fills do it. Idempotent behind scene_mg.cpp's own once-per-process
       guard, so on a boot that carries several minigame rows this is a no-op
       and on a tree that ever drops the curling row it is still correct. */
    port_scene_mg_prepare(port_scene_env_want());

    if (port_scene_env_want() == 370) {
        static int armed;
        if (!armed) {
            armed = 1;
            std::atexit(port_scene_bomroom_hits);
        }
    }
}

/* The registry's factory column is void *(*)(void) and so is the matched
   factory, but the forwarder is kept anyway for the reason title_spawn,
   port_mg_curling_spawn and port_mg_flower_spawn are: it gives the seat one
   place to observe the object without the registry table growing a second
   column.

   THE FACTORY ITSELF NEEDS NO DISPLACEMENT RULING, which is worth recording
   because 0x169's did. src/MgSortOrSplode_Spawn.c calls
   func_ov004_020b2adc(p) WITH its argument, where src/func_ov006_020e0574.cpp
   calls the same base constructor with none and rides r0 through. That callee
   dereferences on its first statement and then writes three vtable words
   through the pointer, so the difference is a wild write versus a correct one.
   This factory is on the correct side of it and is linked from
   port/slice_sos.txt rather than host-copied. Verified against the ROM at
   0x020d9574 (0x30 bytes) during this lane's adjudication pass. */
static char *g_sos_self;

extern "C" void *port_mg_bomroom_spawn(void)
{
    void *p = MgSortOrSplode_Spawn();
    g_sos_self = (char *)p;
    return p;
}

/* This class's own witness. hal/scene_boot.cpp's end-of-run block still has no
   branch for a fifth minigame and is still not this lane's region to widen, so
   the same std::atexit shape port_scene_fill_curling and port_scene_fill_coin
   use is used here. Registered only when scene 370 is the requested one, so a
   curling or level run prints nothing extra. */
extern "C" void port_scene_bomroom_hits(void)
{
    std::printf("[scene] dScMgBomroom_c slot hits: init %u, behavior %u, "
                "render %u, D2 %u, D0 %u, state-reset %u\n",
                g_sos_hits[0], g_sos_hits[6], g_sos_hits[9], g_sos_hits[16],
                g_sos_hits[17], g_sos_hits[18]);
    if (g_sos_init_skipped || g_sos_render_skipped)
        std::printf("[scene] dScMgBomroom_c diagnostics: slot 0 skipped %u "
                    "time(s), slot 9 skipped %u time(s) -- the counts above "
                    "are NOT the real bodies\n",
                    g_sos_init_skipped, g_sos_render_skipped);
    {
        unsigned calls = 0, unknown = 0;
        port_mg_dispatch_counts(&calls, &unknown);
        std::printf("[scene] dScMgBomroom_c state dispatch: %u routed to a "
                    "dScMgBomroom_c state across its SEVEN tables, %u "
                    "framework call(s), %u UNHANDLED address(es)\n",
                    port_mg_bomroom_state_hits(), calls, unknown);
    }
    /* THE THING ONLY A RUN CAN SAY. func_ov006_020d8f98 is the dispatcher that
       open-codes the ARM Itanium sequence in plain ints: no unresolvable
       symbol, no member-pointer spelling, so neither the link nor a source
       sweep can see it and only this count convicts it. Printed whether or not
       it fired, because a zero here means the class never entered the state
       that dispatches it and NOT that the shape is absent. */
    std::printf("[scene] dScMgBomroom_c open-coded dispatcher "
                "(func_ov006_020d8f98, table data_ov006_021416a0): %u "
                "dispatch(es) through the host decode\n",
                port_mg_bomroom_opencoded_calls());
    /* THE TWO FLOORS, REPORTED WHETHER OR NOT THEY FIRED. Both are decomp gaps
       and neither is a port one; a zero has to say WHICH zero it is. */
    std::printf("[scene] dScMgBomroom_c floors: state 0x020d7c4c (slot 5 of "
                "data_ov006_02141730, no delink block and no src) wanted %u "
                "time(s); callee 0x020d8408 (0x4fc, called by state 2 of "
                "data_ov006_021416e0) entered %u time(s)\n",
                port_mg_bomroom_floor_hits(), port_mg_bomroom_trap_hits());
    if (g_sos_self)
        std::printf("[scene] dScMgBomroom_c object at %p, state index %d, "
                    "sub-state index %d\n", (void *)g_sos_self,
                    *(int *)(g_sos_self + 0x62d0),
                    *(int *)(g_sos_self + 0x62d4));
    std::fflush(stdout);
}
