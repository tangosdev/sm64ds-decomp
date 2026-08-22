// dScMgCurling2_c, actor id 0x177, scene 375 -- the ADVANCED half of the
// curling pair whose basic half is dScMgCurling_c / scene 374 (Shuffle Shell).
// Run mg6, lane S75. The seventh ov006 minigame class the port seats.
//
// Read port/slice_s75.txt for the identity derivation, the three width checks,
// the seven-table state machine and the closure. Read
// port/mg_fanout_costs.txt for what the family costs. This file is the seat:
// six faces, the fill, the factory forwarder, the witness and two aliases.
//
// ---- 1. WHY THIS IS A SEPARATE FILE FROM hal/scene_mg.cpp -----------------
//
// hal/scene_mg_flower.cpp section 1's ruling, unchanged and not re-argued:
// scene_mg.cpp is the FAMILY half and exports exactly one seam,
// port_scene_mg_fill_shared(). Copying kMgBaseFaces into a second translation
// unit would install a second set of host thunks over the same DS words and
// split the per-slot witness in two, so this file reaches the family through
// the seam and adds only what is this class's.
//
// ---- 2. THE HIERARCHY IS TWO DEEP AND THE ROM SAYS SO --------------------
//
//     Scene -> dScMgBase_c (data_ov004_020bc0c0) -> dScMgCurling2_c (0213c510)
//
// The word before slot 0 is an mwcc typeinfo record, {0x0209a764 the
// __si_class_type_info vptr, 0x0213c4c8 the name, 0x020bbf6c the base}, and
// 0x0213c4c8 reads "15dScMgCurling2_c". Curling's record at 0x0213c2c4 has the
// identical shape, names "14dScMgCurling_c", and carries THE SAME base word,
// so the two classes are siblings under dScMgBase_c and there is no
// intermediate table to fill. That is the difference between this seat and
// dScMgFlower_c's and dScMgCup_c's, and it is why only ONE face array appears
// below.
//
// The factory confirms it from the other side. func_ov006_020e6bf4 calls
// _ZN9ActorBasenwEj(21956), then func_ov004_020b2adc -- dScMgBase_c's
// constructor, WITH the object in r0, which src also spells with the argument,
// so this class does NOT need the factory host copy dScMgCup_c does -- then
// writes 0x0213c510 into the object's first word and nothing else. One vptr
// store, one base, one table.
//
// ---- 3. THE WIDTH IS 36, AND THE NEIGHBOUR CHECK MATTERS HERE MOST -------
//
//   1. SPAN     data_ov006_0213c510 -> data_ov006_0213c5a0, exactly 36 words.
//   2. SLOT 35  0x020ad660, the family's terminal.
//   3. WORD 36  0x2f474d2f. Every other lane records this as "ASCII /MG/, a
//               file path". Here it is this class's OWN path string: slot 0
//               passes &data_ov006_0213c5a0 to func_020adc74 and the bytes
//               read "/MG/d_2d_mg_bg_jokyu_curling_ncg.bin". A 37-slot fill
//               would overwrite the first four characters of the filename the
//               scene opens on its first frame.
//
// THIS TABLE IS THE CLOSEST ANY SEATED CLASS SITS TO CURLING'S LIVE DATA, so
// port/mg_fanout_costs.txt section 11's hazard was measured in both directions
// rather than assumed:
//
//   curling's vtable        0x0213c304
//   curling's 25 pairs      0x0213c1e4 .. 0x0213c2bc
//   THIS class's 25 pairs   0x0213c3d4 .. 0x0213c4b4
//   THIS class's vtable     0x0213c510 .. 0x0213c59c
//
// The fill starts 0x254 bytes above the top of curling's pair span and 0x5c
// bytes above the top of THIS class's own, so 36 words from 0x0213c510 reach
// neither. Nothing this file writes can corrupt scene 374, and the canary run
// in the lane report is the check rather than this paragraph.
//
// ---- 4. THE ROW GOES LAST -------------------------------------------------
//
// port_scene_registry_install walks port_scene_classes[] in table order and
// calls every row's fill on every boot; port_scene_mg_overlay_load runs the
// thirty-five constructors once per process from the tail of the FIRST
// minigame row's fill. Appending after every existing row means the
// constructors have already run with clean ROM words when this fill starts,
// which is the latent-safe direction section 11 names. It is a rule this lane
// obeys rather than a hazard it relies on: section 3 above measures that this
// fill writes outside nobody's data at any width.
//
// ---- 5. THIS CLASS HAS THE WALL, IN ALL THREE OF ITS FORMS ---------------
//
// dScMgFlower_c's seat records "WHAT THIS CLASS DOES NOT HAVE: A
// POINTER-TO-MEMBER WALL". This one has the opposite. FIVE dispatching TUs,
// SEVEN tables, twenty-five states, and between them they cover every hiding
// place port/mg_fanout_costs.txt section 4 and lane FLW enumerate: two TUs the
// link names, two hidden behind extern "C", and one -- vtable slot 6 itself --
// that open-codes the ARM Itanium sequence in plain ints and is invisible to
// both the link and a `::*` sweep. All five are host-copied in
// port/unmatched/MgCurling2_StateDispatch.cpp, which carries the twenty-five
// addresses, the per-table arities read off the ROM, and the one hard floor.
//
// SO NO STATIC EVIDENCE FROM THIS LANE LICENSES THE WORD "NONE". The dispatch
// census printed below is the detector, and a run with slot hits and zero
// dispatch calls has ticked the object without entering its state machine.
//
// ---- 6. THE FLOORS ARE RETIRED -------------------------------------------
//
// Run mg6 left three, all the same class of thing: a config symbol, no delink
// block in config/arm9/overlays/ov006/delinks.txt, no src file anywhere. Run
// mg7 lane L375 decompiled all three out of
// extracted/overlays/overlay_0006.bin at base 0x020bfec0, which is the decomp
// transcription lanes CT1 and CUR2 did for curling and not a port job:
//
//   func_ov006_020e4bd4   0x300  slot 1 of state table data_ov006_02141978, a
//                                LIVE state -- the stylus aim step. SIZE EXACT
//                                192/192 words; register colouring only.
//   func_ov006_020e513c   0x314  overlap depenetration over the eleven records
//   func_ov006_020e5450   0x560  the shell-vs-shell elastic collision, asked
//                                for 6810 times in the mg6 1200-frame run
//
// All three ship NONMATCHING-bannered. None is an invented body:
// port/tools/inferred_stub_guard.py exists to refuse exactly that, and each
// file's header states the ROM read it was transcribed from.
//
// AND THE VENEER LANDMINE mg6 recorded is defused. src/func_ov006_020e5b70.c is
// the image's `ldr ip,[pc]; bx ip; .word 0x020e5450` tail jump; it declared the
// callee `(void)` and dropped both arguments, which cost nothing only while
// 0x020e5450 was a trap. It now takes (char *, int) and forwards them, and
// mwccarm 1.2/sp2p3 emits the same three words either way, so no host copy of
// the veneer was needed. The dispatch site in MgCurling2_StateDispatch.cpp was
// dropping them too and is fixed with it.

#include "hal/screen_gap.h"

#include <cstdio>
#include <cstdlib>

extern "C" {

/* the seam into hal/scene_mg.cpp -- mounts, arm9 fill, dScMgBase_c's faces */
unsigned port_scene_mg_fill_shared(void **vt, unsigned n);
void     port_scene_mg_prepare(int id);
int      port_scene_env_want(void);
int      IsMinigameActorID(unsigned int id);

/* the mount storage the fill writes into. Inside the ov006 mount, so the fill
   replaces the mount's own words rather than building a fresh host array --
   the ov007, curling and flower treatment, for the same reason: a second host
   array of the same name is a duplicate symbol, and leaving the mounted table
   alone would leave live wild DS pointers in the table the factory installs. */
extern unsigned char data_ov006_0213c510[];   /* dScMgCurling2_c, 36 slots */
extern unsigned char data_ov004_020bc0c0[];   /* dScMgBase_c,     36 slots */
extern unsigned char data_ov006_0213c434[];   /* the SpawnInfo record      */

/* the class's six overrides */
int   func_ov006_020e6894(char *c);           /* slot  0 InitResources */
int   func_ov006_020e683c(char *c);           /* slot  6 Behavior      */
int   func_ov006_020e67f0(int c);             /* slot  9 Render        */
int   func_ov006_020e3854(int *c);            /* slot 16 D2            */
int  *func_ov006_020e3878(int *c);            /* slot 17 D0            */
void  func_ov006_020e6774(unsigned char *c);  /* slot 18 state reset   */

/* the factory */
int  *func_ov006_020e6bf4(void);

/* the state machine's own census, from unmatched/MgCurling2_StateDispatch.cpp */
unsigned port_mg_curling2_state_calls(void);
unsigned port_mg_curling2_state_unknown(void);
unsigned port_mg_curling2_state_floor(void);

/* the round-end machine's per-slot census, same file: table data_ov006_021419d8
   slot 0..3 = 020e5e3c / 020e5b7c / 020e5b70 / 020e5a0c, and the state that used
   to be reported as a floor */
unsigned port_mg_curling2_d8_hits(int slot);
unsigned port_mg_curling2_state_4bd4(void);

void port_scene_curling2_hits(void);

}  /* extern "C" */

// ---- the tick witness ------------------------------------------------------
//
// One counter per dispatched slot, the instrument every scene seat in this port
// carries, for hal/scene_mg.cpp's reason: an object that EXISTS and an object
// that RUNS look identical from outside. These count only the slots THIS file's
// faces own; the framework slots dScMgBase_c keeps are counted by scene_mg.cpp's
// own array and printed by its atexit report, which a scene-375 boot also
// registers because 375 is an id IsMinigameActorID accepts.
static unsigned g_c2_hits[36];

#define C2(n)  (++g_c2_hits[(n)])

static int __fastcall c2_init(void *s, void *)
{ C2(0);  const int r = func_ov006_020e6894((char *)s);
  /* the GaplessMinigames latch, for hal/scene_mg.cpp's reason: every seated
     minigame calls it so the ones the gapless table does not name can say
     "unsupported" instead of doing nothing quietly. */
  hal_gapless_minigames_latch(); return r; }
static int __fastcall c2_beh(void *s, void *)
{ C2(6);  return func_ov006_020e683c((char *)s); }
static int __fastcall c2_render(void *s, void *)
{ C2(9);  return func_ov006_020e67f0((int)(size_t)s); }
static void *__fastcall c2_d2(void *s, void *)
{ C2(16); return (void *)(size_t)func_ov006_020e3854((int *)s); }
static void *__fastcall c2_d0(void *s, void *)
{ C2(17); return (void *)func_ov006_020e3878((int *)s); }
/* THE RIDE-THROUGH IS LOAD-BEARING and is not optional on any slot-18 thunk.
   Every slot-18 dispatch site in both overlay images passes one argument (lane
   BASESET's 22-site census, runs/mg5/out/baseset/slot18_19_scan.txt), and the
   round teardown's dispatcher is a __thiscall caller that cleans nothing. A
   thunk declared (void*, void*) compiles to a bare ret, leaks those four bytes,
   and the dispatcher's own `pop ebp; ret` then takes its saved frame pointer as
   a return address. The parameter exists so __fastcall cleans four bytes; the
   ROM body ignores its r1 and is called without it. */
static int __fastcall c2_reset(void *s, void *, int /*ridethrough*/)
{ C2(18); func_ov006_020e6774((unsigned char *)s); return 1; }

/* SM64DS_SCENE_SLOT0=0 and SM64DS_SCENE_SLOT9=0, the diagnostics the ov003,
   ov007, curling and flower seats all carry, counted separately so a run can
   never read a no-op as the real body having run. */
static unsigned g_c2_init_skipped, g_c2_render_skipped;
static int __fastcall c2_init_noop(void *, void *)
{ ++g_c2_init_skipped; return 1; }
static int __fastcall c2_render_noop(void *, void *)
{ ++g_c2_render_skipped; return 1; }

struct C2Face { unsigned ds; void *host; };

static const C2Face kCurling2Faces[] = {
    {0x020e6894u, (void *)c2_init},   {0x020e683cu, (void *)c2_beh},
    {0x020e67f0u, (void *)c2_render}, {0x020e3854u, (void *)c2_d2},
    {0x020e3878u, (void *)c2_d0},     {0x020e6774u, (void *)c2_reset},
};

static unsigned c2_apply(void **vt, unsigned n, const C2Face *f, unsigned nf)
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
static unsigned c2_raw_left(void **vt, unsigned n)
{
    unsigned left = 0;
    for (unsigned i = 0; i < n; ++i) {
        const unsigned w = (unsigned)(size_t)vt[i];
        if (w >= 0x02000000u && w < 0x02400000u)
            ++left;
    }
    return left;
}

extern "C" void port_scene_fill_curling2(void)
{
    void **base = (void **)data_ov004_020bc0c0;
    void **vt   = (void **)data_ov006_0213c510;

    /* THE BASE TABLE IS FILLED HERE TOO AND IT IS NOT CEREMONY, for
       hal/scene_mg_flower.cpp's reason: on a tree that carries the curling row
       this is a second pass over words that are already host pointers and finds
       nothing, and it is here so this class does not depend on another class's
       row existing. The factory's first act is func_ov004_020b2adc, which
       writes data_ov004_020bc0c0 into the object's first word before the
       derived table lands, and thirty-six raw DS words in a table the ROM
       installs is what produced the ov007 lane's wild-execute fault. */
    port_scene_mg_fill_shared(base, 36);

    /* THE DERIVED TABLE. Shared arm9 and dScMgBase_c words first, then this
       class's six. The two key sets are disjoint because a word is one
       address. */
    port_scene_mg_fill_shared(vt, 36);
    c2_apply(vt, 36, kCurling2Faces,
             sizeof kCurling2Faces / sizeof kCurling2Faces[0]);

    /* the two diagnostics, applied after the fill so they override it */
    {
        const char *s0 = std::getenv("SM64DS_SCENE_SLOT0");
        const char *s9 = std::getenv("SM64DS_SCENE_SLOT9");
        if (s0 && s0[0] == '0') vt[0] = (void *)c2_init_noop;
        if (s9 && s9[0] == '0') vt[9] = (void *)c2_render_noop;
    }

    {
        const unsigned lb = c2_raw_left(base, 36);
        const unsigned lv = c2_raw_left(vt, 36);
        if (lb || lv) {
            std::fprintf(stderr, "  [scene] CURLING2 FILL INCOMPLETE: "
                         "dScMgBase_c leaves %u of 36 raw DS words, "
                         "dScMgCurling2_c %u. A dispatch of any of them jumps "
                         "to a DS address as a host one.\n", lb, lv);
            std::fflush(stderr);
        }
    }

    /* The constructors, gated on the requested id, exactly as the curling and
       flower fills do it. Idempotent behind scene_mg.cpp's own once-per-process
       guard, so on a boot that carries the other rows this is a no-op and on a
       tree that ever drops them it is still correct. THIS CLASS NEEDS IT MORE
       THAN MOST: its seven state tables are .bss and are filled by
       __sinit_ov006_02130758 alone, so without the constructors every one of
       the twenty-five states dispatches on a zero code word. */
    port_scene_mg_prepare(port_scene_env_want());

    if (IsMinigameActorID((unsigned)port_scene_env_want())) {
        static int armed;
        if (!armed) {
            armed = 1;
            std::atexit(port_scene_curling2_hits);
        }
    }
}

/* The registry's factory column is void *(*)(void) and so is the matched
   factory, but the forwarder is kept for the reason title_spawn,
   port_mg_curling_spawn and port_mg_flower_spawn are: it gives the seat one
   place to observe the object without the registry table growing a second
   column. */
static void *g_c2_self;

extern "C" void *port_mg_curling2_spawn(void)
{
    void *p = (void *)func_ov006_020e6bf4();
    g_c2_self = p;
    return p;
}

extern "C" void port_scene_curling2_hits(void)
{
    unsigned total = 0;
    for (int i = 0; i < 36; ++i) total += g_c2_hits[i];

    std::printf("[scene] dScMgCurling2_c slot hits: init %u, behavior %u, "
                "render %u, state-reset %u%s\n",
                g_c2_hits[0], g_c2_hits[6], g_c2_hits[9], g_c2_hits[18],
                g_c2_render_skipped
                    ? "  [RENDER SLOT NO-OP'd: SM64DS_SCENE_SLOT9=0]" : "");
    if (g_c2_init_skipped)
        std::printf("[scene] INIT SLOT NO-OP'd: SM64DS_SCENE_SLOT0=0, %u "
                    "time(s)\n", g_c2_init_skipped);

    std::printf("[scene] dScMgCurling2_c own slots entered:");
    for (int i = 0; i < 36; ++i)
        if (g_c2_hits[i]) std::printf(" %d(x%u)", i, g_c2_hits[i]);
    std::printf("   (%u total)\n", total);

    /* THE STATE INDEX IS PRINTED BECAUSE THREE BODIES AGREE ON IT and because
       it is the only thing that separates "the behavior slot ran" from "the
       state machine ran". Slot 0 sets +0x5580 to 1, slot 18 zeroes it, slot 6
       dispatches on it. A run whose behavior counter climbs while this stays
       at its initial value has a state machine that never advanced. */
    if (g_c2_self)
        std::printf("[scene] curling2 state index +0x5580 = %d, round timer "
                    "+0x55b6 = %u, sub-timer +0x55bd = %u\n",
                    *(int *)((char *)g_c2_self + 0x5580),
                    *(unsigned short *)((char *)g_c2_self + 0x55b6),
                    *(unsigned char *)((char *)g_c2_self + 0x55bd));
    else
        std::printf("[scene] curling2 state: the class never spawned\n");

    std::printf("[scene] dScMgCurling2_c state dispatch: %u call(s) through "
                "the address switches, %u UNHANDLED address(es), %u to the "
                "retired floor (must stay 0: func_ov006_020e4bd4 has a body)\n",
                port_mg_curling2_state_calls(),
                port_mg_curling2_state_unknown(),
                port_mg_curling2_state_floor());

    /* THE ROUND-END MACHINE, printed on every run and not only when it moves.
       Table data_ov006_021419d8 is the eleven-record machine run mg6 could not
       run: three of its four states entered a trap and returned, so the records
       stayed where slot 0 left them. All four now have bodies behind them --
       slot 0 -> func_ov006_020e513c, slot 1 -> func_ov006_020e5450, slot 2 ->
       the same body through the argument-forwarding veneer
       func_ov006_020e5b70 -- so these four counters moving IS the machine
       running. A zero row is a claim, and a claim only printed when it is false
       is not a measurement. */
    std::printf("[scene] curling2 round-end machine (table 021419d8): "
                "slot0 020e5e3c %u, slot1 020e5b7c %u, slot2 020e5b70 %u, "
                "slot3 020e5a0c %u; state 020e4bd4 %u\n",
                port_mg_curling2_d8_hits(0), port_mg_curling2_d8_hits(1),
                port_mg_curling2_d8_hits(2), port_mg_curling2_d8_hits(3),
                port_mg_curling2_state_4bd4());
    std::fflush(stdout);
}

/* ---- THE NAME-SPELLING FACES --------------------------------------------
 *
 * TWO ROWS, both /alternatename and neither a stand-in for a body. Both were
 * checked against port/mg_fanout_costs.txt section 10 finding 1 first -- the
 * facegen failure that aliased curling's twenty-five-entry pointer-to-member
 * STATE TABLE because its wall test is `"P8" in sym` and a struct wrapper hides
 * the P8. NEITHER TARGET IS A PAIR TABLE. This class's seven pair tables are
 * reached only from unmatched/MgCurling2_StateDispatch.cpp, which declares them
 * as `struct MgC2Pair { int code; int adj; }` at C linkage on purpose, so no
 * alias is involved in any of the seven and none could be.
 *
 * ROW 1 -- THE VTABLE UNDER ITS C++ SPELLING. src/func_ov006_020e6bf4.c is the
 * factory and its recovered header names the table _ZTV15dScMgCurling2_c, which
 * the ov006 mount defines as the plain C symbol data_ov006_0213c510. The
 * ADDRESS is what the code uses -- `p[0] = (int)_ZTV15dScMgCurling2_c` -- and
 * the address is the same one either spelling names, so an alias is exactly
 * right and no storage is involved. This is hal/scene_mg_faces.cpp:245's row
 * for _ZTV14dScMgCurling_c, one class over.
 *
 * AND THE ROW THIS FILE DELIBERATELY DOES NOT ADD, because it is already
 * there. src/func_ov006_020e6894.c (slot 0) declares
 * `extern char* func_020adc74(void*)` and hands it &data_ov006_0213c5a0, this
 * class's "/MG/..." path string; 0x020adc74 is func_ov004_020adc74 in
 * config/arm9/overlays/ov004/symbols.txt and the matched TU is already in the
 * image. That is the same misspelling port/mg_fanout_costs.txt section 6d
 * records for curling's own src/func_ov006_020e3578.c, and
 * hal/scene_mg_faces.cpp:246 already carries the alias for it. A SECOND
 * DIRECTIVE FOR THE SAME LHS IS NOT FREE: port/tools/alternatename_guard.py
 * counts every pragma under port/ as a linker input, so a duplicate would show
 * up as a second firing row for one symbol and the next lane reading the
 * guard's census would be counting spellings that do not exist. One alias, in
 * the file that derived it.
 */
#pragma comment(linker, "/alternatename:__ZTV15dScMgCurling2_c=_data_ov006_0213c510")
